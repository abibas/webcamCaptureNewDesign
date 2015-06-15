#include "videoform.h"
#include "ui_videoform.h"
using namespace std::placeholders;

VideoForm::VideoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoForm)
{
    ui->setupUi(this);
}

VideoForm::~VideoForm()
{
    delete ui;
}

void VideoForm::FrameCaptureCallback(PixelBuffer& buffer)
{
    int x = 1;
    return;
}

std::function<void(PixelBuffer& buffer)> VideoForm::getFrameCallback()
{
    return std::bind(&VideoForm::FrameCaptureCallback, this, _1);
}
