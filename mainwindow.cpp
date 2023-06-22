
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
    setWindowTitle(tr("Orzeł wylądował"));
    setWindowIcon(QIcon(":/new/prefix1/pic/icon.png"));

    //Scene setup
    scene = new QGraphicsScene(ui->page_4);
    eagle = lander();
    scene->addItem(eagle.get_eagle());
    ui->viewer->setScene(scene);
    game_start = false;
    ui->VVelLabel->setText(QString::number(eagle.get_velY(), 'd', 2));
    ui->HVelLabel->setText(QString::number(eagle.get_velX(), 'd', 2));
    ui->ScoreLabel->setText(QString::number(eagle.get_score(), 'd', 0));

    ui->EngineStatus->setPixmap(QPixmap(":/new/prefix1/pic/redled.png").scaled(15,15,Qt::KeepAspectRatio));
    ConnectControler();

    //Plot setup
    series = new QSplineSeries();
    for (int i = 0; i < 100; ++i) {
        series->append(i*DT, 0);
    }
    series->setColor(Qt::green);
    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setBackgroundBrush((QBrush(QColor("transparent"))));
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    layout = new QVBoxLayout(ui->horizontalFrame);
    layout->addWidget(chartView);
    axisXindex = 0;
    axisY = new QValueAxis;
    axisY->setRange(-90, 90);
    chartView->chart()->setAxisY(axisY, series);


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
            if (controller_data.at(2) == '1') {
                if(eagle.get_landed()) {
                    scene = new QGraphicsScene(ui->page_4);
                    eagle.reset_state();
                    scene->addItem(eagle.get_eagle());
                    ui->viewer->setScene(scene);
                }
                else
                    on_menuButton_clicked();
            }
            if (controller_data.at(4) == '1' && eagle.get_fuel() > 0) {
                on_startButton_clicked();

                if(!eagle.get_engine()) {
                    eagle.set_engine(true);
                    scene->removeItem(eagle.get_eagle());
                    ui->EngineStatus->setPixmap(QPixmap(":/new/prefix1/pic/greenled.png").scaled(15,15,Qt::KeepAspectRatio));
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
            float r = -controller_data.mid(6).toFloat();
            if(r > 90)
                r = 90;
            else if(r < -90)
                r = -90;

            game(r);
            controller_data = "";
            data_ready = false;
        }
    }
}

/**
 * @brief Handles the game logic.
 *
 * This function is called periodically to update the game state based on the input received and perform necessary calculations and UI updates.
 *
 * @param r The rotation value.
 */
void MainWindow::game(const float &r)
{
    if(game_start) {
        eagle.set_rot(-r);
        eagle.tick();
        ui->VVelLabel->setText(QString::number(eagle.get_velY(), 'd', 2));
        ui->HVelLabel->setText(QString::number(eagle.get_velX(), 'd', 2));
        ui->ScoreLabel->setText(QString::number(eagle.get_score(), 'd', 0));
        ui->HeightBar->setValue(480 - eagle.get_eagle()->pos().y());
        ui->FuelBar->setValue(eagle.get_fuel());
        if(eagle.get_landed()) {
            QPointF pos = eagle.get_eagle()->pos();
            pos.setX(pos.x()-180);
            pos.setY(pos.y()-50);
            if(eagle.get_velY() < 20 && eagle.get_velX() < 20) {
                message = new QGraphicsPixmapItem(QPixmap(":/new/prefix1/pic/landed.png"));
                pos.setX(pos.x()+15);
            }
            else {
                message = new QGraphicsPixmapItem(QPixmap(":/new/prefix1/pic/crashed.png"));
            }
            message->setPos(pos);
            scene->addItem(message);
            game_start = false;
        }
        qDebug()  << eagle.get_velY() << "\n";
    }

    axisXindex++;

    series->remove(0);
    series->append((axisXindex + 100)*DT, r);
    axisX = new QValueAxis;
    axisX->setRange(axisXindex*DT, (axisXindex + 100)*DT);
    chartView->chart()->setAxisX(axisX, series);
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
 * @brief Slot for handling the click event of the "connectButton".
 *
 * This slot is triggered when the "connectButton" is clicked.
 * It checks if the controller port is already open. If it is open,
 * it outputs a debug message indicating that it is already connected and displays the controller's error.
 * If the port is not open, it calls the ConnectControler() function
 * to establish the connection and connects the readyRead() signal of the controller to the read_Port()
 * slot of the MainWindow.
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
 * @brief Slot for handling the click event of the "devButton".
 *
 * This slot is triggered when the "devButton" is clicked.
 * It sets the background image to the game background and switches to the corresponding page in the stacked widget.
 */
void MainWindow::on_devButton_clicked()
{
    ui->centralwidget->setStyleSheet("border-image: url(:/new/prefix1/pic/backgroundGame.png);");
    ui->stackedWidget->setCurrentWidget(ui->page);
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
 * @brief Slot for handling the click event of the "Menu" button.
 *
 * This slot is triggered when the "Menu" button is clicked.
 * It sets the style sheet of the central widget to display a background image,
 * switches the stacked widget to display the third page, and sets the game_start flag to false.
 */
void MainWindow::on_menuButton_clicked()
{
    ui->centralwidget->setStyleSheet("border-image: url(:/new/prefix1/pic/background.png);");
    ui->stackedWidget->setCurrentWidget(ui->page_3);
    game_start = false;
}

/**
 * @brief Slot for handling the click event of the "Menu" button.
 *
 * This slot is triggered when the "Menu" button is clicked.
 * It sets the style sheet of the central widget to display a background image,
 * switches the stacked widget to display the third page, and sets the game_start flag to false.
 */
void MainWindow::on_menuButton_2_clicked()
{
    ui->centralwidget->setStyleSheet("border-image: url(:/new/prefix1/pic/background.png);");
    ui->stackedWidget->setCurrentWidget(ui->page_3);
    game_start = false;
}


void MainWindow::on_pushButton_clicked()
{
    QTranslator *translator = new QTranslator;
    if(!translated){
        translator->load(":/quickt_en.qm");
        translated = true;
    }
    else{
        translator->load(":/quickt_pl.qm");
        translated = false;
    }
    QApplication::instance()->installTranslator(translator);
    ui->retranslateUi(this);
}

