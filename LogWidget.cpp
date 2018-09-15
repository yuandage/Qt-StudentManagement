#include "LogWidget.h"
#include "ui_LogWidget.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDebug>
LogWidget::LogWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogWidget)
{
    ui->setupUi(this);
    ui->password->setFocus();
    ui->student->setChecked(true);
}

LogWidget::~LogWidget()
{
    delete ui;

}

//登陆按钮
void LogWidget::on_login_clicked()
{
    QString username=ui->username->text();
    QString password=ui->password->text();
    bool flag=0;//连接成功flag

    if(ui->student->isChecked()){
        QSqlQuery query("select * from student_account");
        while(query.next())
        {
            if(username==query.value(0).toString()&&password==query.value(1).toString()){
                flag=1;
                this->hide();
                w.show();
            }
        }
    }

    if(ui->teacher->isChecked()){
        QSqlQuery query("select * from teacher_account");
        while(query.next())
        {
            if(username==query.value(0).toString()&&password==query.value(1).toString()){
                flag=1;
                this->hide();
                w.show();
            }
        }
    }

    if(ui->admin->isChecked()){
        QSqlQuery query("select * from admin_account");
        while(query.next())
        {
            if(username==query.value(0).toString()&&password==query.value(1).toString()){
                flag=1;
                this->hide();
                w.show();
            }
        }
    }

    if(flag==0){
        QMessageBox::information(this,"提示","错误原因:\n1.账号或密码不正确!\n2.身份选择错误!\n  请重新输入!");
    }

}

//重置按钮
void LogWidget::on_reset_clicked()
{
    if(ui->username->text()!="stu1"){
        ui->username->setText("stu1");
    }
    ui->password->clear();
    ui->student->setChecked(true);
}

void LogWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Enter){
      on_login_clicked();
    }
}
