#ifndef HSLDIALOG_H
#define HSLDIALOG_H

#include <QDialog>
#include <QMainWindow>
#include "HSL.hpp"

namespace Ui {
class HSLDialog;
}

class HSLDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HSLDialog(QWidget *parent = 0);
    ~HSLDialog();
    void conform();
    void cancel();

public slots:
    void adjustColor(int index);
    void adjustHSL();

protected:
    void closeEvent(QCloseEvent *e);

private:
    Ui::HSLDialog *ui;
    HSL hsl;
    int color = 0;
    int hue = 180;
    int saturation = 100;
    int brightness = 100;
    QImage tmp;
    bool isConform = false;
};

#endif // HSLDIALOG_H
