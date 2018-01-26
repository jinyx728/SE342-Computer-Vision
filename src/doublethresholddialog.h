#ifndef DOUBLETHRESHOLDDIALOG_H
#define DOUBLETHRESHOLDDIALOG_H

#include <QDialog>

namespace Ui {
class DoubleThresholdDialog;
}

class DoubleThresholdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DoubleThresholdDialog(QWidget *parent = 0);
    ~DoubleThresholdDialog();
    void conform();
    void cancel();

public slots:
    void adjustThreshold();

protected:
    void closeEvent(QCloseEvent *e);

private:
    Ui::DoubleThresholdDialog *ui;
    QImage tmp;
    bool isConform = false;
};

#endif // DOUBLETHRESHOLDDIALOG_H
