#ifndef FMWIDGET_H
#define FMWIDGET_H

#include <QMainWindow>
#include <QFileDialog>

#include "project/channel/fmchannelsettings.h"
#include "bson.h"

namespace Ui {
class FMWidget;
}

class FMWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit FMWidget(QWidget *parent = nullptr);
    ~FMWidget();

    void setSettings(FMChannelSettings* settings);

    void pressKey(const int key);
    void releaseKey(const int key);

signals:
    void keyPressed(const char key, const int velocity);
    void keyReleased(const char key);

private:
    Ui::FMWidget *ui;
    FMChannelSettings* _settings;

private slots:
    void algorithmChanged(const int a);
    void feedbackChanged(const int fb);
    void openTriggered();
    void saveTriggered();
};

#endif // FMWIDGET_H
