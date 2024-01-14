#ifndef FM2WIDGET_H
#define FM2WIDGET_H

#include <QWidget>
#include <QPainter>

#include <QFileDialog>
#include <QStyleOption>
#include <QMimeData>
#include <QDragEnterEvent>

#include "application.h"
#include "fm2settings.h"
#include "commands/editfm2settingscommand.h"
#include "bson.h"

namespace Ui {
    class FM2Widget;
}

class FM2Widget : public QWidget
{
        Q_OBJECT

    public:
        explicit FM2Widget(QWidget *parent = nullptr, Application* app = nullptr);
        ~FM2Widget();

        void setApplication(Application* app);

        const FM2Settings& settings() const;
        void setSettings(FM2Settings* settings);

    public slots:
        void newTriggered();
        void openTriggered();
        void saveTriggered();

    signals:
        void changed();

    private:
        Ui::FM2Widget *ui;
        Application* _app;
        FM2Settings* _settings;

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

#endif // FM2WIDGET_H
