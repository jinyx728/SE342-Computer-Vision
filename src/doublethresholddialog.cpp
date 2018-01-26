#include "doublethresholddialog.h"
#include "ui_doublethresholddialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cvAlgorithms.h"

DoubleThresholdDialog::DoubleThresholdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DoubleThresholdDialog)
{
    ui->setupUi(this);

    // 得到Image
    tmp = ((MainWindow *)parent)->getImage();

    // 绑定事件
    ui->leftThresholdSlider->setMaximum(255);
    ui->leftThresholdSlider->setValue(85);
    ui->rightThresholdSlider->setMaximum(255);
    ui->rightThresholdSlider->setValue(170);
    ui->leftThresholdValue->setText(QString::number(ui->leftThresholdSlider->value()));
    ui->rightThresholdValue->setText(QString::number(ui->rightThresholdSlider->value()));
    connect(ui->leftThresholdSlider, SIGNAL(sliderMoved(int)), this, SLOT(adjustThreshold()));
    connect(ui->leftThresholdSlider, SIGNAL(sliderPressed()), this, SLOT(adjustThreshold()));
    connect(ui->rightThresholdSlider, SIGNAL(sliderMoved(int)), this, SLOT(adjustThreshold()));
    connect(ui->rightThresholdSlider, SIGNAL(sliderPressed()), this, SLOT(adjustThreshold()));
    connect(ui->conformButton, &QPushButton::clicked, this, &DoubleThresholdDialog::conform);
    connect(ui->cancelButton, &QPushButton::clicked, this, &DoubleThresholdDialog::cancel);
    adjustThreshold();
}

DoubleThresholdDialog::~DoubleThresholdDialog()
{
    delete ui;
}

void DoubleThresholdDialog::adjustThreshold()
{
    MainWindow *w = (MainWindow *)parentWidget();

    cv::Mat src = Algo::QImage2cvMat(tmp);
    cv::Mat dst = cv::Mat(src.rows, src.cols, CV_8UC1, cv::Scalar(0));

    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            if (src.at<uchar>(i,j) >= ui->leftThresholdSlider->value() &&
                    src.at<uchar>(i,j) <= ui->rightThresholdSlider->value()) {
                dst.at<uchar>(i,j) = 255;
            }
            else {
                dst.at<uchar>(i,j) = 0;
            }
        }
    }

    ui->leftThresholdValue->setText(QString::number(ui->leftThresholdSlider->value()));
    ui->rightThresholdValue->setText(QString::number(ui->rightThresholdSlider->value()));

    QImage image = Algo::cvMat2QImage(dst);
    w->setImage(image);
    w->showTemp();
}

void DoubleThresholdDialog::conform()
{
    MainWindow *w = (MainWindow *)parentWidget();
    w->showImage();
    isConform = true;
    this->close();
}

void DoubleThresholdDialog::cancel()
{
    this->close();
}

void DoubleThresholdDialog::closeEvent(QCloseEvent *e)
{
    if (!isConform) {
        MainWindow *w = (MainWindow *)parentWidget();
        w->setImage(tmp);
        w->showTemp();
    }
    e->accept();
}
