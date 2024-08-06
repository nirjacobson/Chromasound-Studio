#include "application.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    MainWindow w(nullptr, &a);
    a.setStyle("fusion");
    a.setWindow(&w);
    a.showWindow();

    return a.exec();
}
