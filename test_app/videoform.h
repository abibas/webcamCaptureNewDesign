#ifndef VIDEOFORM_H
#define VIDEOFORM_H

#include <QWidget>
#include <pixel_buffer.h>
#include <functional>
#include <camera_interface.h>

using namespace webcam_capture;

namespace Ui {
class VideoForm;
}

class VideoForm : public QWidget
{
    Q_OBJECT

public:
    explicit VideoForm(CameraInterface* camera, int width, int height, QWidget *parent = 0);
    ~VideoForm();

    void FrameCaptureCallback(PixelBuffer& buffer);
    std::function<void(PixelBuffer& buffer)> getFrameCallback();
    QImage YUV422toRGBA32(PixelBuffer& buffer);

    void setCapturingStatus(bool isCapturing);

private:
    Ui::VideoForm *ui;
    CameraInterface* camera;
    bool isCapturing;
};

#endif // VIDEOFORM_H
