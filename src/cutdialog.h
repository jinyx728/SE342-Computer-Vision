#ifndef CUTDIALOG_H
#define CUTDIALOG_H

#include <QDialog>

namespace Ui {
class CutDialog;
}

class CutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CutDialog(QWidget *parent = 0);
    ~CutDialog();
    void conform();
    void cancel();

public slots:
    void cutImage();

protected:
    void closeEvent(QCloseEvent *e);

private:
    Ui::CutDialog *ui;
    QImage tmp;
    bool isConform = false;
};

#endif // CUTDIALOG_H
