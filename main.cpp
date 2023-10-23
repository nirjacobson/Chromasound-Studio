#include "application.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    MainWindow* w = new MainWindow(nullptr, &a);
    a.setWindow(w);
    a.showWindow();

    return a.exec();
}
