#ifndef GRAYSCALEMORDIALOG_H
#define GRAYSCALEMORDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <cvAlgorithms.h>

namespace Ui {
class GrayScaleMorDialog;
}

class GrayScaleMorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GrayScaleMorDialog(QWidget *parent = 0);
    ~GrayScaleMorDialog();
    cv::Mat morphologicalReconstruct(cv::Mat mat, std::vector<std::vector<int>>& kernel);
    void conform();
    void cancel();

public slots:
    void changeSize();
    void changeType();

protected:
    void closeEvent(QCloseEvent *e);

private:
    Ui::GrayScaleMorDialog *ui;
    QImage tmp;
    bool isConform = false;
    QButtonGroup *sizeGroup;
    QButtonGroup *typeGroup;
};

#endif // GRAYSCALEMORDIALOG_H
