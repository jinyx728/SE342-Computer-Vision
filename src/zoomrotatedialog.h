#ifndef ZOOMROTATEDIALOG_H
#define ZOOMROTATEDIALOG_H

#include <QDialog>

namespace Ui {
class ZoomRotateDialog;
}

class ZoomRotateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ZoomRotateDialog(QWidget *parent = 0);
    ~ZoomRotateDialog();
    void conform();
    void cancel();

public slots:
    void adjust();
    void checkNearest();
    void checkLinear();

protected:
    void closeEvent(QCloseEvent *e);

private:
    Ui::ZoomRotateDialog *ui;
    QImage tmp;
    bool isConform = false;
    int zoomRate = 100;
    int rotateRate = 0;
};

#endif // ZOOMROTATEDIALOG_H
