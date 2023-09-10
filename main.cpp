#include "application.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    a.project().addChannel();
    a.project().addChannel();
    a.project().addChannel();
    a.project().addChannel();

    MainWindow w(nullptr, &a);
    w.show();
    return a.exec();
}
