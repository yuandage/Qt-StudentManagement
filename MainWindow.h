#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_stuinfomanmenu_clicked();
    void on_sysmanmenu_clicked();
    void on_stuinfo_clicked();
    void on_query_clicked();
    void on_flash_clicked();
    void on_add_clicked();
    void on_deletedata_clicked();
    void on_alter_clicked();
    //双击修改数据
    void get_changed_data();
    //保存双击修改的数据和(行号,列号)
    void get_alter_after_data();

    void on_confirmadd_clicked();

    void on_help_clicked();

    void on_class_manage_clicked();

    void on_score_manage_clicked();

private:
    Ui::MainWindow *ui;
    QSqlTableModel *model;
    //添加的条数
    int count;
    //选中项的(行号,列号)
    int select_row;
    int select_col;
    //原始的id
    QString *before_data_id;
    //修改的数据(二维数组)
    QString **alter_after_data;

};

#endif // MAINWINDOW_H
