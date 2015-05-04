#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "QFile"
#include "QMap"
#include "QCheckBox"
#include "QProcess"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupTables();
    dataPath = "../data";
    scriptPath = "../script";

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clear(){
    qDebug()<<"click clear";
    for(int i = ui->tableWidget->rowCount() ; i>=0 ; i--){
        ui->tableWidget->removeRow(i);

    }
    for(int i = ui->tableWidget_2->rowCount() ; i>=0 ; i--){
        ui->tableWidget_2->removeRow(i);

    }
}

void MainWindow::generateFiles(){
    qDebug()<<"click generate";
    QHash<QString,float> powerMap;
    QHash<QString,bool> failedMap;

    QString flpFileName = "system.flp";
    QFile flpFile(flpFileName);
    if(flpFile.open(QIODevice::ReadWrite)){
        QTextStream stream(&flpFile);

        for(int i = 0; i < ui->tableWidget->rowCount(); i++){
            QString name = ui->tableWidget->item(i,0)->text();
            QString width = ui->tableWidget->item(i,1)->text().replace(",",".");
            QString height = ui->tableWidget->item(i,2)->text().replace(",",".");
            QString leftX = ui->tableWidget->item(i,3)->text().replace(",",".");
            QString bottomY = ui->tableWidget->item(i,4)->text().replace(",",".");

            QString utlization = ui->tableWidget_2->item(i,0)->text();
            QString pIdle = ui->tableWidget_2->item(i,1)->text();
            QString pMax = ui->tableWidget_2->item(i,2)->text();
            QCheckBox *cb = qobject_cast<QCheckBox *>(ui->tableWidget_2->cellWidget(i, 3));
            bool isFailed = false;
            if(cb->isChecked()){
                isFailed = true;
            }

            qDebug() << name << " " << width << " "<< height << " "<< leftX << " "<< bottomY << " "<< utlization;
            float power = pIdle.toFloat() + ( pMax.toFloat() - pIdle.toFloat())*utlization.toFloat();
            powerMap.insert(name,power);
            stream << name.toUpper() << " " << width << " "<< height << " "<< leftX << " "<< bottomY << "\n";
            qDebug() << isFailed;
            failedMap.insert(name,isFailed);

        }
    }
    qDebug() << powerMap.size();
    flpFile.close();
    QString powFileName = "system.pow";
    QFile powFile(powFileName);
    QString idWorkingState = ui->idLine->text();
    QString nameFile = QString(""+idWorkingState+"_");
    qDebug() << nameFile;
    if(powFile.open(QIODevice::ReadWrite)){
        QTextStream stream(&powFile);
        for(int i = 0; i < powerMap.keys().length(); i++){
            stream << powerMap.keys()[i].toUpper();
            if(i != (powerMap.keys().length()-1)){
                stream << " ";
            }
            if(!failedMap.value(powerMap.keys()[i])){
                nameFile.append("1");
                qDebug() << nameFile;

            }else{
                nameFile.append("0");
                qDebug() << nameFile;

            }
            if(i != (powerMap.keys().length()-1 )){
                nameFile.append(":");
                qDebug() << nameFile;

            }
        }
         stream << "\n";
        for(int i = 0; i < powerMap.keys().length(); i++){
            qDebug()<< "in loop";
            stream << powerMap.value(powerMap.keys()[i]);
            if(i != (powerMap.keys().length()-1)){
                stream << " ";
            }
        }
        stream << "\n";
    }

    nameFile.append(".txt");
    qDebug() << nameFile;

    QString command  = scriptPath+"/hotspot -f system.flp -p system.pow -steady_file "+dataPath+"/"+nameFile+"";
    QProcess::startDetached( "/bin/bash", QStringList() << "-c"<<command);

}

void MainWindow::setupTables(){
   ui->tableWidget->insertColumn(0);
   ui->tableWidget->insertColumn(1);
   ui->tableWidget->insertColumn(2);
   ui->tableWidget->insertColumn(3);
   ui->tableWidget->insertColumn(4);

   ui->tableWidget_2->insertColumn(0);
   ui->tableWidget_2->insertColumn(1);
   ui->tableWidget_2->insertColumn(2);
   ui->tableWidget_2->insertColumn(3);



   QTableWidgetItem *item = new QTableWidgetItem("Name");
   QTableWidgetItem *item2 = new QTableWidgetItem("Width[m]");
   QTableWidgetItem *item3 = new QTableWidgetItem("Height[m]");
   QTableWidgetItem *item4 = new QTableWidgetItem("Left-x[m]");
   QTableWidgetItem *item5 = new QTableWidgetItem("Bottom-y[m]");
   QTableWidgetItem *item9 = new QTableWidgetItem("Utilization");
   QTableWidgetItem *item7 = new QTableWidgetItem("P_Idle");
   QTableWidgetItem *item8 = new QTableWidgetItem("P_Max");
   QTableWidgetItem *item10 = new QTableWidgetItem("Failed");


   ui->tableWidget->setHorizontalHeaderItem(0,item);
   ui->tableWidget->setHorizontalHeaderItem(1,item2);
   ui->tableWidget->setHorizontalHeaderItem(2,item3);
   ui->tableWidget->setHorizontalHeaderItem(3,item4);
   ui->tableWidget->setHorizontalHeaderItem(4,item5);
   ui->tableWidget_2->setHorizontalHeaderItem(0,item9);
   ui->tableWidget_2->setHorizontalHeaderItem(1,item7);
   ui->tableWidget_2->setHorizontalHeaderItem(2,item8);
   ui->tableWidget_2->setHorizontalHeaderItem(3,item10);



}

void MainWindow::addNewLine(){
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget_2->insertRow(ui->tableWidget_2->rowCount());

    QWidget *pWidget = new QWidget();
    QCheckBox *pCheckBox = new QCheckBox();
    QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
    pLayout->addWidget(pCheckBox);
    pLayout->setAlignment(Qt::AlignCenter);
    pLayout->setContentsMargins(0,0,0,0);
    pWidget->setLayout(pLayout);
    ui->tableWidget_2->setCellWidget(ui->tableWidget_2->rowCount()-1,3,pCheckBox);

}
