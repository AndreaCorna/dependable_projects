#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QString"
#include "QHash"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void generateFiles();
    void clear();
    void addNewLine();
    void intermediate();
    void addLineSched();
    void clearSched();

private:
    Ui::MainWindow *ui;
    void setupTables();
    QString dataPath;
    QString scriptPath;
    QHash<QString,double> alphas;
    void generatePartScheduler();
    void updateMap(bool cpu0Active, bool cpu1Active,bool cpu2Active,bool cpu3Active,double duration,int index);
};

#endif // MAINWINDOW_H
