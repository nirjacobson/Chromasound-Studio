#ifndef CHANNELWIDGET_H
#define CHANNELWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QMimeData>

#include "application.h"
#include "commands/movechannelupcommand.h"
#include "commands/movechanneldowncommand.h"
#include "commands/deletechannelcommand.h"
#include "commands/editchannelvolumecommand.h"
#include "commands/setchanneltypecommand.h"
#include "commands/setfmchannelsettingscommand.h"
#include "commands/setpcmchannelsettingscommand.h"
#include "commands/removetrackitemscommand.h"
#include "commands/addtrackitemscommand.h"
#include "commands/setchanneltypecommand.h"
#include "bson.h"
#include "midi/midi.h"
#include "midi/midifile.h"

namespace Ui {
    class ChannelWidget;
}

class ChannelWidget : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor fmColor READ fmColor WRITE setFMColor)
        Q_PROPERTY(QColor toneColor READ toneColor WRITE setToneColor)
        Q_PROPERTY(QColor noiseColor READ noiseColor WRITE setNoiseColor)

    public:
        explicit ChannelWidget(QWidget *parent = nullptr, Application* app = nullptr, int index = 0);
        ~ChannelWidget();

        void setOn(const bool on);
        void setChecked(const bool checked);
        void setSelected(const bool selected);

        bool on() const;
        bool checked() const;

        int index() const;
        void setIndex(const int idx);
        void setVolume(const int volume);

        void showStepKeysWidget();
        void showStepVelsWidget();
        void hideStepWidgets();

        const QRect getSequencerGeometry();

        void fromPath(const QString& path);

    private:
        Ui::ChannelWidget *ui;
        Application* _app;
        int _index;

        QMenu _contextMenu;
        QAction _pianoRollAction;
        QAction _moveUpAction;
        QAction _moveDownAction;
        QAction _deleteAction;

        QActionGroup _typeActionGroup;
        QAction _toneAction;
        QAction _noiseAction;
        QAction _fmAction;
        QAction _pcmAction;

        QAction _fillEvery2StepsAction;
        QAction _fillEvery4StepsAction;

        QColor _fmColor;
        QColor _toneColor;
        QColor _noiseColor;
        QColor _pcmColor;

        const QColor& fmColor() const;
        const QColor& toneColor() const;
        const QColor& noiseColor() const;
        const QColor& pcmColor() const;

        void setFMColor(const QColor& color);
        void setToneColor(const QColor& color);
        void setNoiseColor(const QColor& color);
        void setPCMColor(const QColor& color);

    signals:
        void pianoKeyClicked(const Qt::MouseButton button, const int step, const int key);
        void velocityClicked(const int step, const int velocity);

        void ledShiftClicked();
        void toggled(bool);

        void rectLedClicked();
        void rectLedDoubleClicked();
        void pianoRollTriggered(bool on);

        void selected();

        void nameChanged();

    private slots:
        void ledClicked(bool shift);
        void buttonPressed();
        void buttonContextMenuRequested(const QPoint& p);
        void pianoRollWasToggled();
        void pianoRollWasTriggered();
        void toneWasTriggered();
        void noiseWasTriggered();
        void fmWasTriggered();
        void pcmWasTriggered();
        void volumeDialChanged(const int val);

        void deleteTriggered();
        void moveUpTriggered();
        void moveDownTriggered();

        void fillEvery2StepsTriggered();
        void fillEvery4StepsTriggered();

    protected:
        void paintEvent(QPaintEvent* event);
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);

};

#endif // CHANNELWIDGET_H
