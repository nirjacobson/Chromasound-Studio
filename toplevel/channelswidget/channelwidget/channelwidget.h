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
#include "commands/composite/setfmchannelcommand.h"
#include "commands/composite/setpcmchannelcommand.h"
#include "commands/composite/replacetrackitemscommand.h"
#include "commands/setpcmchannelsettingscommand.h"
#include "commands/removetrackitemscommand.h"
#include "commands/addtrackitemscommand.h"
#include "commands/setchanneltypecommand.h"
#include "commands/setchannelnamecommand.h"
#include "commands/setchannelenabledcommand.h"
#include "formats/bson.h"
#include "formats/midi/midi.h"
#include "formats/midi/midifile.h"

namespace Ui {
    class ChannelWidget;
}

class ChannelWidget : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor fmColor READ fmColor WRITE setFMColor)
        Q_PROPERTY(QColor toneColor READ toneColor WRITE setToneColor)
        Q_PROPERTY(QColor noiseColor READ noiseColor WRITE setNoiseColor)
        Q_PROPERTY(QColor ssgColor READ ssgColor WRITE setSSGColor)
        Q_PROPERTY(QColor melodyColor READ melodyColor WRITE setMelodyColor)
        Q_PROPERTY(QColor rhythmColor READ rhythmColor WRITE setRhythmColor)
        Q_PROPERTY(QColor pcmColor READ pcmColor WRITE setPCMColor)

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

        void doUpdate(const float position, const bool full);

    private:
        Ui::ChannelWidget *ui;
        Application* _app;
        float _appPosition;

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
        QAction _ssgAction;
        QAction _melodyAction;
        QAction _rhythmAction;
        QAction _pcmAction;

        QAction _fillEvery2StepsAction;
        QAction _fillEvery4StepsAction;

        QColor _fmColor;
        QColor _toneColor;
        QColor _noiseColor;
        QColor _ssgColor;
        QColor _melodyColor;
        QColor _rhythmColor;
        QColor _pcmColor;

        const QColor& fmColor() const;
        const QColor& toneColor() const;
        const QColor& noiseColor() const;
        const QColor& ssgColor() const;
        const QColor& melodyColor() const;
        const QColor& rhythmColor() const;
        const QColor& pcmColor() const;

        void setFMColor(const QColor& color);
        void setToneColor(const QColor& color);
        void setNoiseColor(const QColor& color);
        void setSSGColor(const QColor& color);
        void setMelodyColor(const QColor& color);
        void setRhythmColor(const QColor& color);
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

    private slots:
        void ledClicked(bool shift);
        void buttonPressed();
        void buttonContextMenuRequested(const QPoint& p);
        void pianoRollWasToggled();
        void pianoRollWasTriggered();
        void toneWasTriggered();
        void noiseWasTriggered();
        void fmWasTriggered();
        void ssgWasTriggered();
        void melodyWasTriggered();
        void rhythmWasTriggered();
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
