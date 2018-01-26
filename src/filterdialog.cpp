#include "filterdialog.h"
#include "ui_filterdialog.h"
#include "mainwindow.h"
#include "cvAlgorithms.h"

FilterDialog::FilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterDialog)
{
    ui->setupUi(this);

    tmp = ((MainWindow *)parent)->getImage();

    typeGroup = new QButtonGroup();
    typeGroup->addButton(ui->meanButton, 0);
    typeGroup->addButton(ui->gaussianButton, 1);
    typeGroup->addButton(ui->medianButton, 2);

    sizeGroup = new QButtonGroup();
    sizeGroup->addButton(ui->size3Button, 0);
    sizeGroup->addButton(ui->size4Button, 1);
    sizeGroup->addButton(ui->size5Button, 2);

    ui->meanButton->setChecked(true);
    ui->size3Button->setChecked(true);

    connect(typeGroup, SIGNAL(buttonClicked(int)), this, SLOT(applyFilter()));
    connect(sizeGroup, SIGNAL(buttonClicked(int)), this, SLOT(applyFilter()));

    connect(ui->conformButton, &QPushButton::clicked, this, &FilterDialog::conform);
    connect(ui->cancelButton, &QPushButton::clicked, this, &FilterDialog::cancel);

    applyFilter();
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

void FilterDialog::applyFilter()
{
    MainWindow *w = (MainWindow *)parentWidget();

    int typeIndex = typeGroup->checkedId();
    int sizeIndex = sizeGroup->checkedId();
    cv::Mat src = Algo::QImage2cvMat(tmp);
    cv::Mat dst = Algo::smoothFilter(src, typeIndex, sizeIndex);
    QImage image = Algo::cvMat2QImage(dst);
    w->setImage(image);
    w->showTemp();
}

void FilterDialog::conform()
{
    MainWindow *w = (MainWindow *)parentWidget();
    w->showImage();
    isConform = true;
    this->close();
}

void FilterDialog::cancel()
{
    this->close();
}

void FilterDialog::closeEvent(QCloseEvent *e)
{
    if (!isConform) {
        MainWindow *w = (MainWindow *)parentWidget();
        w->setImage(tmp);
        w->showTemp();
    }
    e->accept();
}
