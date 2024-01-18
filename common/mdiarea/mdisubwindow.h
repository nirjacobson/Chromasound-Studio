#ifndef MDISUBWINDOW_H
#define MDISUBWINDOW_H

#include <QMdiSubWindow>
#include <QCloseEvent>

#include "mdiarea.h"

class MdiSubWindow : public QMdiSubWindow
{
        Q_OBJECT

    public:
        MdiSubWindow(MdiArea* mdiArea);

    signals:
        void closed();

        // QWidget interface
    protected:
        void closeEvent(QCloseEvent* event);
};

#endif // MDISUBWINDOW_H
