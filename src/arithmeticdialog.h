#ifndef ARITHMETICDIALOG_H
#define ARITHMETICDIALOG_H

#include <QDialog>

namespace Ui {
class ArithmeticDialog;
}

class ArithmeticDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArithmeticDialog(QWidget *parent = 0);
    ~ArithmeticDialog();
    void plus();
    void minus();
    void multiply();

private:
    Ui::ArithmeticDialog *ui;
};

#endif // ARITHMETICDIALOG_H
