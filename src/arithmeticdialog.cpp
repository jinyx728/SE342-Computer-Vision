#include "arithmeticdialog.h"
#include "ui_arithmeticdialog.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "cvAlgorithms.h"

ArithmeticDialog::ArithmeticDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArithmeticDialog)
{
    ui->setupUi(this);

    QImage tmp = ((MainWindow *)parent)->getImage();

    ui->xPosValue->setText("0");
    ui->yPosValue->setText("0");
    ui->xPosValue->setValidator(new QIntValidator(0, tmp.width(), this));
    ui->yPosValue->setValidator(new QIntValidator(0, tmp.height(), this));

    connect(ui->plusButton, &QPushButton::clicked, this, &ArithmeticDialog::plus);
    connect(ui->minusButton, &QPushButton::clicked, this, &ArithmeticDialog::minus);
    connect(ui->multiplyButton, &QPushButton::clicked, this, &ArithmeticDialog::multiply);
}

ArithmeticDialog::~ArithmeticDialog()
{
    delete ui;
}

void ArithmeticDialog::plus()
{
    MainWindow *w = (MainWindow *)parentWidget();
    QImage oriImage = w->getImage();
    QImage addImage;
    int x = ui->xPosValue->text().toInt();
    int y = ui->yPosValue->text().toInt();
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开图片"), ".", tr("Images (*.png *.xpm *.jpg *.bmp *.jpeg)"));
    if (!fileName.isEmpty()) {
        if(!addImage.load(fileName)) {
            QMessageBox::information(this, tr("Error"), tr("打开图像失败!"));
            this->close();
            return;
        }
        cv::Mat ori = Algo::QImage2cvMat(oriImage).clone();
        cv::Mat add = Algo::QImage2cvMat(addImage);
        if (ori.type() != add.type()) {
            QMessageBox::information(this, tr("Error"), tr("图像通道不匹配！"));
            this->close();
            return;
        }
        cv::Mat result = Algo::plus(ori, add, x, y);
        w->setImage(Algo::cvMat2QImage(result));
        w->showImage();
        this->close();
    }
}

void ArithmeticDialog::minus()
{
    MainWindow *w = (MainWindow *)parentWidget();
    QImage oriImage = w->getImage();
    QImage addImage;
    int x = ui->xPosValue->text().toInt();
    int y = ui->yPosValue->text().toInt();
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开图片"), ".", tr("Images (*.png *.xpm *.jpg *.bmp *.jpeg)"));
    if (!fileName.isEmpty()) {
        if(!addImage.load(fileName)) {
            QMessageBox::information(this, tr("Error"), tr("打开图像失败!"));
            this->close();
            return;
        }
        cv::Mat ori = Algo::QImage2cvMat(oriImage).clone();
        cv::Mat add = Algo::QImage2cvMat(addImage);
        if (ori.type() != add.type()) {
            QMessageBox::information(this, tr("Error"), tr("图像通道不匹配！"));
            this->close();
            return;
        }
        cv::Mat result = Algo::minus(ori, add, x, y);
        w->setImage(Algo::cvMat2QImage(result));
        w->showImage();
        this->close();
    }
}

void ArithmeticDialog::multiply()
{
    MainWindow *w = (MainWindow *)parentWidget();
    QImage oriImage = w->getImage();
    QImage addImage;
    int x = ui->xPosValue->text().toInt();
    int y = ui->yPosValue->text().toInt();
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开图片"), ".", tr("Images (*.png *.xpm *.jpg *.bmp *.jpeg)"));
    if (!fileName.isEmpty()) {
        if(!addImage.load(fileName)) {
            QMessageBox::information(this, tr("Error"), tr("打开图像失败!"));
            this->close();
            return;
        }
        cv::Mat ori = Algo::QImage2cvMat(oriImage).clone();
        cv::Mat add = Algo::QImage2cvMat(addImage);
        if (ori.type() != add.type()) {
            QMessageBox::information(this, tr("Error"), tr("图像通道不匹配！"));
            this->close();
            return;
        }
        cv::Mat result = Algo::multiply(ori, add, x, y);
        w->setImage(Algo::cvMat2QImage(result));
        w->showImage();
        this->close();
    }
}
