
#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->page_3);
    this->setWindowTitle("Orzeł wylądował");

    scene = new QGraphicsScene(ui->page_4);
    lander = new QGraphicsPixmapItem(QPixmap(":/new/prefix1/pic/lander.png"));
    lander->setScale(.1);
    lander->setTransformOriginPoint(100, 100);
    scene->addItem(lander);
    ui->viewer->setScene(scene);
    ConnectControler();

    ui->EngineStatus->setPixmap(QPixmap(":/new/prefix1/pic/redled.png").scaled(15,15,Qt::KeepAspectRatio));
    connect(controller, &QSerialPort::readyRead, this, &MainWindow::read_Port);
}

MainWindow::~MainWindow()
{
    delete ui;
    controller->close();
}

void MainWindow::ConnectControler()
{
    ui->serialPortPrinter->viewport()->setAutoFillBackground(false);
    ui->serialPortPrinter->setTextColor( QColor( "green" ) );
    ui->serialPortPrinter->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->serialPortPrinter->setFrameStyle(QFrame::NoFrame);
    ui->serialPortPrinter->hide();
    controller = new QSerialPort();
    controller->setPortName("COM9");
    controller->setBaudRate(QSerialPort::BaudRate::Baud115200);
    controller->setParity(QSerialPort::Parity::NoParity);
    controller->setDataBits(QSerialPort::DataBits::Data8);
    controller->setStopBits(QSerialPort::StopBits::OneStop);
    controller->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    controller->open(QIODevice::ReadOnly);

    if(controller->isOpen()){
        qDebug() << "Connected";
        qDebug() << controller->error();
        ui->ControlerStatus->setPixmap(QPixmap(":/new/prefix1/pic/greenled.png").scaled(15,15,Qt::KeepAspectRatio));
    }
    else {
        qDebug() << "Not connected";
        qDebug() << controller->error();
        ui->ControlerStatus->setPixmap(QPixmap(":/new/prefix1/pic/redled.png").scaled(15,15,Qt::KeepAspectRatio));
    }
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
            if (controller_data.at(2) == '1'){
                ui->serialPortPrinter->append("Pauza!\n");
                on_testButton_clicked();
            }
            if (controller_data.at(4) == '1'){
                ui->serialPortPrinter->append("Silnik włączony!\n");
                on_startButton_clicked();
            }
            float r = controller_data.mid(6).toFloat();
            lander->setRotation(r);
            controller_data = "";
            data_ready = false;
        }
    }
}

void MainWindow::on_startButton_clicked()
{
    ui->centralwidget->setStyleSheet("border-image: url(:/new/prefix1/pic/backgroundGame.png);");
    ui->stackedWidget->setCurrentWidget(ui->page_4);
}


void MainWindow::on_exitButton_clicked()
{
    close();
}


void MainWindow::on_testButton_clicked()
{
    ui->centralwidget->setStyleSheet("border-image: url(:/new/prefix1/pic/background.png);");
    ui->stackedWidget->setCurrentWidget(ui->page_3);
    ui->serialPortPrinter->hide();
}


void MainWindow::on_devButton_clicked()
{
    ui->centralwidget->setStyleSheet("border-image: url(:/new/prefix1/pic/backgroundGame.png);");
    ui->stackedWidget->setCurrentWidget(ui->page_4);
    ui->serialPortPrinter->show();
}

