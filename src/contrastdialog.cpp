#include "contrastdialog.h"
#include "ui_contrastdialog.h"
#include "mainwindow.h"
#include "cvAlgorithms.h"
#include <vector>
#include <QPainter>

ContrastDialog::ContrastDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContrastDialog)
{
    ui->setupUi(this);

    connect(ui->linearButton, SIGNAL(clicked(bool)), this, SLOT(setLinear()));
    connect(ui->expButton, SIGNAL(clicked(bool)), this, SLOT(setExp()));
    connect(ui->logButton, SIGNAL(clicked(bool)), this, SLOT(setLog()));

    connect(ui->leftPointSlider, SIGNAL(sliderMoved(int)), SLOT(adjustLinear()));
    connect(ui->leftPointSlider, SIGNAL(sliderPressed()), SLOT(adjustLinear()));
    connect(ui->leftSlopeSlider, SIGNAL(sliderMoved(int)), SLOT(adjustLinear()));
    connect(ui->leftSlopeSlider, SIGNAL(sliderPressed()), SLOT(adjustLinear()));
    connect(ui->middleSlopeSlider, SIGNAL(sliderMoved(int)), SLOT(adjustLinear()));
    connect(ui->middleSlopeSlider, SIGNAL(sliderPressed()), SLOT(adjustLinear()));
    connect(ui->rightPointSlider, SIGNAL(sliderMoved(int)), SLOT(adjustLinear()));
    connect(ui->rightPointSlider, SIGNAL(sliderPressed()), SLOT(adjustLinear()));
    connect(ui->rightSlopeSlider, SIGNAL(sliderMoved(int)), SLOT(adjustLinear()));
    connect(ui->rightSlopeSlider, SIGNAL(sliderPressed()), SLOT(adjustLinear()));
    connect(ui->expSlider, SIGNAL(sliderMoved(int)), SLOT(adjustExp()));
    connect(ui->expSlider, SIGNAL(sliderPressed()), SLOT(adjustExp()));
    connect(ui->logSlider, SIGNAL(sliderMoved(int)), SLOT(adjustLog()));
    connect(ui->logSlider, SIGNAL(sliderPressed()), SLOT(adjustLog()));

    connect(ui->equalizationButton, &QPushButton::clicked, this, &ContrastDialog::equalization);
    connect(ui->conformButton, &QPushButton::clicked, this, &ContrastDialog::conform);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ContrastDialog::cancel);

    tmp = ((MainWindow *)parent)->getImage();
    drawHistogram(tmp);
    setLinear();
}

ContrastDialog::~ContrastDialog()
{
    delete ui;
}

void ContrastDialog::drawHistogram(QImage img)
{
    std::vector<int> count(256, 0);
    for (int i = 0; i < img.width(); i++) {
        for (int j = 0; j < img.height(); j++) {
            int index = 255 - img.pixelColor(i,j).black();
            count[index] += 1;
        }
    }
    std::vector<int> sortcount = count;
    std::sort(sortcount.begin(),sortcount.end());
    int maxcount = sortcount[sortcount.size()-1];

    QImage *hist = new QImage(ui->imageLabel->width(), ui->imageLabel->height(), QImage::Format_RGB888);
    QPainter p(hist);
    p.setBrush(QBrush(QColor(255,255,255)));
    p.drawRect(0, 0, hist->width(), hist->height());

    p.translate(0,hist->height());
    p.drawLine(0,0,100,100);
    int wid=hist->width();
    int hei=hist->height();

    float xstep = float(wid-20) / 256;
    float ystep = float(hei-20) / maxcount;

    for (int i = 0; i != 256; i++) {
        p.drawRect(10+i*xstep,-10,xstep,-10-ystep*count[i]);
        if(i % 32 == 0||i==255) {
            p.drawText(QPointF(10+(i-0.5)*xstep,0),QString::number(i));
        }
    }

    QPixmap pixmap = QPixmap::fromImage(*hist);
    ui->imageLabel->setPixmap(pixmap);
}

void ContrastDialog::setLinear()
{
    MainWindow *w = (MainWindow *)parentWidget();

    ui->linearButton->setChecked(true);
    ui->expButton->setChecked(false);
    ui->logButton->setChecked(false);

    ui->expSlider->setVisible(false);
    ui->logSlider->setVisible(false);
    ui->explogLabel->setVisible(false);
    ui->explogValue->setVisible(false);
    ui->leftPointLabel->setVisible(true);
    ui->leftSlopeLabel->setVisible(true);
    ui->middleSlopeLabel->setVisible(true);
    ui->rightPointLabel->setVisible(true);
    ui->rightSlopeLabel->setVisible(true);
    ui->leftPointSlider->setVisible(true);
    ui->leftSlopeSlider->setVisible(true);
    ui->middleSlopeSlider->setVisible(true);
    ui->rightPointSlider->setVisible(true);
    ui->rightSlopeSlider->setVisible(true);
    ui->leftPointValue->setVisible(true);
    ui->leftSlopeValue->setVisible(true);
    ui->rightPointValue->setVisible(true);
    ui->rightSlopeValue->setVisible(true);
    ui->middleSlopeValue->setVisible(true);

    ui->leftPointSlider->setMinimum(0);
    ui->leftPointSlider->setMaximum(255);
    ui->leftPointSlider->setValue(128);

    ui->rightPointSlider->setMinimum(0);
    ui->rightPointSlider->setMaximum(255);
    ui->rightPointSlider->setValue(128);

    ui->leftSlopeSlider->setMinimum(0);
    ui->leftSlopeSlider->setMaximum(20);
    ui->leftSlopeSlider->setValue(10);

    ui->rightSlopeSlider->setMinimum(0);
    ui->rightSlopeSlider->setMaximum(20);
    ui->rightSlopeSlider->setValue(10);

    ui->middleSlopeSlider->setMinimum(0);
    ui->middleSlopeSlider->setMaximum(20);
    ui->middleSlopeSlider->setValue(10);

    ui->leftPointValue->setText(QString::number(ui->leftPointSlider->value()));
    ui->leftSlopeValue->setText(QString::number(ui->leftSlopeSlider->value() / 10.0));
    ui->middleSlopeValue->setText(QString::number(ui->middleSlopeSlider->value() / 10.0));
    ui->rightPointValue->setText(QString::number(ui->rightPointSlider->value()));
    ui->rightSlopeValue->setText(QString::number(ui->rightSlopeSlider->value() / 10.0));

    float ls = ui->leftSlopeSlider->value() / 10.0;
    float lp = ui->leftPointSlider->value();
    float ms = ui->middleSlopeSlider->value() / 10.0;
    float rp = ui->rightPointSlider->value();
    float rs = ui->rightSlopeSlider->value() / 10.0;
    cv::Mat mat = Algo::QImage2cvMat(tmp);
    cv::Mat result = Algo::contrastLinear(mat, ls, lp, ms, rp, rs);
    QImage image = Algo::cvMat2QImage(result);
    drawHistogram(image);
    w->setImage(image);
    w->showTemp();
}

void ContrastDialog::adjustLinear()
{
    MainWindow *w = (MainWindow *)parentWidget();

    ui->leftPointValue->setText(QString::number(ui->leftPointSlider->value()));
    ui->leftSlopeValue->setText(QString::number(ui->leftSlopeSlider->value() / 10.0));
    ui->middleSlopeValue->setText(QString::number(ui->middleSlopeSlider->value() / 10.0));
    ui->rightPointValue->setText(QString::number(ui->rightPointSlider->value()));
    ui->rightSlopeValue->setText(QString::number(ui->rightSlopeSlider->value() / 10.0));

    float ls = ui->leftSlopeSlider->value() / 10.0;
    float lp = ui->leftPointSlider->value();
    float ms = ui->middleSlopeSlider->value() / 10.0;
    float rp = ui->rightPointSlider->value();
    float rs = ui->rightSlopeSlider->value() / 10.0;
    cv::Mat mat = Algo::QImage2cvMat(tmp);
    cv::Mat result = Algo::contrastLinear(mat, ls, lp, ms, rp, rs);
    QImage image = Algo::cvMat2QImage(result);
    drawHistogram(image);
    w->setImage(image);
    w->showTemp();
}

void ContrastDialog::setExp()
{
    MainWindow *w = (MainWindow *)parentWidget();

    ui->linearButton->setChecked(false);
    ui->expButton->setChecked(true);
    ui->logButton->setChecked(false);

    ui->expSlider->setVisible(true);
    ui->logSlider->setVisible(false);
    ui->explogLabel->setVisible(true);
    ui->explogValue->setVisible(true);
    ui->leftPointLabel->setVisible(false);
    ui->leftSlopeLabel->setVisible(false);
    ui->middleSlopeLabel->setVisible(false);
    ui->rightPointLabel->setVisible(false);
    ui->rightSlopeLabel->setVisible(false);
    ui->leftPointSlider->setVisible(false);
    ui->leftSlopeSlider->setVisible(false);
    ui->middleSlopeSlider->setVisible(false);
    ui->rightPointSlider->setVisible(false);
    ui->rightSlopeSlider->setVisible(false);
    ui->leftPointValue->setVisible(false);
    ui->leftSlopeValue->setVisible(false);
    ui->rightPointValue->setVisible(false);
    ui->rightSlopeValue->setVisible(false);
    ui->middleSlopeValue->setVisible(false);

    ui->expSlider->setMinimum(50);
    ui->expSlider->setMaximum(150);
    ui->expSlider->setValue(100);

    ui->explogValue->setText(QString::number(ui->expSlider->value() / 100.0));

    float x = ui->expSlider->value() / 100.0;
    cv::Mat mat = Algo::QImage2cvMat(tmp);
    cv::Mat result = Algo::contrastExp(mat, x);
    QImage image = Algo::cvMat2QImage(result);
    drawHistogram(image);
    w->setImage(image);
    w->showTemp();
}

void ContrastDialog::adjustExp()
{
    MainWindow *w = (MainWindow *)parentWidget();

    ui->explogValue->setText(QString::number(ui->expSlider->value() / 100.0));

    float x = ui->expSlider->value() / 100.0;
    cv::Mat mat = Algo::QImage2cvMat(tmp);
    cv::Mat result = Algo::contrastExp(mat, x);
    QImage image = Algo::cvMat2QImage(result);
    drawHistogram(image);
    w->setImage(image);
    w->showTemp();
}

void ContrastDialog::setLog()
{
    MainWindow *w = (MainWindow *)parentWidget();

    ui->linearButton->setChecked(false);
    ui->expButton->setChecked(false);
    ui->logButton->setChecked(true);

    ui->expSlider->setVisible(false);
    ui->logSlider->setVisible(true);
    ui->explogLabel->setVisible(true);
    ui->explogValue->setVisible(true);
    ui->leftPointLabel->setVisible(false);
    ui->leftSlopeLabel->setVisible(false);
    ui->middleSlopeLabel->setVisible(false);
    ui->rightPointLabel->setVisible(false);
    ui->rightSlopeLabel->setVisible(false);
    ui->leftPointSlider->setVisible(false);
    ui->leftSlopeSlider->setVisible(false);
    ui->middleSlopeSlider->setVisible(false);
    ui->rightPointSlider->setVisible(false);
    ui->rightSlopeSlider->setVisible(false);
    ui->leftPointValue->setVisible(false);
    ui->leftSlopeValue->setVisible(false);
    ui->rightPointValue->setVisible(false);
    ui->rightSlopeValue->setVisible(false);
    ui->middleSlopeValue->setVisible(false);

    ui->logSlider->setMinimum(1);
    ui->logSlider->setMaximum(500);
    ui->logSlider->setValue(250);

    ui->explogValue->setText(QString::number(ui->logSlider->value() / 10.0));

    float x = ui->logSlider->value() / 10.0;
    cv::Mat mat = Algo::QImage2cvMat(tmp);
    cv::Mat result = Algo::contrastLog(mat, x);
    QImage image = Algo::cvMat2QImage(result);
    drawHistogram(image);
    w->setImage(image);
    w->showTemp();
}

void ContrastDialog::adjustLog()
{
    MainWindow *w = (MainWindow *)parentWidget();

    ui->explogValue->setText(QString::number(ui->logSlider->value() / 10.0));

    float x = ui->logSlider->value() / 10.0;
    cv::Mat mat = Algo::QImage2cvMat(tmp);
    cv::Mat result = Algo::contrastLog(mat, x);
    QImage image = Algo::cvMat2QImage(result);
    drawHistogram(image);
    w->setImage(image);
    w->showTemp();
}

void ContrastDialog::equalization()
{
    MainWindow *w = (MainWindow *)parentWidget();

    ui->linearButton->setChecked(false);
    ui->expButton->setChecked(false);
    ui->logButton->setChecked(false);

    ui->expSlider->setVisible(false);
    ui->logSlider->setVisible(false);
    ui->explogLabel->setVisible(false);
    ui->explogValue->setVisible(false);
    ui->leftPointLabel->setVisible(false);
    ui->leftSlopeLabel->setVisible(false);
    ui->middleSlopeLabel->setVisible(false);
    ui->rightPointLabel->setVisible(false);
    ui->rightSlopeLabel->setVisible(false);
    ui->leftPointSlider->setVisible(false);
    ui->leftSlopeSlider->setVisible(false);
    ui->middleSlopeSlider->setVisible(false);
    ui->rightPointSlider->setVisible(false);
    ui->rightSlopeSlider->setVisible(false);
    ui->leftPointValue->setVisible(false);
    ui->leftSlopeValue->setVisible(false);
    ui->rightPointValue->setVisible(false);
    ui->rightSlopeValue->setVisible(false);
    ui->middleSlopeValue->setVisible(false);

    cv::Mat mat = Algo::QImage2cvMat(tmp);
    cv::Mat result = Algo::equalization(mat);
    QImage image = Algo::cvMat2QImage(result);
    drawHistogram(image);
    w->setImage(image);
    w->showTemp();
}

void ContrastDialog::conform()
{
    MainWindow *w = (MainWindow *)parentWidget();
    w->showImage();
    isConform = true;
    this->close();
}

void ContrastDialog::cancel()
{
    this->close();
}

void ContrastDialog::closeEvent(QCloseEvent *e)
{
    if (!isConform) {
        MainWindow *w = (MainWindow *)parentWidget();
        w->setImage(tmp);
        w->showTemp();
    }
    e->accept();
}
