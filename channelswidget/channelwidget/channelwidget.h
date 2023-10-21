#ifndef CHANNELWIDGET_H
#define CHANNELWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <iostream>

#include "application.h"
#include "commands/editchannelvolumecommand.h"

namespace Ui {
    class ChannelWidget;
}

class ChannelWidget : public QWidget
{
        Q_OBJECT

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

    signals:
        void pianoKeyClicked(const Qt::MouseButton button, const int step, const int key);
        void velocityClicked(const int step, const int velocity);

        void ledShiftClicked();
        void toggled(bool);

        void rectLedClicked();
        void rectLedDoubleClicked();
        void pianoRollTriggered();
        void deleteTriggered();
        void moveUpTriggered();
        void moveDownTriggered();

        void toneTriggered();
        void noiseTriggered();
        void fmTriggered();

        void nameChanged();

    private slots:
        void ledClicked(bool shift);
        void buttonPressed();
        void buttonContextMenuRequested(const QPoint& p);
        void pianoRollWasTriggered();
        void volumeDialChanged(const int val);

    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // CHANNELWIDGET_H
