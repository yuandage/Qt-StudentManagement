#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>
#include "MainWindow.h"
#include <QKeyEvent>

namespace Ui {
class LogWidget;
}

class LogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget *parent = 0);
    ~LogWidget();

private slots:
    void on_login_clicked();

    void on_reset_clicked();
protected:
    virtual void keyPressEvent(QKeyEvent *event);
private:
    Ui::LogWidget *ui;
    MainWindow w;

};

#endif // LOGWIDGET_H
