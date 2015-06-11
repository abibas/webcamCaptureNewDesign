#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

#include <backend_implementation.h>
#include <backend_factory.h>

using namespace webcam_capture;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();   

private slots:
    void on_deviceListRefresh_clicked();

    void on_deviceComboBox_currentIndexChanged(int index);

private:
    void fillDeviceCombobox();
    void fillAvailableBackends();
    std::vector<BackendImplementation> backends;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
