#include "MainWindow.h"
#include <QApplication>
#include "LogWidget.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSqlDatabase db;
    db= QSqlDatabase::addDatabase("QMYSQL");
    //连接数据库
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("165035");
    db.setDatabaseName("studentmanager");
    //打开数据库
    if(!db.open()){
        QMessageBox::warning(0,"无法连接到数据库!",db.lastError().text());
        LogWidget l;
        l.show();
        QMessageBox::warning(0,"无法登陆!","请联系管理员!\n程序即将退出!");
        return 0;
    }else{
        LogWidget l;
        l.show();
        return a.exec();
    }
}
