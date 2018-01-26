#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hsldialog.h"
#include "doublethresholddialog.h"
#include "zoomrotatedialog.h"
#include "arithmeticdialog.h"
#include "contrastdialog.h"
#include "filterdialog.h"
#include "edgedetectiondialog.h"
#include "houghdialog.h"
#include "cutdialog.h"
#include "binarymordialog.h"
#include "grayscalemordialog.h"
#include "cvAlgorithms.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>
#include <QScrollBar>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // MenuBar 初始化
    QAction *openAction = new QAction(tr("&打开图片"),this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::open);

    QAction *saveAction = new QAction(tr("&保存图片"),this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::save);

    QAction *undoAction = new QAction(tr("&撤销"),this);
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered, this, &MainWindow::undo);

    QAction *redoAction = new QAction(tr("&恢复"),this);
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, &QAction::triggered, this, &MainWindow::redo);

    QMenu *file = menuBar()->addMenu(tr("&文件"));
    file->addAction(openAction);
    file->addAction(saveAction);

    QMenu *edit = menuBar()->addMenu(tr("&编辑"));
    edit->addAction(undoAction);
    edit->addAction(redoAction);

    ui->menuBar->addMenu(file);
    ui->menuBar->addMenu(edit);

    // StatusBar 初始化
    locationLabel = new QLabel();
    locationLabel->setText(tr("位置："));
    locationLabel->setStyleSheet("color:white");
    locationLabel->setFixedWidth(30);

    mouseLocationLabel = new QLabel();
    mouseLocationLabel->setText("");
    mouseLocationLabel->setStyleSheet("color:white");
    mouseLocationLabel->setFixedWidth(70);

    statusLabel = new QLabel();
    statusLabel->setText(tr("像素值："));
    statusLabel->setStyleSheet("color:white");
    statusLabel->setFixedWidth(50);

    mousePointLabel = new QLabel();
    mousePointLabel->setText("");
    mousePointLabel->setStyleSheet("color:white");
    mousePointLabel->setFixedWidth(150);

    ui->statusBar->addPermanentWidget(locationLabel);
    ui->statusBar->addPermanentWidget(mouseLocationLabel);
    ui->statusBar->addPermanentWidget(statusLabel);
    ui->statusBar->addPermanentWidget(mousePointLabel);

    // 设置鼠标跟随
    setMouseTracking(true);
    ui->centralWidget->setMouseTracking(true);
    ui->scrollArea->setMouseTracking(true);
    ui->scrollAreaWidgetContents->setMouseTracking(true);
    ui->imageLabel->setMouseTracking(true);

    // 初始化载入图
    img=new QImage;

    // 为按钮绑定事件
    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::open);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::save);
    connect(ui->undoButton, &QPushButton::clicked, this, &MainWindow::undo);
    connect(ui->redoButton, &QPushButton::clicked, this, &MainWindow::redo);
    connect(ui->channelButton, &QPushButton::clicked, this, &MainWindow::channelSeparation);
    connect(ui->toGrayButton, &QPushButton::clicked, this, &MainWindow::color2gray);
    connect(ui->HSLButton, &QPushButton::clicked, this, &MainWindow::HSLTransform);
    connect(ui->otsuButton, &QPushButton::clicked, this, &MainWindow::otsu);
    connect(ui->thresholdButton, &QPushButton::clicked, this, &MainWindow::doubleThreshold);
    connect(ui->zoomRotateButton, &QPushButton::clicked, this, &MainWindow::zoomRotate);
    connect(ui->plusMinusMulButton, &QPushButton::clicked, this, &MainWindow::plusMinusMul);
    connect(ui->contrastButton, &QPushButton::clicked, this, &MainWindow::contrast);
    connect(ui->filterButton, &QPushButton::clicked, this, &MainWindow::filter);
    connect(ui->edgeButton, &QPushButton::clicked, this, &MainWindow::edgeDetection);
    connect(ui->houghButton, &QPushButton::clicked, this, &MainWindow::houghTransform);
    connect(ui->cutButton, &QPushButton::clicked, this, &MainWindow::cut);
    connect(ui->binarymorButton, &QPushButton::clicked, this, &MainWindow::binaryMor);
    connect(ui->graymorButton, &QPushButton::clicked, this, &MainWindow::grayMor);

    ui->channelButton->setEnabled(false);
    ui->toGrayButton->setEnabled(false);
    ui->HSLButton->setEnabled(false);
    ui->otsuButton->setEnabled(false);
    ui->thresholdButton->setEnabled(false);
    ui->zoomRotateButton->setEnabled(false);
    ui->plusMinusMulButton->setEnabled(false);
    ui->contrastButton->setEnabled(false);
    ui->filterButton->setEnabled(false);
    ui->edgeButton->setEnabled(false);
    ui->houghButton->setEnabled(false);
    ui->cutButton->setEnabled(false);
    ui->binarymorButton->setEnabled(false);
    ui->graymorButton->setEnabled(false);

    // Undo/Redo 操作
    statusIndex = -1;
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage MainWindow::getImage()
{
    return *img;
}

void MainWindow::setImage(QImage image)
{
    *img = image;
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    // 获取鼠标点的像素值
    int imgLeft = ui->scrollArea->x();
    int imgRight = ui->scrollArea->x() + ui->scrollArea->width();
    int imgTop = ui->scrollArea->y();
    int imgBottom = ui->scrollArea->y() + ui->scrollArea->width();
    if (!img->isNull() && e->x() >= imgLeft && e->x() <= imgRight && e->y() >= imgTop && e->y() <= imgBottom)  {
        mouseLocationLabel->setText("(" + QString::number(e->x() - imgLeft + ui->scrollArea->horizontalScrollBar()->value()) + ", "
                                    + QString::number(e->y() - imgTop + ui->scrollArea->verticalScrollBar()->value()) + ")");
        QColor pixelColor = img->pixelColor(e->x() - imgLeft + ui->scrollArea->horizontalScrollBar()->value(),
                                            e->y() - imgTop + ui->scrollArea->verticalScrollBar()->value());
        if (img->isGrayscale()) {
            if (Algo::isBinary(*img)) {
                mousePointLabel->setText("Binary Scale: " + QString::number(pixelColor.black() / 255));
            }
            else {
                mousePointLabel->setText("Gray Scale: " + QString::number(255-pixelColor.black()));
            }
        }
        else {
            mousePointLabel->setText("R: " + QString::number(pixelColor.red()) +
                                     "  G: " + QString::number(pixelColor.green()) +
                                     "  B: " + QString::number(pixelColor.blue()));
        }
    }
    else {
        mousePointLabel->setText("");
    }
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开图片"), ".", tr("Images (*.png *.xpm *.jpg *.bmp *.jpeg)"));
    if (!fileName.isEmpty()) {
        if(!img->load(fileName)) {
            QMessageBox::information(this, tr("Error"), tr("打开图像失败!"));
            delete img;
            return;
        }
        showImage();
    }
}

void MainWindow::save()
{
    if (!img->isNull()){
        QString fileName = QFileDialog::getSaveFileName(this,tr("保存图片"),tr("未命名.jpg"),tr("Images (*.png *.xpm *.jpg *.bmp *.jpeg)"));
        img->save(fileName);
    }
}

void MainWindow::undo()
{
    if (statusIndex > 0) {
        statusIndex -= 1;
        *img = imgList[statusIndex];
        ui->imageLabel->setMinimumSize(img->width(), img->height());
        ui->imageLabel->setMaximumSize(img->width(), img->height());
        ui->scrollAreaWidgetContents->setMinimumSize(img->width(), img->height());
        QPixmap pixmap = QPixmap::fromImage(*img);
        ui->imageLabel->setPixmap(pixmap);
        setButton();
    }
}

void MainWindow::redo()
{
    if (statusIndex < (int)imgList.size() - 1) {
        statusIndex += 1;
        *img = imgList[statusIndex];
        ui->imageLabel->setMinimumSize(img->width(), img->height());
        ui->imageLabel->setMaximumSize(img->width(), img->height());
        ui->scrollAreaWidgetContents->setMinimumSize(img->width(), img->height());
        QPixmap pixmap = QPixmap::fromImage(*img);
        ui->imageLabel->setPixmap(pixmap);
        setButton();
    }
}

void MainWindow::showImage()
{
    std::vector<QImage>::const_iterator first = imgList.begin();
    std::vector<QImage>::const_iterator last = imgList.begin() + statusIndex + 1;
    std::vector<QImage> newVec(first, last);
    imgList = newVec;
    imgList.push_back(*img);
    statusIndex = imgList.size() - 1;
    ui->imageLabel->setMinimumSize(img->width(), img->height());
    ui->imageLabel->setMaximumSize(img->width(), img->height());
    ui->scrollAreaWidgetContents->setMinimumSize(img->width(), img->height());
    QPixmap pixmap = QPixmap::fromImage(*img);
    ui->imageLabel->setPixmap(pixmap);
    setButton();
}

void MainWindow::showTemp()
{
    ui->imageLabel->setMinimumSize(img->width(), img->height());
    ui->imageLabel->setMaximumSize(img->width(), img->height());
    ui->scrollAreaWidgetContents->setMinimumSize(img->width(), img->height());
    QPixmap pixmap = QPixmap::fromImage(*img);
    ui->imageLabel->setPixmap(pixmap);
}

void MainWindow::setButton()
{
    if (img->isGrayscale()) {
        ui->channelButton->setEnabled(false);
        ui->toGrayButton->setEnabled(false);
        ui->HSLButton->setEnabled(false);
        if (Algo::isBinary(*img)) {
            ui->otsuButton->setEnabled(false);
            ui->thresholdButton->setEnabled(false);
            ui->contrastButton->setEnabled(false);
            ui->filterButton->setEnabled(true);
            ui->edgeButton->setEnabled(true);
            ui->houghButton->setEnabled(true);
            ui->binarymorButton->setEnabled(true);
            ui->graymorButton->setEnabled(false);
        }
        else {
            ui->otsuButton->setEnabled(true);
            ui->thresholdButton->setEnabled(true);
            ui->contrastButton->setEnabled(true);
            ui->filterButton->setEnabled(true);
            ui->edgeButton->setEnabled(true);
            ui->houghButton->setEnabled(true);
            ui->binarymorButton->setEnabled(false);
            ui->graymorButton->setEnabled(true);
        }
    }
    else {
        ui->channelButton->setEnabled(true);
        ui->toGrayButton->setEnabled(true);
        ui->HSLButton->setEnabled(true);
        ui->otsuButton->setEnabled(false);
        ui->thresholdButton->setEnabled(false);
        ui->contrastButton->setEnabled(false);
        ui->filterButton->setEnabled(false);
        ui->edgeButton->setEnabled(false);
        ui->houghButton->setEnabled(false);
        ui->binarymorButton->setEnabled(false);
        ui->binarymorButton->setEnabled(false);
        ui->graymorButton->setEnabled(false);
    }
    ui->cutButton->setEnabled(true);
    ui->zoomRotateButton->setEnabled(true);
    ui->plusMinusMulButton->setEnabled(true);
}

void MainWindow::channelSeparation()
{
    int channel;
    cv::Mat matrix = Algo::QImage2cvMat(*img);
    std::vector<cv::Mat> channels;
    Algo::split(matrix, channels);
    QMessageBox *msgBox = new QMessageBox();
    msgBox->setText("请选择通道");
    msgBox->setStyleSheet("QMessageBox{background-color:rgb(65, 65, 65);}"
                          "QMessageBox QLabel{color:rgb(221, 221, 221);qproperty-alignment: AlignCenter;}"
                          "QPushButton{border:2px groove rgb(221, 221, 221);"
                          "border-radius: 6px;color:rgb(221, 221, 221);padding: 2px 5px;}"
                          "QPushButton:hover{border:2px groove rgb(221, 221, 221);"
                          "color: rgb(83, 83, 83);background-color: rgb(221, 221, 221);}"
                          "QPushButton:pressed{border:2px groove rgb(70, 160, 245);"
                          "color: rgb(221, 221, 221);background-color: rgb(70, 160, 245);}");
    QPushButton *btn_r = msgBox->addButton("R", QMessageBox::ActionRole);
    QPushButton *btn_g = msgBox->addButton("G", QMessageBox::ActionRole);
    QPushButton *btn_b = msgBox->addButton("B", QMessageBox::ActionRole);
    btn_r->setCursor(Qt::PointingHandCursor);
    btn_g->setCursor(Qt::PointingHandCursor);
    btn_b->setCursor(Qt::PointingHandCursor);
    msgBox->exec();
    if (msgBox->clickedButton() == btn_r) {
        channel = 2;
    }
    else if (msgBox->clickedButton() == btn_g) {
        channel = 1;
    }
    else if (msgBox->clickedButton() == btn_b) {
        channel = 0;
    }
    else {
        return;
    }
    *img = Algo::cvMat2QImage(channels[channel]);
    showImage();
}

void MainWindow::color2gray()
{
    cv::Mat matrix = Algo::QImage2cvMat(*img);
    matrix = Algo::toGray(matrix);
    *img = Algo::cvMat2QImage(matrix);
    showImage();
}

void MainWindow::HSLTransform()
{
    HSLDialog *dialog = new HSLDialog(this);
    dialog->exec();
}

void MainWindow::otsu()
{
    if (img->isGrayscale()) {
        cv::Mat matrix = Algo::QImage2cvMat(*img);
        matrix = Algo::otsu(matrix);
        *img = Algo::cvMat2QImage(matrix);
        showImage();
    }
}

void MainWindow::doubleThreshold()
{
    DoubleThresholdDialog *dialog = new DoubleThresholdDialog(this);
    dialog->exec();
}

void MainWindow::zoomRotate()
{
    ZoomRotateDialog *dialog = new ZoomRotateDialog(this);
    dialog->exec();
}

void MainWindow::plusMinusMul()
{
    ArithmeticDialog *dialog = new ArithmeticDialog(this);
    dialog->exec();
}

void MainWindow::contrast()
{
    ContrastDialog *dialog = new ContrastDialog(this);
    dialog->exec();
}

void MainWindow::filter()
{
    FilterDialog *dialog = new FilterDialog(this);
    dialog->exec();
}

void MainWindow::edgeDetection()
{
    EdgeDetectionDialog *dialog = new EdgeDetectionDialog(this);
    dialog->exec();
}

void MainWindow::houghTransform()
{
    HoughDialog *dialog = new HoughDialog(this);
    dialog->exec();
}

void MainWindow::cut()
{
    CutDialog *dialog = new CutDialog(this);
    dialog->exec();
}

void MainWindow::binaryMor()
{
    BinaryMorDialog *dialog = new BinaryMorDialog(this);
    dialog->exec();
}

void MainWindow::grayMor()
{
    GrayScaleMorDialog *dialog = new GrayScaleMorDialog(this);
    dialog->exec();
}
