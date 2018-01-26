#ifndef EDGEDETECTIONDIALOG_H
#define EDGEDETECTIONDIALOG_H

#include <QDialog>
#include <QButtonGroup>

namespace Ui {
class EdgeDetectionDialog;
}

class EdgeDetectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EdgeDetectionDialog(QWidget *parent = 0);
    ~EdgeDetectionDialog();
    void conform();
    void cancel();

public slots:
    void applyEdgeDetection();

protected:
    void closeEvent(QCloseEvent *e);

private:
    Ui::EdgeDetectionDialog *ui;
    QButtonGroup *btnGroup;
    QImage tmp;
    bool isConform = false;
};

#endif // EDGEDETECTIONDIALOG_H
