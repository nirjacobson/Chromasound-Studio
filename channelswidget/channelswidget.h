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

        void update();
        void update(const int index);

        void select(const int index);
        int selected() const;

        int activeChannel() const;

    signals:
        void pianoRollTriggered(const int index);
        void deleteTriggered(const int index);
        void channelAdded();
        void moveUpTriggered(const int index);
        void moveDownTriggered(const int index);
        void toneTriggered(const int index);
        void noiseTriggered(const int index);
        void fmTriggered(const int index);
        void channelSelected(const int index);
        void nameChanged(const int index);

    private:
        Ui::ChannelsWidget *ui;
        Application* _app;

        QList<ChannelWidget*> _channelWidgets;
        ChannelWidget* _activeChannelWidget;

        void toggleSolo(ChannelWidget* channelWidget);
        void handleToggle(ChannelWidget* channelWidget, const bool selected);
        void handleSelect(ChannelWidget* channelWidget);

    private slots:
        void handleSelectAll();
        void pianoButtonClicked();
        void velocityButtonClicked();
        void pianoKeyClicked(const Qt::MouseButton button, const int step, const int key);
        void velocityClicked(const int step, const int velocity);

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // CHANNELSWIDGET_H
