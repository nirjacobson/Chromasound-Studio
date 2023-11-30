#include "mdisubwindow.h"

MdiSubWindow::MdiSubWindow(MdiArea* mdiArea)
    : QMdiSubWindow(mdiArea)
{
    setWindowIcon(QIcon(QPixmap(1,1)));
}

void MdiSubWindow::closeEvent(QCloseEvent* event)
{
    emit closed();

    QMdiSubWindow::closeEvent(event);
}
