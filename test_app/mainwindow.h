#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

#include "videoform.h"

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

private slots:
    void on_createBackendBtn_clicked();

    void on_createCameraBtn_clicked();

    void on_cameraListComboBox_currentIndexChanged(int index);

    void on_deleteBackendBtn_clicked();

    void on_captureVideoBtb_clicked();

    void on_deleteCameraBtn_clicked();

private:
    void fillFrameworkListCB();
    void fillCameraListCB();
    void fillCameraCapabilitiesCB();
    std::string formatToString(Format format);


    std::vector<BackendImplementation> backendList;
    std::vector<CameraInformation> cameraInfoList;
    std::vector<Capability> capabilityList;
    BackendInterface* backend;
    CameraInterface* camera;

    VideoForm *videoForm;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
