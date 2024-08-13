#ifndef CHANNELSWIDGET_H
#define CHANNELSWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QList>
#include <QWindow>
#include <QStyle>
#include <QStyleOption>

#include "application.h"
#include "channelwidget/channelwidget.h"
#include "stepcursorwidget.h"
#include "commands/addnotecommand.h"
#include "commands/removenotecommand.h"
#include "commands/editnotecommand.h"
#include "commands/addchannelcommand.h"
#include "commands/composite/addtrackcommand.h"
#include "commands/setchanneltypecommand.h"
#include "commands/composite/addfmchannelcommand.h"
#include "commands/composite/addpcmchannelcommand.h"

namespace Ui {
    class ChannelsWidget;
}

class ChannelsWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit ChannelsWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~ChannelsWidget();

        void rebuild();
        void remove(const int index);
        void add(const int index);
        void setVolume(const int index, const int volume);

        void doUpdate(const float position, const bool full = false);
        void update(const int index);
        void update();

        void select(const int index);
        int selected() const;

        int activeChannel() const;

    signals:
        void pianoRollTriggered(const int index, const bool on);
        void channelSelected(const int index);

    private:
        Ui::ChannelsWidget *ui;
        Application* _app;

        QList<ChannelWidget*> _channelWidgets;
        ChannelWidget* _activeChannelWidget;
        StepCursorWidget* _stepCursorWidget;

        void toggleSolo(ChannelWidget* channelWidget);
        void handleToggle(ChannelWidget* channelWidget, const bool selected);
        void handleSelect(ChannelWidget* channelWidget);

    private slots:
        void handleSelectAll();
        void pianoButtonClicked();
        void velocityButtonClicked();
        void pianoKeyClicked(const Qt::MouseButton button, const int step, const int key);
        void velocityClicked(const int step, const int velocity);

        void channelAdded();

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);
};

#endif // CHANNELSWIDGET_H
