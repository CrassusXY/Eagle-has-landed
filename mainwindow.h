
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "lander.h"
#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QString>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QtCharts>
#include <QChartView>

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
    void on_connectButton_clicked();
    void on_devButton_clicked();
    void on_exitButton_clicked();
    void on_menuButton_clicked();

    void ConnectControler();
    void read_Port();
    void end_Connection();


    void on_menuButton_2_clicked();

private:
    bool game_start;
    lander eagle;
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *message;
    QSerialPort *controller;
    QSplineSeries *series;
    QChartView *chartView;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QVBoxLayout *layout;
    int axisXindex;
    QString controller_data;
    bool data_ready = false;
};

#endif // MAINWINDOW_H
