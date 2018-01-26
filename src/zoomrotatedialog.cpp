#include "zoomrotatedialog.h"
#include "ui_zoomrotatedialog.h"
#include "mainwindow.h"
#include "cvAlgorithms.h"

ZoomRotateDialog::ZoomRotateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZoomRotateDialog)
{
    ui->setupUi(this);

    // 得到Image
    tmp = ((MainWindow *)parent)->getImage();

    // 初始化Slider
    ui->zoomSlider->setMaximum(this->zoomRate * 2);
    ui->zoomSlider->setValue(this->zoomRate);
    ui->rotateSlider->setMaximum(180);
    ui->rotateSlider->setMinimum(-180);
    ui->rotateSlider->setValue(this->rotateRate);
    ui->zoomValue->setText(QString::number(this->zoomRate) + "%");
    ui->rotateValue->setText(QString::number(this->rotateRate) + "° ");
    ui->nearest->setChecked(true);
    ui->linear->setChecked(false);

    // 绑定事件
    connect(ui->zoomSlider, SIGNAL(sliderMoved(int)), this, SLOT(adjust()));
    connect(ui->zoomSlider, SIGNAL(sliderPressed()), this, SLOT(adjust()));
    connect(ui->rotateSlider, SIGNAL(sliderMoved(int)), this, SLOT(adjust()));
    connect(ui->rotateSlider, SIGNAL(sliderPressed()), this, SLOT(adjust()));
    connect(ui->conformButton, &QPushButton::clicked, this, &ZoomRotateDialog::conform);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ZoomRotateDialog::cancel);
    connect(ui->nearest, SIGNAL(clicked(bool)), this, SLOT(checkNearest()));
    connect(ui->linear, SIGNAL(clicked(bool)), this, SLOT(checkLinear()));
}

ZoomRotateDialog::~ZoomRotateDialog()
{
    delete ui;
}

void ZoomRotateDialog::adjust()
{
    MainWindow *w = (MainWindow *)parentWidget();

    zoomRate = ui->zoomSlider->value();
    rotateRate = ui->rotateSlider->value();

    cv::Mat src = Algo::QImage2cvMat(tmp);
    cv::Mat dst;
    if (ui->nearest->isChecked()) {
        dst = Algo::zoomNearest(src, zoomRate);
        dst = Algo::rotateNearest(dst, rotateRate);
    }
    else if (ui->linear->isChecked()) {
        dst = Algo::zoomLinear(src, zoomRate);
        dst = Algo::rotateLinear(dst, rotateRate);
    }
    else {
        return;
    }
    QImage image = Algo::cvMat2QImage(dst);

    ui->zoomValue->setText(QString::number(this->zoomRate) + "%");
    ui->rotateValue->setText(QString::number(this->rotateRate) + "° ");

    w->setImage(image);
    w->showTemp();
}

void ZoomRotateDialog::checkNearest()
{
    ui->nearest->setChecked(true);
    ui->linear->setChecked(false);
    adjust();
}

void ZoomRotateDialog::checkLinear()
{
    ui->nearest->setChecked(false);
    ui->linear->setChecked(true);
    adjust();
}

void ZoomRotateDialog::conform()
{
    MainWindow *w = (MainWindow *)parentWidget();
    w->showImage();
    isConform = true;
    this->close();
}

void ZoomRotateDialog::cancel()
{
    this->close();
}

void ZoomRotateDialog::closeEvent(QCloseEvent *e)
{
    if (!isConform) {
        MainWindow *w = (MainWindow *)parentWidget();
        w->setImage(tmp);
        w->showTemp();
    }
    e->accept();
}
