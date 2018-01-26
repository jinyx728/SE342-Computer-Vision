#ifndef CONTRASTDIALOG_H
#define CONTRASTDIALOG_H

#include <QDialog>
#include <vector>

namespace Ui {
class ContrastDialog;
}

class ContrastDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContrastDialog(QWidget *parent = 0);
    ~ContrastDialog();
    void drawHistogram(QImage img);
    void conform();
    void cancel();
    void equalization();

public slots:
    void setLinear();
    void adjustLinear();
    void setExp();
    void adjustExp();
    void setLog();
    void adjustLog();

protected:
    void closeEvent(QCloseEvent *e);

private:
    Ui::ContrastDialog *ui;
    QImage tmp;
    bool isConform = false;
};

#endif // CONTRASTDIALOG_H
