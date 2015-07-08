#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameraform.h"

#include <backend_factory.h>
#include <memory>
#include <functional>


using namespace std::placeholders; //for std::bind _1

typedef std::function<void(PixelBuffer &buffer)> frame_callback;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    backend(NULL)
{
    ui->setupUi(this);

    this->ui->deleteBackendBtn->setDisabled(true);
    this->ui->createCameraBtn->setDisabled(true);
    this->ui->startNotificationsButton->setDisabled(true);
    this->ui->stopNotificationsButton->setDisabled(true);
    fillFrameworkListCB();
}

MainWindow::~MainWindow()
{
    delete backend;
    delete ui;
}

void MainWindow::fillFrameworkListCB()
{
    backendList = BackendFactory::getAvailableBackends();

    for (size_t i = 0; i < backendList.size(); ++i) {
        switch (backendList[i]) {
            case BackendImplementation::MediaFoundation: {
                this->ui->frameworkListComboBox->addItem("Media Foundation");
                break;
            }

            case BackendImplementation::DirectShow: {
                this->ui->frameworkListComboBox->addItem("Direct Show");
                break;
            }

            case BackendImplementation::v4l: {
                this->ui->frameworkListComboBox->addItem("v4l");
                break;
            }

            case BackendImplementation::AVFoundation: {
                this->ui->frameworkListComboBox->addItem("AV Foundation");
                break;
            }
        }
    }
}

void MainWindow::fillCameraListCB()
{
    this->ui->cameraListComboBox->clear();

    for (size_t i = 0; i < cameraInfoList.size(); ++i) {
        this->ui->cameraListComboBox->addItem(cameraInfoList[i]->getCameraName().c_str());
    }
}

void MainWindow::on_createBackendBtn_clicked()
{
    this->ui->createCameraBtn->setDisabled(false);
    this->ui->deleteBackendBtn->setDisabled(false);
    backend = BackendFactory::getBackend(backendList[this->ui->frameworkListComboBox->currentIndex()]);
    cameraInfoList = backend->getAvailableCameras();
    fillCameraListCB();
    this->ui->startNotificationsButton->setDisabled(false);
    this->ui->stopNotificationsButton->setDisabled(false);
}


void MainWindow::on_createCameraBtn_clicked()
{
    CameraForm *camForm = new CameraForm(backend->getCamera(*cameraInfoList[this->ui->cameraListComboBox->currentIndex()]));

    camForm->setAttribute(Qt::WA_DeleteOnClose);
    camForm->show();
}

void MainWindow::on_cameraListComboBox_currentIndexChanged(int index)
{
    if (index >= 0) {
        this->ui->createCameraBtn->setDisabled(false);
    } else {
        this->ui->createCameraBtn->setDisabled(true);
    }
}

void MainWindow::on_deleteBackendBtn_clicked()
{
    this->ui->deleteBackendBtn->setDisabled(true);
    this->ui->createCameraBtn->setDisabled(true);
    this->ui->cameraListComboBox->clear();

    this->ui->startNotificationsButton->setDisabled(true);
    this->ui->stopNotificationsButton->setDisabled(true);

    delete this->backend;
    this->backend = NULL;
}

void MainWindow::CameraEventCaptureCallback(CameraInformation *information, CameraPlugStatus status)
{
    if (status == CameraPlugStatus::CAMERA_CONNECTED) {
        cameraInfoList.push_back(information);
    } else if (status == CameraPlugStatus::CAMERA_DISCONNECTED) {
        for (int i = 0; i < cameraInfoList.size(); i++) {
            if (*information->getUniqueId() == *cameraInfoList.at(i)->getUniqueId() &&
                    information->getCameraName() == cameraInfoList.at(i)->getCameraName()) {
                delete cameraInfoList.at(i);
                cameraInfoList.erase(cameraInfoList.begin() + i);
                delete information;
                break;
            }
        }
    }

    this->fillCameraListCB();
}

void MainWindow::on_stopNotificationsButton_clicked()
{
    backend->setAvaliableCamerasChangedCallback(nullptr); //deinit notifications callback
}

void MainWindow::on_startNotificationsButton_clicked()
{
    backend->setAvaliableCamerasChangedCallback(std::bind(&MainWindow::CameraEventCaptureCallback, this, _1, _2));
}
