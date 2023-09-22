#ifndef CHANNELWIDGET_H
#define CHANNELWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <iostream>

#include "application.h"

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

    signals:
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

        void nameChanged();

    private slots:
        void ledClicked(bool shift);
        void buttonPressed();
        void buttonContextMenuRequested(const QPoint& p);

    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // CHANNELWIDGET_H
