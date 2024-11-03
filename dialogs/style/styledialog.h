#ifndef STYLEDIALOG_H
#define STYLEDIALOG_H

#include <QMainWindow>
#include <QFileDialog>
#include <QStyleOption>
#include <QPainter>

#include "application.h"
#include "common/mdiarea/mdisubwindow.h"

namespace Ui {
    class StyleDialog;
}

class StyleDialog : public QMainWindow
{
        Q_OBJECT

    public:
        explicit StyleDialog(QWidget *parent = nullptr);
        ~StyleDialog();

        void setApplication(Application* app);

    private:
        Ui::StyleDialog *ui;
        Application* _app;

    private slots:
        void newTriggered();
        void openTriggered();
        void saveTriggered();

        void reset();
        void apply();

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
        void closeEvent(QCloseEvent* event);
};

#endif // STYLEDIALOG_H
