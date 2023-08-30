#include "channelswidget.h"
#include "ui_channelswidget.h"

ChannelsWidget::ChannelsWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::ChannelsWidget),
    _app(app)
{
    ui->setupUi(this);

    for (int i = 0; i < app->project().channels(); i++) {
        _channelWidgets.append(new ChannelWidget(this, app, i));
    }

    for (ChannelWidget* channelWidget : _channelWidgets) {
        layout()->addWidget(channelWidget);
        connect(channelWidget, &ChannelWidget::ledShiftClicked, this, [=](){ toggleSolo(channelWidget); });
        connect(channelWidget, &ChannelWidget::toggled, this, [=](bool selected){ handleToggle(channelWidget, selected); });
        connect(channelWidget, &ChannelWidget::rectLedClicked, this, [=](){ handleSelect(channelWidget); });
        connect(channelWidget, &ChannelWidget::rectLedDoubleClicked, this, &ChannelsWidget::handleSelectAll);
        connect(channelWidget, &ChannelWidget::pianoRollTriggered, this, [=](){ emit pianoRollTriggered(channelWidget->index()); });
    }

    if (!_channelWidgets.isEmpty()) {
        StepCursorWidget* stepCursorWidget = new StepCursorWidget(this, app);
        QWidget* widget = new QWidget();
        widget->setLayout(new QHBoxLayout);
        widget->layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));
        widget->layout()->addWidget(stepCursorWidget);
        layout()->addWidget(widget);
    }
}

ChannelsWidget::~ChannelsWidget()
{
    for (ChannelWidget* channelWidget : _channelWidgets) {
        delete channelWidget;
    }

    delete ui;
}

void ChannelsWidget::toggleSolo(ChannelWidget* channelWidget)
{
    bool on = channelWidget->on();

    for (ChannelWidget* cw : _channelWidgets) {
        cw->setOn(cw == channelWidget ? on : !on);
    }
}

void ChannelsWidget::handleToggle(ChannelWidget* channelWidget, const bool selected)
{
    for (ChannelWidget* cw : _channelWidgets) {
        cw->setChecked(cw == channelWidget ? selected : false);
    }
}

void ChannelsWidget::handleSelect(ChannelWidget* channelWidget)
{
    for (ChannelWidget* cw : _channelWidgets) {
        cw->setSelected(cw == channelWidget);
    }
}

void ChannelsWidget::handleSelectAll()
{
    for (ChannelWidget* cw : _channelWidgets) {
        cw->setSelected(true);
    }
}

