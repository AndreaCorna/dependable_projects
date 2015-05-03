#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QString"

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

private:
    Ui::MainWindow *ui;
    void setupTables();
    QString dataPath;
    QString scriptPath;
};

#endif // MAINWINDOW_H
