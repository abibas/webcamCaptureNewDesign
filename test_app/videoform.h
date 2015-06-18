#ifndef VIDEOFORM_H
#define VIDEOFORM_H

#include <QWidget>
#include <pixel_buffer.h>
#include <functional>

using namespace webcam_capture;

namespace Ui {
class VideoForm;
}

class VideoForm : public QWidget
{
    Q_OBJECT

public:
    explicit VideoForm(QWidget *parent = 0);
    ~VideoForm();
    void FrameCaptureCallback(PixelBuffer& buffer);
    std::function<void(PixelBuffer& buffer)> getFrameCallback();
    QImage YUV422toRGBA32(PixelBuffer& buffer);

private:
    Ui::VideoForm *ui;
};

#endif // VIDEOFORM_H
