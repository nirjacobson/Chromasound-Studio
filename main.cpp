#include "application.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);

    MainWindow w(nullptr, &a);
    w.show();
    return a.exec();
}
