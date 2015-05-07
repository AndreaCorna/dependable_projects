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

private:
    Ui::MainWindow *ui;
    void setupTables();
    QString dataPath;
    QString scriptPath;
    QHash<QString,double> alphas;
};

#endif // MAINWINDOW_H
