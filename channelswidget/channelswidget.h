#ifndef CHANNELSWIDGET_H
#define CHANNELSWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QList>

#include "application.h"
#include "channelwidget.h"
#include "stepcursorwidget.h"
#include "stepkeys.h"
#include "stepvelocities.h"

namespace Ui {
    class ChannelsWidget;
}

class ChannelsWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit ChannelsWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~ChannelsWidget();

    signals:
        void pianoRollTriggered(const int index);

    private:
        Ui::ChannelsWidget *ui;
        Application* _app;

        QList<ChannelWidget*> _channelWidgets;
        ChannelWidget* _activeChannelWidget;

        StepKeys* _stepKeysWidget;
        StepVelocities* _stepVelocitiesWidget;

        void toggleSolo(ChannelWidget* channelWidget);
        void handleToggle(ChannelWidget* channelWidget, const bool selected);
        void handleSelect(ChannelWidget* channelWidget);

        void adjustPopup();

    private slots:
        void handleSelectAll();
        void pianoButtonClicked();
        void velocityButtonClicked();
        void pianoKeyClicked(const Qt::MouseButton button, const int step, const int key);
        void velocityClicked(const int step, const int velocity);

        // QWidget interface
    protected:
        void showEvent(QShowEvent*);
};

#endif // CHANNELSWIDGET_H
