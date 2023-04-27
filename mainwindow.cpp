
#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->page_3);

    ui->serialPortPrinter->viewport()->setAutoFillBackground(false);
    ui->serialPortPrinter->setTextColor( QColor( "green" ) );
    ui->serialPortPrinter->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->serialPortPrinter->setFrameStyle(QFrame::NoFrame);
    ui->serialPortPrinter->hide();
    this->setWindowTitle("Orzeł wylądował");

    controller = new QSerialPort();
    controller->setPortName("COM5");
    controller->setBaudRate(QSerialPort::BaudRate::Baud115200);
    controller->setParity(QSerialPort::Parity::NoParity);
    controller->setDataBits(QSerialPort::DataBits::Data8);
    controller->setStopBits(QSerialPort::StopBits::OneStop);
    controller->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    controller->open(QIODevice::ReadOnly);

    if(controller->isOpen()){
        qDebug() << "Connected";
        qDebug() << controller->error();
    }
    else {
        qDebug() << "Not connected";
        qDebug() << controller->error();
    }

    connect(controller, &QSerialPort::readyRead, this, &MainWindow::read_Port);
}

MainWindow::~MainWindow()
{
    delete ui;
    controller->close();
}

void MainWindow::read_Port()
{
    if(controller->isOpen()){
        while(controller->bytesAvailable()) {
            controller_data += controller->readAll();
        }
        if (controller_data.at(controller_data.length() - 1) == char(10))
            data_ready = true;

        if (data_ready) {
            qDebug() << "Paczka: "  << controller_data;
            ui->serialPortPrinter->append(controller_data);
            if (controller_data.at(0) == '1'){
                ui->serialPortPrinter->append("Pauza!\n");
                on_testButton_clicked();
            }
            if (controller_data.at(2) == '1'){
                ui->serialPortPrinter->append("Silnik włączony!\n");
                on_startButton_clicked();
            }
            controller_data = "";
            data_ready = false;
        }
    }
}

void MainWindow::on_startButton_clicked()
{
    ui->centralwidget->setStyleSheet("border-image: url(:/new/prefix1/backgroundGame.png);");
    ui->stackedWidget->setCurrentWidget(ui->page_4);
}


void MainWindow::on_exitButton_clicked()
{
    close();
}


void MainWindow::on_testButton_clicked()
{
    ui->centralwidget->setStyleSheet("border-image: url(:/new/prefix1/background.png);");
    ui->stackedWidget->setCurrentWidget(ui->page_3);
    ui->serialPortPrinter->hide();
}


void MainWindow::on_devButton_clicked()
{
    ui->centralwidget->setStyleSheet("border-image: url(:/new/prefix1/backgroundGame.png);");
    ui->stackedWidget->setCurrentWidget(ui->page_4);
    ui->serialPortPrinter->show();
}

