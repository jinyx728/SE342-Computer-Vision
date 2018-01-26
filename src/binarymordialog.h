#ifndef BINARYMORDIALOG_H
#define BINARYMORDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <opencv2/core.hpp>

namespace Ui {
class BinaryMorDialog;
}

class BinaryMorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BinaryMorDialog(QWidget *parent = 0);
    ~BinaryMorDialog();
    cv::Mat distanceTrans(cv::Mat mat);
    cv::Mat morphologicalReconstruct(cv::Mat mat, std::vector<std::vector<int>>& kernel);
    void conform();
    void cancel();

public slots:
    void changeSize();
    void changeType();

protected:
    void closeEvent(QCloseEvent *e);

private:
    Ui::BinaryMorDialog *ui;
    QImage tmp;
    bool isConform = false;
    QButtonGroup *sizeGroup;
    QButtonGroup *typeGroup;
};

#endif // BINARYMORDIALOG_H
