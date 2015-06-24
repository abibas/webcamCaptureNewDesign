#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

#include <backend_implementation.h>
#include <camera_information.h>
#include <capability.h>

#include <backend_interface.h>
#include <camera_interface.h>

using namespace webcam_capture;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void CameraEventCaptureCallback(CameraInformation& information);

private slots:
    void on_createBackendBtn_clicked();
    void on_createCameraBtn_clicked();
    void on_cameraListComboBox_currentIndexChanged(int index);
    void on_deleteBackendBtn_clicked();

private:
    void fillFrameworkListCB();
    void fillCameraListCB();
    std::string formatToString(Format format);


    std::vector<BackendImplementation> backendList;
    std::vector<CameraInformation> cameraInfoList;
    BackendInterface* backend;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
