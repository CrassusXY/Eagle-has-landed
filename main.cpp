#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
/**

@brief The entry point of the application.
This function is the entry point of the application. It initializes the application, installs a translator for
localization support, creates an instance of the MainWindow class, and starts the application event loop.
@param argc The number of command-line arguments.
@param argv An array of command-line arguments.
@return The exit code of the application.
*/
int main(int argc, char *argv[])
{
    //QTranslator translator;
    //translator.load(":/quickt_en.qm");
    QApplication a(argc, argv);
    //a.installTranslator(&translator);
    MainWindow w;
    w.show();
    return a.exec();
}
