#ifndef EMULATIONSETTINGSWIDGET_H
#define EMULATIONSETTINGSWIDGET_H

#include <QWidget>
#include <QSettings>

#include "Chromasound_Studio.h"

namespace Ui {
    class EmulationSettingsWidget;
}

class EmulationSettingsWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit EmulationSettingsWidget(QWidget *parent = nullptr);
        ~EmulationSettingsWidget();

        int bass() const;
        int treble() const;
        int audioBufferSize() const;
        int playbackReadBufferSize() const;
        int interactiveReadBufferSize() const;

        void setDual(const bool dual);

    private:
        Ui::EmulationSettingsWidget *ui;
};

#endif // EMULATIONSETTINGSWIDGET_H
