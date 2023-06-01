
#include "mainwindow.h"
#include "./ui_mainwindow.h"

/**

@brief Constructs a MainWindow object.

This constructor sets up the user interface, initializes various class members and variables, and connects controller signals to slots.

@param parent The parent widget of the main window.
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Ui setup
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->page_3);
    setWindowTitle("Orzeł wylądował");
    setWindowIcon(QIcon(":/new/prefix1/pic/icon.png"));

    //Serial printer setup
    ui->serialPortPrinter->viewport()->setAutoFillBackground(false);
    ui->serialPortPrinter->setTextColor( QColor( "green" ) );
    ui->serialPortPrinter->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->serialPortPrinter->setFrameStyle(QFrame::NoFrame);
    ui->serialPortPrinter->hide();

    //Scene setup
    scene = new QGraphicsScene(ui->page_4);
    eagle = lander();
    scene->addItem(eagle.get_eagle());
    ui->viewer->setScene(scene);
    game_start = false;

    ConnectControler();
    ui->EngineStatus->setPixmap(QPixmap(":/new/prefix1/pic/redled.png").scaled(15,15,Qt::KeepAspectRatio));

    ui->VVelLabel->setText(QString::number(eagle.get_velY(), 'd', 2));
    ui->HVelLabel->setText(QString::number(eagle.get_velX(), 'd', 2));

    //Signals
    connect(controller, &QSerialPort::readyRead, this, &MainWindow::read_Port);
    connect(controller, &QSerialPort::errorOccurred, this, &MainWindow::end_Connection);
}

/**

@brief Destroys the MainWindow object.
This destructor frees up resources used by the class, including the user interface and the serial controller.
*/
MainWindow::~MainWindow()
{
    delete ui;
    controller->close();
}

/**

@brief Sets up and connects the serial port controller.

This function creates a new serial port controller object, sets its properties (port name, baud rate, parity, data bits, stop bits, and flow control),
and opens it for reading. It also checks whether the controller was successfully opened.
If the connection is successful, it sets the ControlerStatus LED to green.
If the connection is not successful, it sets the ControlerStatus LED to red.
*/
void MainWindow::ConnectControler()
{
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
    }
}

/**
 * @brief Reads data from the serial port and processes it.
 *
 * This function is called whenever there is data available to be read from the serial port. It reads the data and
 * stores it in a buffer until a complete "packet" is received (indicated by a newline character). When a complete
 * packet is received, it is processed based on its content:
 *
 *  - If the menu bit equals 1, the menu page is displayed.
 *  - If the start bit equals 1, the game page is displayed or engine starts.
 *  - The remaining characters (starting from the seventh) are parsed as a float and used to rotate the lander
 *    sprite in the game.
 *
 * Additionally, this function updates the lander sprite animation based on a count value (updated on each function
 * call) and the state of the controller (which is used to determine whether the engine should be on or off).
 *
 * @note This function is connected to the QSerialPort::readyRead() signal in order to be called when data is
 * available to be read from the serial port.
 */
void MainWindow::read_Port()
{
    if(controller->isOpen()){
        while(controller->bytesAvailable()) {
            controller_data += controller->readAll();
        }
        // Check if a complete packet has been received
        if (controller_data.at(controller_data.length() - 1) == char(10) && controller_data.at(0) == 'X')
            data_ready = true;

        // Process the packet if one is ready
        if (data_ready) {
            //qDebug() << "Paczka: "  << controller_data;
            ui->serialPortPrinter->append(controller_data);
            if (controller_data.at(2) == '1') {
                ui->serialPortPrinter->append("Pauza!\n");
                on_menuButton_clicked();  
            }
            if (controller_data.at(4) == '1' && eagle.get_fuel() > 0) {
                ui->serialPortPrinter->append("Silnik włączony!\n");
                on_startButton_clicked();

                ui->EngineStatus->setPixmap(QPixmap(":/new/prefix1/pic/greenled.png").scaled(15,15,Qt::KeepAspectRatio));

                if(!eagle.get_engine()) {
                    eagle.set_engine(true);
                    scene->removeItem(eagle.get_eagle());
                    eagle.set_PixMap(":/new/prefix1/pic/EngineOn1.png");
                    scene->addItem(eagle.get_eagle());
                }
            }
            else if (controller_data.at(4) == '0' || eagle.get_fuel() == 0) {
                ui->EngineStatus->setPixmap(QPixmap(":/new/prefix1/pic/redled.png").scaled(15,15,Qt::KeepAspectRatio));

                if(eagle.get_engine()) {
                    eagle.set_engine(false);
                    scene->removeItem(eagle.get_eagle());
                    eagle.set_PixMap(":/new/prefix1/pic/lander.png");
                    scene->addItem(eagle.get_eagle());
                }
            }

            // Parse the remaining characters as a float and use it to rotate the lander sprite
            float r = controller_data.mid(6).toFloat();
            if(r > 90)
                r = 90;
            else if(r < -90)
                r = -90;

            if(game_start) {
                eagle.set_rot(-r);
                eagle.tick();
                ui->VVelLabel->setText(QString::number(eagle.get_velY(), 'd', 2));
                ui->HVelLabel->setText(QString::number(eagle.get_velX(), 'd', 2));
                ui->HeightBar->setValue(480 - eagle.get_eagle()->pos().y());
                ui->FuelBar->setValue(eagle.get_fuel());
                if(eagle.get_landed()) {
                    if(eagle.get_velY() < 20) {
                        message = new QGraphicsPixmapItem(QPixmap(":/new/prefix1/pic/landed.png"));
                        QPointF pos = eagle.get_eagle()->pos();
                        pos.setX(pos.x()-200);
                        scene->addItem(message);
                    }
                    else {
                        message = new QGraphicsPixmapItem(QPixmap(":/new/prefix1/pic/crashed.png"));
                        QPointF pos = eagle.get_eagle()->pos();
                        pos.setX(pos.x()-200);
                        message->setPos(pos);
                        scene->addItem(message);
                    }

                }
                qDebug()  << eagle.get_velY() << "\n";
                //qDebug()  << r << ", " <<((cos(r * M_PI / 180) * ENGINE_ACC ) - MOON_ACC) << ", " << sin(r*M_PI/180) << "\n";
            }
            controller_data = "";
            data_ready = false;
        }
    }
}

/**
 * @brief Disconnects the controller from the serial port.
 *
 * This function closes the controller and sets the ControlerStatus LED to red.
 */
void MainWindow::end_Connection()
{
    controller->close();
    ui->ControlerStatus->setPixmap(QPixmap(":/new/prefix1/pic/redled.png").scaled(15,15,Qt::KeepAspectRatio));
}

/**
 * @brief Handles the startButton click event.
 *
 * This function sets the background image to the game background and switches to the game screen.
 */
void MainWindow::on_startButton_clicked()
{
    game_start = true;
    ui->centralwidget->setStyleSheet("border-image: url(:/new/prefix1/pic/backgroundGame.png);");
    ui->stackedWidget->setCurrentWidget(ui->page_4);
    //    if(!controller->isOpen()){

    //    }
}

/**
 * @brief Handles the connectButton click event.
 *
 * This function attempts to connect the controller to the serial port and sets up the readyRead signal.
 * If the controller is already connected, it does nothing.
 */
void MainWindow::on_connectButton_clicked()
{
    if(controller->isOpen()){
        qDebug() << "Already connected";
        qDebug() << controller->error();
    }
    else{
        ConnectControler();
        connect(controller, &QSerialPort::readyRead, this, &MainWindow::read_Port);
    }
}

/**
 * @brief Handles the devButton click event.
 *
 * This function sets the background image to the game background, switches to the game screen, and shows the serial printer.
 */
void MainWindow::on_devButton_clicked()
{
    ui->centralwidget->setStyleSheet("border-image: url(:/new/prefix1/pic/backgroundGame.png);");
    ui->stackedWidget->setCurrentWidget(ui->page_4);
    ui->serialPortPrinter->show();
}

/**
 * @brief Handles the exitButton click event.
 *
 * This function closes the MainWindow object.
 */
void MainWindow::on_exitButton_clicked()
{
    close();
}

/**
 * @brief Handles the menuButton click event.
 *
 * This function sets the background image to the main menu background and switches to the main menu screen.
 * It also hides the serial printer.
 */
void MainWindow::on_menuButton_clicked()
{
    ui->centralwidget->setStyleSheet("border-image: url(:/new/prefix1/pic/background.png);");
    ui->stackedWidget->setCurrentWidget(ui->page_3);
    ui->serialPortPrinter->hide();
    game_start = false;
}

