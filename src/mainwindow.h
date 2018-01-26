#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QMouseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void open();
    void save();
    void undo();
    void redo();
    void showImage();
    void showTemp();
    void setButton();

    QImage getImage();
    void setImage(QImage image);

    void channelSeparation();
    void color2gray();
    void HSLTransform();
    void otsu();
    void doubleThreshold();
    void zoomRotate();
    void plusMinusMul();
    void contrast();
    void filter();
    void edgeDetection();
    void houghTransform();
    void cut();
    void binaryMor();
    void grayMor();

protected:
    void mouseMoveEvent(QMouseEvent *e);

private:
    Ui::MainWindow *ui;  
    QLabel *statusLabel;
    QLabel *mousePointLabel;
    QLabel *locationLabel;
    QLabel *mouseLocationLabel;
    std::vector<QImage> imgList;
    int statusIndex;
    QImage *img;
};

#endif // MAINWINDOW_H
