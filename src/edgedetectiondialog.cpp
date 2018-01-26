#include "edgedetectiondialog.h"
#include "ui_edgedetectiondialog.h"
#include "mainwindow.h"
#include "cvAlgorithms.h"

EdgeDetectionDialog::EdgeDetectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EdgeDetectionDialog)
{
    ui->setupUi(this);

    tmp = ((MainWindow *)parent)->getImage();

    btnGroup = new QButtonGroup();
    btnGroup->addButton(ui->sobelButton, 0);
    btnGroup->addButton(ui->laplaceButton, 1);
    btnGroup->addButton(ui->cannyButton, 2);

    ui->sobelButton->setChecked(true);

    connect(btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(applyEdgeDetection()));
    connect(ui->conformButton, &QPushButton::clicked, this, &EdgeDetectionDialog::conform);
    connect(ui->cancelButton, &QPushButton::clicked, this, &EdgeDetectionDialog::cancel);

    applyEdgeDetection();
}

EdgeDetectionDialog::~EdgeDetectionDialog()
{
    delete ui;
}

void EdgeDetectionDialog::applyEdgeDetection()
{
    MainWindow *w = (MainWindow *)parentWidget();

    int typeIndex = btnGroup->checkedId();
    cv::Mat src = Algo::QImage2cvMat(tmp);
    cv::Mat dst = Algo::EdgeDetection(src, typeIndex);
    QImage image = Algo::cvMat2QImage(dst);
    w->setImage(image);
    w->showTemp();
}

void EdgeDetectionDialog::conform()
{
    MainWindow *w = (MainWindow *)parentWidget();
    w->showImage();
    isConform = true;
    this->close();
}

void EdgeDetectionDialog::cancel()
{
    this->close();
}

void EdgeDetectionDialog::closeEvent(QCloseEvent *e)
{
    if (!isConform) {
        MainWindow *w = (MainWindow *)parentWidget();
        w->setImage(tmp);
        w->showTemp();
    }
    e->accept();
}
