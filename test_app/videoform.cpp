#include "videoform.h"
#include "ui_videoform.h"

#include <QPoint>
#include <QMatrix>

using namespace std::placeholders; //for std::bind _1

//TO SAVE mjpg to file
#include <fstream>
using namespace std;
std::fstream outfile;
/////////////////////

#include <qdebug.h>

VideoForm::VideoForm(CameraInterface &camera, int width, int height, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoForm),
    camera(camera),
    isCapturing(false)
{
    ui->setupUi(this);
    this->setFixedSize(width, height);
    this->ui->videoLabel->setFixedSize(width, height);
}

VideoForm::~VideoForm()
{

    camera.stop();
    delete ui;
}

void VideoForm::FrameCaptureCallback(Frame &frame)
{
    QImage img;
    const bool displayYUY2 = true;

    if (displayYUY2) {
        img = YUV422toRGBA32(frame);
    } else {
        // display RGB24
        img = QImage(frame.plane[0], frame.width[0], frame.height[0], 3*frame.width[0], QImage::Format_RGB888).rgbSwapped();
        QMatrix matrix;

        // comment next 2 lines to remove rotation
        matrix.translate(img.rect().center().x(), img.rect().center().y());
        matrix.rotate(180);

        img = img.transformed(matrix);
    }

    this->ui->videoLabel->setPixmap(QPixmap::fromImage(img));
///EXAMPLE - SAVE mjpg to file
//    /// Test of mjpeg writing.
//    printf("Frame callback: %lu bytes, stride: %lu \n", buffer.nbytes, buffer.stride[0]);
//    static int count = 0;
//    static char fname[1024];

//    sprintf(fname, "test_%04d.jpeg", count);

//    std::ofstream ofs(fname, std::ios::out | std::ios::binary);
//    ofs.write((char*)buffer.plane[0], buffer.nbytes);
//    ofs.close();
//    count++;
}

void VideoForm::setCapturingStatus(bool isCapturing)
{
    this->isCapturing = isCapturing;
}


// https://en.wikipedia.org/wiki/YUV#Converting_between_Y.27UV_and_RGB
QImage VideoForm::YUV422toRGBA32(Frame &frame)
{
    QImage rgbImg(frame.width[0], frame.height[0], QImage::Format_RGB888);

    int w = 0;
    int h = 0;

#define CLAMP(X) (((X) > 255) ? 255 : (((X) < 0) ? 0 : (X)))
#define YUV_TO_R(Y, U, V) (CLAMP((Y) + 1.402 * ((V) - 128)))
#define YUV_TO_G(Y, U, V) (CLAMP((Y) - 0.334 * ((U) - 128) - 0.714 * ((V) - 128)))
#define YUV_TO_B(Y, U, V) (CLAMP((Y) + 1.772 * ((U) - 128)))
#define YUV_TO_QRGB(Y, U, V) (qRgb(YUV_TO_R((Y), (U), (V)), YUV_TO_G((Y), (U), (V)), YUV_TO_B((Y), (U), (V))))

    for (unsigned int i = 0 ; i <= frame.nbytes - 4 ; i += 4) {
        int y1 = frame.plane[0][i + 0];
        int u  = frame.plane[0][i + 1];
        int y2 = frame.plane[0][i + 2];
        int v  = frame.plane[0][i + 3];

        if (w == frame.width[0]) {
            w = 0;
            h ++;
        }
        rgbImg.setPixel(w++, h, YUV_TO_QRGB(y1, u, v));

        if (w == frame.width[0]) {
            w = 0;
            h ++;
        }
        rgbImg.setPixel(w++, h, YUV_TO_QRGB(y2, u, v));
    }

#undef CLAMP
#undef YUV_TO_R
#undef YUV_TO_G
#undef YUV_TO_B
#undef YUV_TO_QRGB

    return rgbImg;
}

FrameCallback VideoForm::getFrameCallback()
{
    return std::bind(&VideoForm::FrameCaptureCallback, this, _1);
}
