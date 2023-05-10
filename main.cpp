#include "mainwindow.h"

#include <QApplication>

/**

@brief The main function of the application.
This function initializes the QApplication instance, creates the main window, shows it, and runs the application event loop.
@param argc The number of command-line arguments.
@param argv The command-line arguments as an array of C-style strings.
@return The exit code of the application.
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
