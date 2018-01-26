#include "houghdialog.h"
#include "ui_houghdialog.h"
#include "mainwindow.h"
#include "cvAlgorithms.h"

HoughDialog::HoughDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HoughDialog)
{
    ui->setupUi(this);

    tmp = ((MainWindow *)parent)->getImage();

    btnGroup = new QButtonGroup();
    btnGroup->addButton(ui->lineButton);
    btnGroup->addButton(ui->circleButton);

    ui->lineButton->setChecked(true);

    connect(ui->conformButton, &QPushButton::clicked, this, &HoughDialog::conform);
    connect(ui->cancelButton, &QPushButton::clicked, this, &HoughDialog::cancel);
    connect(ui->lineButton, SIGNAL(clicked(bool)), this, SLOT(detectLine()));
    connect(ui->circleButton, SIGNAL(clicked(bool)), this, SLOT(detectCircle()));

    detectLine();
}

HoughDialog::~HoughDialog()
{
    delete ui;
}

void HoughDialog::detectLine()
{
    MainWindow *w = (MainWindow *)parentWidget();

    cv::Mat src = Algo::QImage2cvMat(tmp);
    cv::Mat dst = Algo::houghLineTransform(src);
    QImage image = Algo::cvMat2QImage(dst);
    w->setImage(image);
    w->showTemp();
}

void HoughDialog::detectCircle()
{
    MainWindow *w = (MainWindow *)parentWidget();

    cv::Mat src = Algo::QImage2cvMat(tmp);
    cv::Mat dst = Algo::houghCircleTransform(src);
    QImage image = Algo::cvMat2QImage(dst);
    w->setImage(image);
    w->showTemp();
}

void HoughDialog::conform()
{
    MainWindow *w = (MainWindow *)parentWidget();
    w->showImage();
    isConform = true;
    this->close();
}

void HoughDialog::cancel()
{
    this->close();
}

void HoughDialog::closeEvent(QCloseEvent *e)
{
    if (!isConform) {
        MainWindow *w = (MainWindow *)parentWidget();
        w->setImage(tmp);
        w->showTemp();
    }
    e->accept();
}
