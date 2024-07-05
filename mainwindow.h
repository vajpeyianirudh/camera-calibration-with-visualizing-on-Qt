#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/flann.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/reg/map.hpp>
#include <opencv2/aruco.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>

#ifdef _WIN32
    // For Windows Sleep(ms)
    #include <windows.h>
#else
    // For POSIX sleep(sec)
    #include <unistd.h>
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace cv;
using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void displayVideo();
    bool loadCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoefficients);

private:
    Ui::MainWindow *ui;
    QImage imdisplay;  //This will create QImage which is shown in Qt label
    QTimer* Timer;   // A timer is needed in GUI application

    VideoCapture cap;
    Mat frame;

    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_4X4_50);
    vector<int> markerIds;
    vector<vector<Point2f>> markerCorners, rejectedCandidates;
    Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
    const float calibSquareDim = 0.01908f;
    const float arucoSquareDim = 0.02489f;
    vector<Vec3d> rotationVectors, translationVectors;
    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
    Mat distanceCoefficients;

};
#endif // MAINWINDOW_H
