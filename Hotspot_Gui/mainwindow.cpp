#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "QFile"
#include "QMap"
#include "QCheckBox"
#include "QProcess"
#include "QPair"
#include "QList"

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
    ui->idLine->setText("");
}

void MainWindow::generatePartScheduler(){


    for(int i=0;i<ui->tableWidget_3->rowCount();i++){
        double duration = ui->tableWidget_3->item(i,0)->text().toDouble();
        qDebug() << "in generate part";
        QCheckBox *cpu0 = qobject_cast<QCheckBox *>(ui->tableWidget_3->cellWidget(i, 1));
        QCheckBox *cpu1 = qobject_cast<QCheckBox *>(ui->tableWidget_3->cellWidget(i, 2));
        QCheckBox *cpu2 = qobject_cast<QCheckBox *>(ui->tableWidget_3->cellWidget(i, 3));
        QCheckBox *cpu3 = qobject_cast<QCheckBox *>(ui->tableWidget_3->cellWidget(i, 4));
        QCheckBox *cpu4 = qobject_cast<QCheckBox *>(ui->tableWidget_3->cellWidget(i, 5));

        updateMap(cpu0->isChecked(),cpu1->isChecked(),cpu2->isChecked(),cpu3->isChecked(),cpu4->isChecked(),duration,i);

    }
}

void MainWindow::generateFiles(){
    if(ui->idLine->text() != ""){
        qDebug()<<"click generate";
        QHash<QString,float> powerMap;
        QHash<QString,bool> failedMap;
        QList<QString> power2;


        generatePartScheduler();

        QString flpFileName = "system.flp";
        QFile flpFile(flpFileName);
        qDebug() << "open";
        if(flpFile.open(QIODevice::ReadWrite)){
            QTextStream stream(&flpFile);

            for(int i = 0; i < ui->tableWidget->rowCount(); i++){
                QString name = ui->tableWidget->item(i,0)->text();
                QString width = ui->tableWidget->item(i,1)->text().replace(",",".");
                QString height = ui->tableWidget->item(i,2)->text().replace(",",".");
                QString leftX = ui->tableWidget->item(i,3)->text().replace(",",".");
                QString bottomY = ui->tableWidget->item(i,4)->text().replace(",",".");

                //QString utlization = ui->tableWidget_2->item(i,0)->text();
                QString pIdle = ui->tableWidget_2->item(i,1)->text();
                QString pMax = ui->tableWidget_2->item(i,2)->text();
                QCheckBox *cb = qobject_cast<QCheckBox *>(ui->tableWidget_2->cellWidget(i, 3));
                bool isFailed = false;
                if(cb->isChecked()){
                    isFailed = true;
                }

                qDebug() << name << " " << width << " "<< height << " "<< leftX << " "<< bottomY << " ";
                //float power = pIdle.toFloat() + ( pMax.toFloat() - pIdle.toFloat())*utlization.toFloat();
                powerMap.insert(name,0);
                power2.append(name);
                stream << name.toUpper() << " " << width << " "<< height << " "<< leftX << " "<< bottomY << "\n";
                qDebug() << isFailed;
                failedMap.insert(name,isFailed);

            }
        }
        qDebug() << powerMap.size();
        flpFile.close();
        QString idWorkingState = ui->idLine->text();

        //QString powFileName = idWorkingState+"_system.pow";

        QString nameFile = QString(dataPath+"/"+idWorkingState+"_");

        qDebug() << nameFile;
        for(int i = 0; i < power2.length(); i++){
            /*stream << powerMap.keys()[i].toUpper();
            if(i != (powerMap.keys().length()-1)){
                stream << " ";
            }*/
            if(!failedMap.value(power2.at(i))){
                nameFile.append("1");
                qDebug() << nameFile;

            }else{
                nameFile.append("0");
                qDebug() << nameFile;

            }
            if(i != (power2.length()-1 )){
                nameFile.append(":");
                qDebug() << nameFile;

            }
        }

        nameFile.append(".txt");

        QFile powFile(nameFile);
        if(powFile.open(QIODevice::ReadWrite)){
            QTextStream stream(&powFile);
            for(int i = 0; i < power2.length(); i++){
                qDebug()<< "in loop";
                double alpha = ui->totalTime->text().toDouble()/alphas.value(power2.at(i).toUpper());
                stream << alpha;
                if(i != (power2.length()-1)){
                    stream << " ";
                }
            }
        }


       // QString command  = scriptPath+"/hotspot -f system.flp -p "+powFileName+" -steady_file "+dataPath+"/"+nameFile+"";
        //QProcess::startDetached( "/bin/bash", QStringList() << "-c"<<command);

        alphas.clear();
    }


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
   QTableWidgetItem *item11 = new QTableWidgetItem("Active");


   ui->tableWidget->setHorizontalHeaderItem(0,item);
   ui->tableWidget->setHorizontalHeaderItem(1,item2);
   ui->tableWidget->setHorizontalHeaderItem(2,item3);
   ui->tableWidget->setHorizontalHeaderItem(3,item4);
   ui->tableWidget->setHorizontalHeaderItem(4,item5);
   ui->tableWidget_2->setHorizontalHeaderItem(0,item9);
   ui->tableWidget_2->setHorizontalHeaderItem(1,item7);
   ui->tableWidget_2->setHorizontalHeaderItem(2,item8);
   ui->tableWidget_2->setHorizontalHeaderItem(3,item10);

   ui->tableWidget_3->insertColumn(0);
   ui->tableWidget_3->insertColumn(1);
   ui->tableWidget_3->insertColumn(2);
   ui->tableWidget_3->insertColumn(3);
   ui->tableWidget_3->insertColumn(4);
   ui->tableWidget_3->insertColumn(5);




   ui->tableWidget_3->setHorizontalHeaderItem(0,new QTableWidgetItem("Period"));
   ui->tableWidget_3->setHorizontalHeaderItem(1,new QTableWidgetItem("LI1"));
   ui->tableWidget_3->setHorizontalHeaderItem(2,new QTableWidgetItem("LI2"));
   ui->tableWidget_3->setHorizontalHeaderItem(3,new QTableWidgetItem("LI3"));
   ui->tableWidget_3->setHorizontalHeaderItem(4,new QTableWidgetItem("HW1"));
   ui->tableWidget_3->setHorizontalHeaderItem(5,new QTableWidgetItem("HW2"));


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

void MainWindow::updateMap(bool cpu0Active, bool cpu1Active,bool cpu2Active,bool cpu3Active,bool cpu4Active,double duration,int index){

    QString flpFileName = "system.flp";
    QFile flpFile(flpFileName);
    if(flpFile.open(QIODevice::ReadWrite)){
        QTextStream stream(&flpFile);

        for(int i = 0; i < ui->tableWidget->rowCount(); i++){
            QString name = ui->tableWidget->item(i,0)->text().toUpper();
            QString width = ui->tableWidget->item(i,1)->text().replace(",",".");
            QString height = ui->tableWidget->item(i,2)->text().replace(",",".");
            QString leftX = ui->tableWidget->item(i,3)->text().replace(",",".");
            QString bottomY = ui->tableWidget->item(i,4)->text().replace(",",".");
            QCheckBox *cb = qobject_cast<QCheckBox *>(ui->tableWidget_2->cellWidget(i, 3));
            bool isFailed = false;
            if(cb->isChecked()){
                isFailed = true;
            }
            stream << name.toUpper() << " " << width << " "<< height << " "<< leftX << " "<< bottomY << "\n";

        }
    }
    flpFile.close();

    QString temporaryFileName = "system_"+ui->idLine->text()+"_"+QString::number(index)+".pow";
    QFile powFile(temporaryFileName);
    if(powFile.open(QIODevice::ReadWrite)){
        QTextStream stream(&powFile);

        for(int i = 0; i < ui->tableWidget->rowCount(); i++){
            QString name = ui->tableWidget->item(i,0)->text().toUpper();
            if(!alphas.keys().contains(name)){
                alphas.insert(name,0.0);
            }
            stream << name.toUpper();
            if(i != (ui->tableWidget->rowCount()-1)){
                stream << " ";
            }


        }
        QString dataToWrite = "";
        if(cpu0Active){
            dataToWrite.append(ui->tableWidget_2->item(0,2)->text());
        }else{
            dataToWrite.append(ui->tableWidget_2->item(0,1)->text());
        }
        dataToWrite.append(" ");
        if(cpu1Active){
            dataToWrite.append(ui->tableWidget_2->item(1,2)->text());
        }else{
            dataToWrite.append(ui->tableWidget_2->item(1,1)->text());
        }
        dataToWrite.append(" ");
        if(cpu2Active){
            dataToWrite.append(ui->tableWidget_2->item(2,2)->text());
        }else{
            dataToWrite.append(ui->tableWidget_2->item(2,1)->text());
        }
        dataToWrite.append(" ");
        if(cpu3Active){
            dataToWrite.append(ui->tableWidget_2->item(3,2)->text());
        }else{
            dataToWrite.append(ui->tableWidget_2->item(3,1)->text());
        }
        dataToWrite.append(" ");
        if(cpu4Active){
            dataToWrite.append(ui->tableWidget_2->item(4,2)->text());
        }else{
            dataToWrite.append(ui->tableWidget_2->item(4,1)->text());
        }
        stream<<"\n";
        stream<<dataToWrite;
        stream<<"\n";
        powFile.close();
    }


    QString command  = scriptPath+"/hotspot -f system.flp -p "+temporaryFileName+" -steady_file tmp.txt";

    QProcess pingProcess;
    pingProcess.start(command);
    pingProcess.waitForFinished();

    QFile inputFileNew("tmp.txt");if (inputFileNew.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFileNew);
       int count = 0;
       while (count < ui->tableWidget->rowCount())
       {
          QString line = in.readLine();
          qDebug() << "line";
          qDebug() << line;
          QString name = line.split("\t")[0];
          QString temperature = line.split("\t")[1];
          qDebug() << temperature;
          count++;
          QString command  = scriptPath+"/computeAlpha "+temperature;
          QProcess pingProcess;
          pingProcess.start(command);
          pingProcess.waitForFinished();
          QString output(pingProcess.readAllStandardOutput());
          qDebug()<<output.split(":");
          double alpha = output.split(":")[1].toDouble();
          double weight = duration / alpha;
          qDebug()<<name;
          qDebug()<<weight;
          double previous = alphas.value(name);
          alphas.remove(name);
          alphas.insert(name,previous+weight);
          double eee = alphas.value(name);

          qDebug()<<eee;
       }
       inputFileNew.close();
    }



}

void MainWindow::intermediate(){
    /*qDebug() << "click intermediate";
    QString dataToWrite = "";
    QString flpFileName = "system.flp";
    QFile flpFile(flpFileName);
    if(flpFile.open(QIODevice::ReadWrite)){
        QTextStream stream(&flpFile);

        for(int i = 0; i < ui->tableWidget->rowCount(); i++){
            QString name = ui->tableWidget->item(i,0)->text().toUpper();
            QString width = ui->tableWidget->item(i,1)->text().replace(",",".");
            QString height = ui->tableWidget->item(i,2)->text().replace(",",".");
            QString leftX = ui->tableWidget->item(i,3)->text().replace(",",".");
            QString bottomY = ui->tableWidget->item(i,4)->text().replace(",",".");
            QCheckBox *cb = qobject_cast<QCheckBox *>(ui->tableWidget_2->cellWidget(i, 3));
            bool isFailed = false;
            if(cb->isChecked()){
                isFailed = true;
            }
            stream << name.toUpper() << " " << width << " "<< height << " "<< leftX << " "<< bottomY << "\n";

        }
    }
    flpFile.close();
    QString temporaryFileName = "system.pow";
    QFile powFile(temporaryFileName);
    if(powFile.open(QIODevice::ReadWrite)){
        QTextStream stream(&powFile);

        for(int i = 0; i < ui->tableWidget->rowCount(); i++){
            QString name = ui->tableWidget->item(i,0)->text().toUpper();
            if(!alphas.keys().contains(name)){
                alphas.insert(name,0.0);
            }

            stream << name.toUpper();
            if(i != (ui->tableWidget->rowCount()-1)){
                stream << " ";
            }
            QCheckBox *cb = qobject_cast<QCheckBox *>(ui->tableWidget_3->cellWidget(i, 1));
            if(!cb->isChecked()){
               dataToWrite.append(ui->tableWidget_2->item(i,1)->text());
            }else{
                dataToWrite.append(ui->tableWidget_2->item(i,2)->text());
            }
            if(i != (ui->tableWidget->rowCount()-1)){
                dataToWrite.append(" ");
            }

        }
        stream<<"\n";
        stream<<dataToWrite;
        stream<<"\n";
        powFile.close();
    }

    QString command  = scriptPath+"/hotspot -f system.flp -p "+temporaryFileName+" -steady_file tmp.txt";
    //QProcess::startDetached( "/bin/bash", QStringList() << "-c"<<command);

    QProcess pingProcess;
    pingProcess.start(command);
    pingProcess.waitForFinished();

    QFile inputFileNew("tmp.txt");
    if (inputFileNew.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFileNew);
       int count = 0;
       while (count < ui->tableWidget_3->rowCount())
       {
          QString line = in.readLine();
          qDebug() << "line";
          qDebug() << line;
          QString name = line.split("\t")[0];
          QString temperature = line.split("\t")[1];
          qDebug() << temperature;
          count++;
          QString command  = scriptPath+"/computeAlpha "+temperature;
          QProcess pingProcess;
          pingProcess.start(command);
          pingProcess.waitForFinished();
          QString output(pingProcess.readAllStandardOutput());
          qDebug()<<output.split(":");
          double alpha = output.split(":")[1].toFloat();
          double interval = ui->currentTime->text().toFloat();
          double weight = interval / alpha;
          qDebug()<<name;
          qDebug()<<weight;
          alphas.insert(name,alphas.value(name)+weight);
          double eee = alphas.value(name);

          qDebug()<<eee;
       }
       inputFileNew.close();
    }

    QFile inputFile("tmp.txt");
    //inputFile.remove();
    QFile powFileToRemove(temporaryFileName);
    //powFileToRemove.remove();
*/
}

void MainWindow::addLineSched(){

    ui->tableWidget_3->insertRow(ui->tableWidget_3->rowCount());


    QWidget *pWidget0 = new QWidget();
    QCheckBox *pCheckBox0 = new QCheckBox();
    QHBoxLayout *pLayout0 = new QHBoxLayout(pWidget0);
    pLayout0->addWidget(pCheckBox0);
    pLayout0->setAlignment(Qt::AlignCenter);
    pLayout0->setContentsMargins(0,0,0,0);
    pWidget0->setLayout(pLayout0);
    ui->tableWidget_3->setCellWidget(ui->tableWidget_3->rowCount()-1,1,pCheckBox0);

    QWidget *pWidget1 = new QWidget();
    QCheckBox *pCheckBox1 = new QCheckBox();
    QHBoxLayout *pLayout1 = new QHBoxLayout(pWidget1);
    pLayout1->addWidget(pCheckBox1);
    pLayout1->setAlignment(Qt::AlignCenter);
    pLayout1->setContentsMargins(0,0,0,0);
    pWidget1->setLayout(pLayout1);
    ui->tableWidget_3->setCellWidget(ui->tableWidget_3->rowCount()-1,2,pCheckBox1);

    QWidget *pWidget2 = new QWidget();
    QCheckBox *pCheckBox2 = new QCheckBox();
    QHBoxLayout *pLayout2 = new QHBoxLayout(pWidget2);
    pLayout2->addWidget(pCheckBox2);
    pLayout2->setAlignment(Qt::AlignCenter);
    pLayout2->setContentsMargins(0,0,0,0);
    pWidget2->setLayout(pLayout2);
    ui->tableWidget_3->setCellWidget(ui->tableWidget_3->rowCount()-1,3,pCheckBox2);

    QWidget *pWidget3 = new QWidget();
    QCheckBox *pCheckBox3 = new QCheckBox();
    QHBoxLayout *pLayout3 = new QHBoxLayout(pWidget3);
    pLayout3->addWidget(pCheckBox3);
    pLayout3->setAlignment(Qt::AlignCenter);
    pLayout3->setContentsMargins(0,0,0,0);
    pWidget3->setLayout(pLayout3);
    ui->tableWidget_3->setCellWidget(ui->tableWidget_3->rowCount()-1,4,pCheckBox3);

    QWidget *pWidget4 = new QWidget();
    QCheckBox *pCheckBox4 = new QCheckBox();
    QHBoxLayout *pLayout4 = new QHBoxLayout(pWidget4);
    pLayout4->addWidget(pCheckBox4);
    pLayout4->setAlignment(Qt::AlignCenter);
    pLayout4->setContentsMargins(0,0,0,0);
    pWidget4->setLayout(pLayout4);
    ui->tableWidget_3->setCellWidget(ui->tableWidget_3->rowCount()-1,5,pCheckBox4);


}

void MainWindow::clearSched(){
    for(int i = ui->tableWidget_3->rowCount() ; i>=0 ; i--){
        ui->tableWidget_3->removeRow(i);

    }
}
