#include "mdisubwindow.h"

MdiSubWindow::MdiSubWindow(MdiArea* mdiArea)
    : QMdiSubWindow(mdiArea)
{

}

void MdiSubWindow::closeEvent(QCloseEvent* event)
{
    emit closed();
    event->accept();
}
