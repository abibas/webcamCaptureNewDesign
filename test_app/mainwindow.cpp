#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <fstream>
using namespace std;
std::fstream outfile;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //this->fillAvailableBackends();
}

MainWindow::~MainWindow()
{   
    delete ui;
}

void MainWindow::fillAvailableBackends()
{
    backends = BackendFactory::getAvailableBackends();
    for(size_t i = 0; i < backends.size(); ++i) {
        switch (BackendImplementation(backends[i])){
            case BackendImplementation::MediaFoundation: {
                ui->backendListComboBox->addItem("Media Foundation");
                break;
            }
            case BackendImplementation::v4l: {
                ui->backendListComboBox->addItem("V4L");
                break;
            }
            case BackendImplementation::AVFoundation: {
                ui->backendListComboBox->addItem("AV Foundation");
                break;
            }
        }
    }
}

void MainWindow::fillDeviceCombobox()
{
//    ui->deviceComboBox->clear();
//    //Devices list
//    std::vector<Device> devices = this->cap->getDevices();



//    if(devices.size() == 0) {
//      ui->deviceComboBox->addItem("No Devices Found");
//      return;
//    }

//    for(size_t i = 0; i < devices.size(); ++i) {
//      ui->deviceComboBox->addItem(devices[i].name.c_str());
//    }
}


void MainWindow::on_deviceListRefresh_clicked()
{
    this->fillDeviceCombobox();
}

void MainWindow::on_deviceComboBox_currentIndexChanged(int index)
{
//    ui->capabilityComboBox->clear();
//    std::vector<Capability> caps = cap->getCapabilities(index);
//    if(caps.size() == 0) {
//      return;
//    }

//    for(size_t i = 0; i < caps.size(); ++i) {
//      Capability& cb = caps[i];
//      QString str = QString("[%1] %2x%3 %4 fps, %5").arg(cb.capability_index).arg(cb.width).arg(cb.height).arg(cb.fps/100.0f).arg(format_to_string(cb.pixel_format).c_str());
//      //QString str = QString.fromStdString(cb.capability_index) + " " + cb.width + "x" + cb.height + " " + cb.fps/100.0f + " fps " + format_to_string(cb.pixel_format).c_str();

//      ui->capabilityComboBox->addItem(str);

//      if (cb.description.size() > 0) {
//        printf(", %s", cb.description.c_str());
//      }

//      printf("\n");
//    }
}


