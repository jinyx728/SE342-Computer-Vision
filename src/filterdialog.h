#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QButtonGroup>

namespace Ui {
class FilterDialog;
}

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog(QWidget *parent = 0);
    ~FilterDialog();
    void conform();
    void cancel();

public slots:
    void applyFilter();

protected:
    void closeEvent(QCloseEvent *e);

private:
    Ui::FilterDialog *ui;
    QButtonGroup *typeGroup;
    QButtonGroup *sizeGroup;
    QImage tmp;
    bool isConform = false;
};

#endif // FILTERDIALOG_H
