#ifndef OPLWIDGET_H
#define OPLWIDGET_H

#include <QWidget>
#include <QPainter>

#include <QFileDialog>
#include <QStyleOption>
#include <QMimeData>
#include <QDragEnterEvent>

#include "application.h"
#include "oplsettings.h"
#include "commands/editoplsettingscommand.h"
#include "formats/bson.h"

namespace Ui {
    class OPLWidget;
}

class OPLWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit OPLWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~OPLWidget();

        void setApplication(Application* app);

        const OPLSettings& settings() const;
        void setSettings(OPLSettings* settings);

    public slots:
        void newTriggered();
        void openTriggered();
        void saveTriggered();

    signals:
        void changed();

    private:
        Ui::OPLWidget *ui;
        Application* _app;
        OPLSettings* _settings;

    private slots:
        void operator1Changed();
        void operator2Changed();
        void tlDialChanged(const int value);
        void fbDialChanged(const int value);

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);

        // QWidget interface
    protected:
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);
};

#endif // OPLWIDGET_H
