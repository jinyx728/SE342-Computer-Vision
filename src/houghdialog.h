#ifndef HOUGHDIALOG_H
#define HOUGHDIALOG_H

#include <QDialog>
#include <QButtonGroup>

namespace Ui {
class HoughDialog;
}

class HoughDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HoughDialog(QWidget *parent = 0);
    ~HoughDialog();
    void conform();
    void cancel();

public slots:
    void detectLine();
    void detectCircle();

protected:
    void closeEvent(QCloseEvent *e);

private:
    Ui::HoughDialog *ui;
    QButtonGroup *btnGroup;
    QImage tmp;
    bool isConform = false;
};

#endif // HOUGHDIALOG_H
