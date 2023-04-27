
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();
    void on_exitButton_clicked();
    void on_testButton_clicked();
    void read_Port();

    void on_devButton_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *controller;
    QString controller_data;
    bool data_ready = false;
};

#endif // MAINWINDOW_H