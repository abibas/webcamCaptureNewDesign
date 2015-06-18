#ifndef VIDEOFORM_H
#define VIDEOFORM_H

#include <QWidget>
#include <pixel_buffer.h>
#include <functional>
#include <camera_interface.h>

using namespace webcam_capture;



using namespace webcam_capture;

namespace Ui {
class VideoForm;
}

class VideoForm : public QWidget
{
    Q_OBJECT

public:
    explicit VideoForm(int width, int height, QWidget *parent = 0);
    ~VideoForm();

    void FrameCaptureCallback(PixelBuffer& buffer);
    std::function<void(PixelBuffer& buffer)> getFrameCallback();
    QImage YUV422toRGBA32(PixelBuffer& buffer);

private:
    void closeEvent(QCloseEvent *);

    Ui::VideoForm *ui;
    CameraInterface* camera;
};

#endif // VIDEOFORM_H
