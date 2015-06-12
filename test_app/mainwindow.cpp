#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <backend_factory.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->createCameraBtn->setVisible(false);
    fillFrameworkListCB();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fillFrameworkListCB()
{
    backendList = BackendFactory::getAvailableBackends();

    for(size_t i = 0; i < backendList.size(); ++i) {
        switch (backendList[i]) {
            case BackendImplementation::MediaFoundation: {
                this->ui->frameworkListComboBox->addItem("Media Foundation");
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
    cameraInfoList = backend->getAvailableCameras();
    for(size_t i = 0; i < cameraInfoList.size(); ++i) {
        this->ui->cameraListComboBox->addItem(cameraInfoList[i].getCameraName().c_str());
    }
}

void MainWindow::fillCameraCapabilitiesCB()
{
    capabilityList = camera->getCapabilities();
    for(size_t i = 0; i < capabilityList.size(); ++i) {
        QString str = QString("[%1] %2x%3 %4 fps, %5").
                arg(capabilityList[i].getCapabilityIndex()).
                arg(capabilityList[i].getWidth()).
                arg(capabilityList[i].getHeight()).
                arg(capabilityList[i].getFps()/100.0f).
                arg(formatToString(capabilityList[i].getPixelFormat()).c_str());
        this->ui->capabilityComboBox->addItem(str);

    }
}

std::string  MainWindow::formatToString(Format format){
    switch(format) {
      case Format::UYVY422:         return "UYVY422";
      case Format::YUYV422:         return "YUYV422";
      case Format::YUV422P:         return "YUV422P";
      case Format::YUV420P:         return "YUV420P";
      case Format::YUV420BP:        return "YUV420BP";
      case Format::YUVJ420P:        return "YUVJ420P";
      case Format::YUVJ420BP:       return "YUVJ420BP";
      case Format::ARGB32:          return "ARGB32";
      case Format::BGRA32:          return "BGRA32";
      case Format::RGB24:           return "RGB24";
      case Format::JPEG_OPENDML:    return "JPEG_OPENDML";
      case Format::H264:            return "H264";
      case Format::MJPEG:           return "MJPEG";
      default:                      return "UNKNOWN_FORMAT";
    }
}

void MainWindow::on_createBackendBtn_clicked()
{
    backend = BackendFactory::getBackend(backendList[this->ui->frameworkListComboBox->currentIndex()]);
    fillCameraListCB();
}




void MainWindow::on_createCameraBtn_clicked()
{
    camera = backend->getCamera(cameraInfoList[this->ui->cameraListComboBox->currentIndex()]);
    fillCameraCapabilitiesCB();
}

void MainWindow::on_cameraListComboBox_currentIndexChanged(int index)
{
    if (index >= 0) {
        this->ui->createCameraBtn->setVisible(true);
    } else {
        this->ui->createCameraBtn->setVisible(false);
    }
}
