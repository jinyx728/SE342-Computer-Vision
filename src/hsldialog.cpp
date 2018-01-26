#include "hsldialog.h"
#include "ui_hsldialog.h"
#include "cvAlgorithms.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

HSLDialog::HSLDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HSLDialog)
{
    ui->setupUi(this);

    // 得到Image
    tmp = ((MainWindow *)parent)->getImage();

    // 初始化Slider, ComboBox
    ui->hueSlider->setMaximum(this->hue * 2);
    ui->hueSlider->setValue(this->hue);
    ui->saturationSlider->setMaximum(this->saturation * 2);
    ui->saturationSlider->setValue(this->saturation);
    ui->lightnessSlider->setMaximum(this->brightness * 2);
    ui->lightnessSlider->setValue(this->brightness);
    ui->colorBox->addItem(tr("全图"));
    ui->colorBox->addItem(tr("红色"));
    ui->colorBox->addItem(tr("黄色"));
    ui->colorBox->addItem(tr("绿色"));
    ui->colorBox->addItem(tr("青色"));
    ui->colorBox->addItem(tr("蓝色"));
    ui->colorBox->addItem(tr("洋红"));
    ui->hueValue->setText(QString::number(this->hue));
    ui->saturationValue->setText(QString::number(this->saturation));
    ui->lightnessValue->setText(QString::number(this->brightness));

    // 绑定事件
    connect(ui->colorBox, SIGNAL(currentIndexChanged(int)), this, SLOT(adjustColor(int)));
    connect(ui->hueSlider, SIGNAL(sliderMoved(int)), this, SLOT(adjustHSL()));
    connect(ui->hueSlider, SIGNAL(sliderPressed()), this, SLOT(adjustHSL()));
    connect(ui->saturationSlider, SIGNAL(sliderMoved(int)), this, SLOT(adjustHSL()));
    connect(ui->saturationSlider, SIGNAL(sliderPressed()), this, SLOT(adjustHSL()));
    connect(ui->lightnessSlider, SIGNAL(sliderMoved(int)), this, SLOT(adjustHSL()));
    connect(ui->lightnessSlider, SIGNAL(sliderPressed()), this, SLOT(adjustHSL()));
    connect(ui->conformButton, &QPushButton::clicked, this, &HSLDialog::conform);
    connect(ui->cancelButton, &QPushButton::clicked, this, &HSLDialog::cancel);
}

HSLDialog::~HSLDialog()
{
    delete ui;
}

void HSLDialog::adjustColor(int index)
{
    hue = hsl.channels[index].hue + 180;
    saturation = hsl.channels[index].saturation + 100;
    brightness = hsl.channels[index].brightness + 100;

    ui->hueSlider->setValue(hue);
    ui->saturationSlider->setValue(saturation);
    ui->lightnessSlider->setValue(brightness);

    ui->hueValue->setText(QString::number(this->hue));
    ui->saturationValue->setText(QString::number(this->saturation));
    ui->lightnessValue->setText(QString::number(this->brightness));
    adjustHSL();
}

void HSLDialog::adjustHSL()
{
    MainWindow *w = (MainWindow *)parentWidget();

    hue = ui->hueSlider->value();
    saturation = ui->saturationSlider->value();
    brightness = ui->lightnessSlider->value();

    cv::Mat src = Algo::QImage2cvMat(tmp);
    cv::Mat dst;

    hsl.channels[ui->colorBox->currentIndex()].hue = hue - 180;
    hsl.channels[ui->colorBox->currentIndex()].saturation = saturation - 100;
    hsl.channels[ui->colorBox->currentIndex()].brightness = brightness - 100;

    hsl.adjust(src, dst);
    QImage image = Algo::cvMat2QImage(dst);

    ui->hueValue->setText(QString::number(this->hue));
    ui->saturationValue->setText(QString::number(this->saturation));
    ui->lightnessValue->setText(QString::number(this->brightness));

    w->setImage(image);
    w->showTemp();
}

void HSLDialog::conform()
{
    MainWindow *w = (MainWindow *)parentWidget();
    w->showImage();
    isConform = true;
    this->close();
}

void HSLDialog::cancel()
{
    this->close();
}

void HSLDialog::closeEvent(QCloseEvent *e)
{
    if (!isConform) {
        MainWindow *w = (MainWindow *)parentWidget();
        w->setImage(tmp);
        w->showTemp();
    }
    e->accept();
}
