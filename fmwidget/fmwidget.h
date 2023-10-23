#ifndef FMWIDGET_H
#define FMWIDGET_H

#include <QMainWindow>
#include <QFileDialog>
#include <QStyleOption>

#include "application.h"
#include "project/channel/fmchannelsettings.h"
#include "bson.h"

namespace Ui {
class FMWidget;
}

class FMWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit FMWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~FMWidget();

    void setSettings(FMChannelSettings* settings);

    void pressKey(const int key);
    void releaseKey(const int key);

    void doUpdate();

signals:
    void keyPressed(const char key, const int velocity);
    void keyReleased(const char key);

private:
    Ui::FMWidget *ui;
    Application* _app;

    FMChannelSettings* _settings;

private slots:
    void openTriggered();
    void saveTriggered();

    // QWidget interface
protected:
    void paintEvent(QPaintEvent* event);
};

#endif // FMWIDGET_H
