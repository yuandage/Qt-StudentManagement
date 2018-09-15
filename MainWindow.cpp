#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QSqlError>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->widget_2->hide();
    ui->widget_3->hide();
    ui->stuinfomanmenu->setText("学生信息管理   ＞");
    ui->sysmanmenu->setText("系统管理      ＞");
    ui->stackedWidget->setCurrentWidget(ui->account_manage);

    model=new QSqlTableModel(this);
    //指定使用表
    model->setTable("student");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, tr("学号"));
    model->setHeaderData(1, Qt::Horizontal, tr("姓名"));
    model->setHeaderData(2, Qt::Horizontal, tr("性别"));
    model->setHeaderData(3, Qt::Horizontal, tr("年龄"));
    model->setHeaderData(4, Qt::Horizontal, tr("院系"));
    model->setHeaderData(5, Qt::Horizontal, tr("兴趣"));
    //手动提交
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    int row=model->rowCount();
    int col=model->columnCount();
    count=0;
    before_data_id=new QString[row];
    //开辟行空间
    alter_after_data=new QString *[row];
    for(int i=0;i<row;i++){
        alter_after_data[i]=new QString[col];//开辟列空间
    }
    //双击项的id保存
    connect(ui->tableView,&QTableView::doubleClicked,this,&MainWindow::get_changed_data);
    //数据修改后的保存
    connect(model,&QSqlTableModel::dataChanged,this,&MainWindow::get_alter_after_data);

}

MainWindow::~MainWindow()
{
    int row=model->rowCount();
    //释放alter_after_data空间
    for (int i=0; i<row; i++)
    {
        delete[] alter_after_data[i];
    }
    delete[] alter_after_data;

    delete ui;

}
//一级菜单-学生信息管理
void MainWindow::on_stuinfomanmenu_clicked()
{
    if(ui->widget_2->isHidden()){
        ui->stuinfomanmenu->setText("学生信息管理   ∨");
        ui->widget_2->show();
    }else{
        ui->stuinfomanmenu->setText("学生信息管理   ＞");
        ui->widget_2->hide();
    }
}
//一级菜单-系统管理
void MainWindow::on_sysmanmenu_clicked()
{
    if(ui->widget_3->isHidden()){
        ui->sysmanmenu->setText("系统管理      ∨");
        ui->widget_3->show();
    }else{
        ui->sysmanmenu->setText("系统管理      ＞");
        ui->widget_3->hide();
    }
}

//学生信息按钮
void MainWindow::on_stuinfo_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_stu_info);
    //把model放在modelview中
    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(5,200);
    ui->tableView->verticalHeader()->setMinimumWidth(30);
    //表头可排序
    ui->tableView->setSortingEnabled(true);
    ui->tableView->sortByColumn(0,Qt::AscendingOrder);
}

//模糊查询
void MainWindow::on_query_clicked()
{
    QString filter = ui->lineEdit_query->text();
    if(filter.isEmpty())
    {
        model->setFilter("");
        model->select();
    }

    //  name like filter or age like  filter .......
    QSqlRecord record = model->record();
    QString modelFilter;
    for(int i=0; i<record.count(); ++i)
    {
        if(i!=0)
        {
            modelFilter += " or ";
        }
        QString field = record.fieldName(i);
        QString subFilter = QString().sprintf("%s like '%%%s%%'", field.toUtf8().data(), filter.toUtf8().data());
        modelFilter += subFilter;
    }
    model->setFilter(modelFilter);
    model->select();
}

//刷新
void MainWindow::on_flash_clicked()
{
    model->select();
}

//添加数据
void MainWindow::on_add_clicked()
{
    disconnect(ui->tableView,&QTableView::doubleClicked,this,&MainWindow::get_changed_data);
    disconnect(model,&QSqlTableModel::dataChanged,this,&MainWindow::get_alter_after_data);

    QSqlTableModel *empty_model=new QSqlTableModel(this);
    if(ui->add->text()=="添加"){
        //添加空记录
        int row=model->rowCount();
        QSqlRecord record=empty_model->record();
        model->insertRecord(row,record);
        count++;
    }
}

//确认添加数据
void MainWindow::on_confirmadd_clicked()
{
    int row=model->rowCount();
    int col=model->columnCount();
    QSqlQuery query;
    QString str[count][col];
    QString content;
    bool result;

    for(int i=0;i<count;i++){
        for(int j=0;j<col;j++){
            str[i][j]=model->index(row-count+i,j).data().toString();
            content+=str[i][j]+" ";
        }
        content+="\n";
    }
    for(int i=0;i<count;i++){
        QString sql=QString("insert into student(id,name,sex,age,department,interest)"
                            " values(%1,'%2','%3',%4,'%5','%6')")
                .arg(str[i][0]).arg(str[i][1])
                .arg(str[i][2]).arg(str[i][3])
                .arg(str[i][4]).arg(str[i][5]);
        result=query.exec(sql);
    }

    count=0;
    int ret=QMessageBox::question(this,"确认添加学生信息?",content,"确认","取消");
    content="";

    if(ret==0){
        if (result==0){
            QMessageBox::critical(this,"错误","添加失败!\nError:"+query.lastError().text(),"确认","取消");
            on_flash_clicked();
            connect(ui->tableView,&QTableView::doubleClicked,this,&MainWindow::get_changed_data);
            connect(model,&QSqlTableModel::dataChanged,this,&MainWindow::get_alter_after_data);

        }else{
            QMessageBox::information(this,"提示","添加成功!","确认","取消");
            on_flash_clicked();
            connect(ui->tableView,&QTableView::doubleClicked,this,&MainWindow::get_changed_data);
            connect(model,&QSqlTableModel::dataChanged,this,&MainWindow::get_alter_after_data);
        }
    }
}

//删除数据
void MainWindow::on_deletedata_clicked()
{
    QItemSelectionModel * selectModel = ui->tableView->selectionModel();
    // 通过选中的数据结构，获取这些格子的ModelIndex
    QModelIndexList selectList =  selectModel->selectedIndexes();
    QList<int> delRow;
    //遍历这些格子，获取格子所在行，因为可能存在相同的行，所以要去重
    for(int i=0; i<selectList.size(); i++)
    {
        QModelIndex index = selectList.at(i);
        delRow << index.row();
    }
    //获取的行号(去重)
    for (int i = 0; i < delRow.count(); i++)  //外循环是循环的次数
    {
        for (int j = delRow.count()-1 ; j >i; j--)  //内循环是 外循环一次比较的次数
        {
            if (delRow.at(i) == delRow.at(j))
            {
                delRow.removeAt(j);
            }
        }
    }
    QSqlQuery query;
    QList<int>::iterator i;
    bool result;
    for(i=delRow.begin(); i!=delRow.end(); ++i)
    {
        QModelIndex index = model->index(*i,0,QModelIndex());
        QString str= index.data().toString();
        QString sql=QString("delete from student where id=%1").arg(str);
        result=query.exec(sql);
    }
    if (result==0){
        QMessageBox::critical(this,"错误","删除失败!\nError:"+query.lastError().text(),"确认","取消");
    }else{
        QMessageBox::information(this,"提示","删除成功!","确认","取消");
        on_flash_clicked();
    }

    //通过 model->removeRow(row)删除数据----失败:Using unsupported buffer type: 8060965  (parameter: 1) QMYSQL3: Unable to bind value
    //    // 通过_view去获取被选中的部分的数据model
    //    QItemSelectionModel * selectModel = ui->tableView->selectionModel();
    //    // 通过选中的数据结构，获取这些格子的ModelIndex
    //    QModelIndexList selectList =  selectModel->selectedIndexes();
    //    QList<int> delRow;
    //    // 遍历这些格子，获取格子所在行，因为可能存在相同的行，所以要去重
    //    for(int i=0; i<selectList.size(); ++i)
    //    {
    //        QModelIndex index = selectList.at(i);
    //        //  model->removeRow(index.row());
    //        delRow << index.row();
    //    }
    //    while(delRow.size() > 0)
    //    {
    //        int row = delRow.at(0);
    //        delRow.removeAll(row);
    //        model->removeRow(row);
    //    }
    //     model->submitAll();

}

//修改数据
void MainWindow::on_alter_clicked()
{
    QSqlQuery query;
    bool result;
    int row=model->rowCount();
    int col=model->columnCount();
    //获取表的列名
    QString sql=QString("select COLUMN_NAME from information_schema.columns where table_name='student'");
    query.exec(sql);
    QString col_name[col];
    int i=0;

    while(query.next()){
        col_name[i]=query.value(0).toString();
        i++;
    }

    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            if(alter_after_data[i][j]!=NULL){
                QString sql=QString("update student set %1='%2' where id=%3").arg(col_name[j])
                        .arg(alter_after_data[i][j])
                        .arg(before_data_id[i]);
                result=query.exec(sql);
                QModelIndex index1 = model->index(i,0);
                before_data_id[i] = model->data(index1).toString();
            }
        }
    }

    if (result==0){
        QMessageBox::critical(this,"错误","修改失败!\nError:"+query.lastError().text(),"确认","取消");
        on_flash_clicked();
    }else{
        QMessageBox::information(this,"提示","修改成功!","确认","取消");
        on_flash_clicked();
    }
}
//双击项的id保存
void MainWindow::get_changed_data()
{
    select_row = ui-> tableView ->currentIndex().row();
    select_col=ui-> tableView ->currentIndex().column();
    QModelIndex index = model->index(select_row,0);
    before_data_id[select_row] = model->data(index).toString();
}
//数据修改后的保存
void MainWindow::get_alter_after_data()
{
    //获取选中项的信息
    select_row = ui-> tableView ->currentIndex().row();
    select_col=ui-> tableView ->currentIndex().column();
    QModelIndex index = model->index(select_row,select_col);
    alter_after_data[select_row][select_col] = model->data(index).toString();
}

//帮助按钮
void MainWindow::on_help_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_help);
}

//班级管理按钮
void MainWindow::on_class_manage_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_class);
    QSqlTableModel *class_model=new QSqlTableModel(this);
    //指定使用表
    class_model->setTable("student_class");
    class_model->select();
    class_model->setHeaderData(0, Qt::Horizontal, tr("学号"));
    class_model->setHeaderData(1, Qt::Horizontal, tr("姓名"));
    class_model->setHeaderData(2, Qt::Horizontal, tr("班级名称"));
    class_model->setHeaderData(3, Qt::Horizontal, tr("班级编号"));
    //手动提交
    class_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //把model放在modelview中
    ui->tableView_class->setModel(class_model);
    ui->tableView_class->verticalHeader()->setMinimumWidth(30);
    //表头可排序
    ui->tableView_class->setSortingEnabled(true);
    ui->tableView_class->sortByColumn(0,Qt::AscendingOrder);
}

//成绩管理按钮
void MainWindow::on_score_manage_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_score);
    QSqlTableModel *score_model=new QSqlTableModel(this);
    //指定使用表
    score_model->setTable("student_score");
    score_model->select();
    score_model->setHeaderData(0, Qt::Horizontal, tr("学号"));
    score_model->setHeaderData(1, Qt::Horizontal, tr("姓名"));
    score_model->setHeaderData(2, Qt::Horizontal, tr("语文成绩"));
    score_model->setHeaderData(3, Qt::Horizontal, tr("数学成绩"));
    score_model->setHeaderData(4, Qt::Horizontal, tr("英语成绩"));
    //手动提交
    score_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //把model放在modelview中
    ui->tableView_score->setModel(score_model);
    ui->tableView_score->verticalHeader()->setMinimumWidth(30);
    //表头可排序
    ui->tableView_score->setSortingEnabled(true);
    ui->tableView_score->sortByColumn(0,Qt::AscendingOrder);
}
