#include "cutdialog.h"
#include "ui_cutdialog.h"
#include "mainwindow.h"

CutDialog::CutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CutDialog)
{
    ui->setupUi(this);

    tmp = ((MainWindow *)parent)->getImage();

    ui->topLeftSlider->setMaximum(tmp.width());
    ui->topLeftSlider->setMinimum(0);
    ui->topLeftSlider->setValue(0);
    ui->topRightSlider->setMaximum(tmp.width());
    ui->topRightSlider->setMinimum(0);
    ui->topRightSlider->setValue(tmp.width());
    ui->bottomLeftSlider->setMaximum(tmp.height());
    ui->bottomLeftSlider->setMinimum(0);
    ui->bottomLeftSlider->setValue(0);
    ui->bottomRightSlider->setMaximum(tmp.height());
    ui->bottomRightSlider->setMinimum(0);
    ui->bottomRightSlider->setValue(tmp.height());

    ui->topLeftValue->setText(QString::number(ui->topLeftSlider->value()));
    ui->topRightValue->setText(QString::number(ui->topRightSlider->value()));
    ui->BottomLeftValue->setText(QString::number(ui->bottomLeftSlider->value()));
    ui->BottomRightValue->setText(QString::number(ui->bottomRightSlider->value()));

    connect(ui->topLeftSlider, SIGNAL(sliderMoved(int)), this, SLOT(cutImage()));
    connect(ui->topLeftSlider, SIGNAL(sliderPressed()), this, SLOT(cutImage()));
    connect(ui->topRightSlider, SIGNAL(sliderMoved(int)), this, SLOT(cutImage()));
    connect(ui->topRightSlider, SIGNAL(sliderPressed()), this, SLOT(cutImage()));
    connect(ui->bottomLeftSlider, SIGNAL(sliderMoved(int)), this, SLOT(cutImage()));
    connect(ui->bottomLeftSlider, SIGNAL(sliderPressed()), this, SLOT(cutImage()));
    connect(ui->bottomRightSlider, SIGNAL(sliderMoved(int)), this, SLOT(cutImage()));
    connect(ui->bottomRightSlider, SIGNAL(sliderPressed()), this, SLOT(cutImage()));
    connect(ui->conformButton, &QPushButton::clicked, this, &CutDialog::conform);
    connect(ui->cancelButton, &QPushButton::clicked, this, &CutDialog::cancel);
}

CutDialog::~CutDialog()
{
    delete ui;
}

void CutDialog::cutImage()
{
    ui->topLeftValue->setText(QString::number(ui->topLeftSlider->value()));
    ui->topRightValue->setText(QString::number(ui->topRightSlider->value()));
    ui->BottomLeftValue->setText(QString::number(ui->bottomLeftSlider->value()));
    ui->BottomRightValue->setText(QString::number(ui->bottomRightSlider->value()));

    MainWindow *w = (MainWindow *)parentWidget();

    QImage image = tmp.copy(ui->topLeftSlider->value(), ui->bottomLeftSlider->value(),
                            (ui->topRightSlider->value() - ui->topLeftSlider->value()),
                            (ui->bottomRightSlider->value() - ui->bottomLeftSlider->value()));
    w->setImage(image);
    w->showTemp();
}

void CutDialog::conform()
{
    MainWindow *w = (MainWindow *)parentWidget();
    w->showImage();
    isConform = true;
    this->close();
}

void CutDialog::cancel()
{
    this->close();
}

void CutDialog::closeEvent(QCloseEvent *e)
{
    if (!isConform) {
        MainWindow *w = (MainWindow *)parentWidget();
        w->setImage(tmp);
        w->showTemp();
    }
    e->accept();
}

