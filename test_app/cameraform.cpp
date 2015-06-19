#include "cameraform.h"
#include "ui_cameraform.h"


CameraForm::CameraForm(CameraInterface *camera, QWidget *parent) :
    camera(camera),
    QWidget(parent),
    ui(new Ui::CameraForm)
{
    ui->setupUi(this);
    videoForm = NULL;
    fillCameraCapabilitiesCB();
    fillVideoPropertySettings();

    connect(this->ui->brigtnessSlider, SIGNAL(valueChanged(int)), this, SLOT(changeBrightnessLabel(int)));
    connect(this->ui->saturationSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSaturationLabel(int)));
    connect(this->ui->contrastSlider, SIGNAL(valueChanged(int)), this, SLOT(changeContrastLabel(int)));
}

CameraForm::~CameraForm()
{
    delete camera;
    delete ui;
}


void CameraForm::fillCameraCapabilitiesCB()
{
    this->ui->capabilityComboBox->clear();
    capabilityList = camera->getCapabilities();
    for(size_t i = 0; i < capabilityList.size(); ++i) {
        QString str = QString("[%1] %2x%3 %4 fps, %5").
                arg(capabilityList[i].getCapabilityIndex()).
                arg(capabilityList[i].getWidth()).
                arg(capabilityList[i].getHeight()).
                arg(capabilityList[i].getMaxFps()/100.0f).
                arg(formatToString(capabilityList[i].getPixelFormat()).c_str());
        this->ui->capabilityComboBox->addItem(str);
    }
}

void CameraForm::fillVideoPropertySettings()
{

    //Brightness
    VideoPropertyRange vprBrightness = camera->getPropertyRange(VideoProperty::Brightness);
    int value;
    this->ui->brigtnessSlider->setRange(vprBrightness.getMinValue(),
                                        vprBrightness.getMaxValue());
    this->ui->brigtnessSlider->setTickInterval(vprBrightness.getStepValue());
    this->ui->minBrightnessLabel->setText(QString::number(vprBrightness.getMinValue()));
    this->ui->maxBrightnessLabel->setText(QString::number(vprBrightness.getMaxValue()));
    value = camera->getProperty(VideoProperty::Brightness);
    this->ui->brigtnessSlider->setValue(value);
    this->changeBrightnessLabel(value);

    //Contrast
    VideoPropertyRange vprContrast = camera->getPropertyRange(VideoProperty::Contrast);
    this->ui->contrastSlider->setRange(vprContrast.getMinValue(),
                                       vprContrast.getMaxValue());
    this->ui->contrastSlider->setTickInterval(vprContrast.getStepValue());
    this->ui->minContrastLabel->setText(QString::number(vprContrast.getMinValue()));
    this->ui->maxContrastLabel->setText(QString::number(vprContrast.getMaxValue()));
    value = camera->getProperty(VideoProperty::Contrast);
    this->ui->contrastSlider->setValue(value);
    this->changeContrastLabel(value);

    //Saturation
    VideoPropertyRange vprSaturation = camera->getPropertyRange(VideoProperty::Saturation);
    this->ui->saturationSlider->setRange(vprSaturation.getMinValue(),
                                         vprSaturation.getMaxValue());
    this->ui->saturationSlider->setTickInterval(vprSaturation.getStepValue());
    this->ui->minSaturationLabel->setText(QString::number(vprSaturation.getMinValue()));
    this->ui->maxSaturationLabel->setText(QString::number(vprSaturation.getMaxValue()));
    value = camera->getProperty(VideoProperty::Saturation);
    this->ui->saturationSlider->setValue(value);
    this->changeSaturationLabel(value);
}

void CameraForm::changeBrightnessLabel(int value)
{
    this->ui->currentBrightnessLabel->setText(QString::number(value));
}

void CameraForm::changeSaturationLabel(int value)
{
    this->ui->currentSaturationLabel->setText(QString::number(value));
}
void CameraForm::changeContrastLabel(int value)
{
    this->ui->currentContrastLabel->setText(QString::number(value));
}

void CameraForm::on_captureVideoBtb_clicked()
{
    Capability cap = capabilityList[this->ui->capabilityComboBox->currentIndex()];
    videoForm = new VideoForm(camera, cap.getWidth(), cap.getHeight());
    videoForm->setAttribute(Qt::WA_DeleteOnClose);
    videoForm->show();

    camera->open(cap, videoForm->getFrameCallback());
    camera->start();
}


void CameraForm::on_pushButton_clicked()
{
    camera->setProperty(VideoProperty::Brightness, this->ui->brigtnessSlider->value());
    camera->setProperty(VideoProperty::Contrast, this->ui->contrastSlider->value());
    camera->setProperty(VideoProperty::Saturation, this->ui->saturationSlider->value());
}

std::string  CameraForm::formatToString(Format format){
    switch(format) {
      ///Uncompressed RGB Formats
      case Format::RGB8:            return "RGB8";
      case Format::RGB555:          return "RGB555";
      case Format::RGB565:          return "RGB565";
      case Format::RGB24:           return "RGB24";
      case Format::RGB32:           return "RGB32";
      case Format::ARGB32:          return "ARGB32";
      ///YUV Formats: 8-Bit and Palettized
      case Format::AI44:            return "AI44";
      case Format::AYUV:            return "AYUV";
      case Format::I420:            return "I420";
      case Format::IYUV:            return "IYUV";
      case Format::NV11:            return "NV11";
      case Format::NV12:            return "NV12";
      case Format::UYVY:            return "UYVY";
      case Format::Y41P:            return "Y41P";
      case Format::Y41T:            return "Y41T";
      case Format::Y42T:            return "Y42T";
      case Format::YUY2:            return "YUY2";
      case Format::YV12:            return "YV12";\
      ///YUV Formats: 10-Bit and 16-Bit
      case Format::P010:            return "P010";
      case Format::P016:            return "P016";
      case Format::P210:            return "P210";
      case Format::P216:            return "P216";
      case Format::v210:            return "v210";
      case Format::v216:            return "v216";
      case Format::v40:             return "v40";
      case Format::Y210:            return "Y210";
      case Format::Y216:            return "Y216";
      case Format::Y40:             return "Y40";
      case Format::Y416:            return "Y416";
      ///Encoded Video Types
      case Format::DV25:            return "DV25";
      case Format::DV50:            return "DV50";
      case Format::DVC:             return "DVC";
      case Format::DVH1:            return "DVH1";
      case Format::DVHD:            return "DVHD";
      case Format::DVSD:            return "DVSD";
      case Format::DVSL:            return "DVSL";
      case Format::H263:            return "H263";
      case Format::H264:            return "H264";
      case Format::H264_ES:         return "H264_ES";
      case Format::HEVC:            return "HEVC";
      case Format::HEVC_ES:         return "HEVC_ES";
      case Format::M4S2:            return "M4S2";
      case Format::MJPG:            return "MJPG";
      case Format::MP43:            return "MP43";
      case Format::MP4S:            return "MP4S";
      case Format::MP4V:            return "MP4V";
      case Format::MPEG2:           return "MPEG2";
      case Format::MPG1:            return "MPG1";
      case Format::MSS1:            return "MSS1";
      case Format::MSS2:            return "MSS2";
      case Format::WMV1:            return "WMV1";
      case Format::WMV2:            return "WMV2";
      case Format::WMV3:            return "WMV3";
      case Format::WVC1:            return "WVC1";
      case Format::O420:            return "420O";
      default:                      return "Unknown format";
    }
}

