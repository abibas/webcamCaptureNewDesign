#include "videoform.h"
#include "ui_videoform.h"

using namespace std::placeholders; //for std::bind _1

#include <fstream>
using namespace std;
std::fstream outfile;

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
    /// Test of mjpeg writing.
    printf("Frame callback: %lu bytes, stride: %lu \n", buffer.nbytes, buffer.stride[0]);
    static int count = 0;
    static char fname[1024];

    sprintf(fname, "test_%04d.jpeg", count);

    std::ofstream ofs(fname, std::ios::out | std::ios::binary);
    ofs.write((char*)buffer.plane[0], buffer.nbytes);
    ofs.close();
    count++;
}

frame_callback VideoForm::getFrameCallback()
{
    return std::bind(&VideoForm::FrameCaptureCallback, this, _1);
}
