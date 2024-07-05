#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Timer = new QTimer(this);
    connect(Timer, SIGNAL(timeout()), this, SLOT(displayVideo()));

    cap.open(0);
//    cap.set(CAP_PROP_FRAME_HEIGHT, 600);
//    cap.set(CAP_PROP_FRAME_WIDTH, 800);
    if(!cap.isOpened())
     {
       cout << "Error opening video stream" << endl;
     }

    loadCameraCalibration("MyCalibratedCamera.txt", cameraMatrix, distanceCoefficients);

    Timer->start();
}

MainWindow::~MainWindow()
{
    cap.release();
    delete ui;
}

void MainWindow::displayVideo(){

    if(!cap.isOpened())
    {
        cout << "Error opening video stream" << endl;
        ui->videoWidget->setText("Error opening video stream!");
        return;
    }

    cap >> frame;
    if (!frame.empty())
    {
        rotate(frame, frame, ROTATE_90_CLOCKWISE);
        cvtColor(frame, frame, COLOR_BGR2RGB);

        cout << frame.size() << endl;
        aruco::detectMarkers(frame, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
        aruco::estimatePoseSingleMarkers(markerCorners, arucoSquareDim, cameraMatrix, distanceCoefficients, rotationVectors, translationVectors);
        Mat outputFrame = frame.clone();
        //aruco::drawDetectedMarkers(outputFrame, markerCorners, markerIds);

        // estimating marker centers
        vector<Point2f>markerCenters {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};

        if(!markerCorners.empty()){
            for (int i = 0; i<markerCorners.size(); i++ ) {
//                cout << "marker corners of id " << markerIds.at(i) << " = " << endl << markerCorners.at(i) << endl;
                for (int j = 0; j < markerCorners.at(i).size(); j++) {
                    markerCenters.at(i) = markerCorners.at(i).at(j) + markerCenters.at(i);
                }
                markerCenters.at(i) = markerCenters.at(i) / 4;
                cout << "marker center of id " << markerIds.at(i) << " = " << markerCenters.at(i) << endl;
                circle(outputFrame, markerCenters.at(i), 16, Scalar(200, 0, 0), FILLED);
            }
        }

        // drawing transparent red circle on marker centers
        addWeighted(outputFrame, 0.75, frame, 0.25, 0, outputFrame);

        QImage currentFrame= QImage((uchar*) outputFrame.data, outputFrame.cols, outputFrame.rows, outputFrame.step, QImage::Format_RGB888);
        ui->videoWidget->setPixmap(QPixmap::fromImage(currentFrame));
        ui->videoWidget->setScaledContents(true);
        ui->videoWidget->show();

        // drawing marker pose
        for(int i = 0; i < markerIds.size(); i++){
            // X axis : red, Y axis : green, Z axis : blue
            aruco::drawAxis(frame, cameraMatrix, distanceCoefficients, rotationVectors[i], translationVectors[i], 0.03f);
        }

//        Mat newCameraMatrix = getOptimalNewCameraMatrix(cameraMatrix, distanceCoefficients, frame.size(), 1, frame.size());
//        Mat frame2;
//        undistort(frame, frame2, cameraMatrix, distanceCoefficients);

        QImage currentFrame2= QImage((uchar*) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        ui->calibWidget->setPixmap(QPixmap::fromImage(currentFrame2));
        ui->calibWidget->setScaledContents(true);
        ui->calibWidget->show();
    }
}

bool MainWindow::loadCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoefficients){
    ifstream inStream(name);
    if(inStream){
        uint16_t rows, columns;

        inStream >> rows;
        inStream >> columns;

        cameraMatrix = Mat(Size(rows, columns), CV_64F);

        for(int r = 0; r < rows; r++){
            for (int c = 0; c < columns; c++){
                double read = 0.0f;
                inStream >> read;
                cameraMatrix.at<double>(r, c) = read;
                cout << cameraMatrix.at<double>(r, c) << "\n";
            }
        }

        inStream >> rows;
        inStream >> columns;

        distanceCoefficients = Mat::zeros(rows, columns, CV_64F);

        for(int r = 0; r < rows; r++){
            for (int c = 0; c < columns; c++){
                double read = 0.0f;
                inStream >> read;
                distanceCoefficients.at<double>(r, c) = read;
                cout << distanceCoefficients.at<double>(r, c) << "\n";
            }
        }
        inStream.close();
        return true;
    }
    return false;
}

