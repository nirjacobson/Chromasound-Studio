#include "channelswidget.h"
#include "ui_channelswidget.h"

ChannelsWidget::ChannelsWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::ChannelsWidget),
    _app(app),
    _activeChannelWidget(nullptr),
    _stepKeysWidget(new StepKeys(this, app))
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

    connect(ui->pianoButton, &QPushButton::clicked, this, &ChannelsWidget::pianoButtonClicked);
    connect(ui->velocityButton, &QPushButton::clicked, this, &ChannelsWidget::velocityButtonClicked);

    _stepKeysWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    connect(_stepKeysWidget, &StepKeys::clicked, this, &ChannelsWidget::pianoKeyClicked);
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
        if (cw != channelWidget) {
            cw->setChecked(false);
        }
    }

    _stepKeysWidget->setVisible(false);
    _activeChannelWidget = nullptr;

    if (selected) {
        _activeChannelWidget = channelWidget;

        if (ui->pianoButton->isChecked() && !_app->project().patterns()[_app->activePattern()].getTrack(_activeChannelWidget->index()).doesUsePianoRoll()) {
            adjustPopup();
            _stepKeysWidget->setChannel(channelWidget->index());
            _stepKeysWidget->setVisible(true);
            _stepKeysWidget->raise();
        }
    }
}

void ChannelsWidget::handleSelect(ChannelWidget* channelWidget)
{
    for (ChannelWidget* cw : _channelWidgets) {
        cw->setSelected(cw == channelWidget);
    }
}

void ChannelsWidget::adjustPopup()
{
    if (_activeChannelWidget) {
        const QRect& rect = _activeChannelWidget->getSequencerGeometry();
        QPoint bottomLeft = rect.bottomLeft();
        _stepKeysWidget->setGeometry(QRect(bottomLeft, QSize(rect.width(), 128)));
    }
}

void ChannelsWidget::handleSelectAll()
{
    for (ChannelWidget* cw : _channelWidgets) {
        cw->setSelected(true);
    }
}

void ChannelsWidget::pianoButtonClicked()
{
    _stepKeysWidget->setVisible(false);

    if (ui->pianoButton->isChecked()) {
        ui->velocityButton->setChecked(false);

        if (_activeChannelWidget && !_app->project().patterns()[_app->activePattern()].getTrack(_activeChannelWidget->index()).doesUsePianoRoll()) {
            adjustPopup();
            _stepKeysWidget->setChannel(_activeChannelWidget->index());
            _stepKeysWidget->setVisible(true);
            _stepKeysWidget->raise();
        }
    }
}

void ChannelsWidget::velocityButtonClicked()
{
    if (ui->velocityButton->isChecked()) {
        ui->pianoButton->setChecked(false);
    }
}

void ChannelsWidget::pianoKeyClicked(const Qt::MouseButton button, const int step, const int key)
{
    float beatsPerStep = 0.25;
    Track& track = _app->project().getPattern(_app->activePattern()).getTrack(_activeChannelWidget->index());

    if (button == Qt::LeftButton) {
        auto existingItem = std::find_if(track.items().begin(), track.items().end(), [=](const Track::Item* item){ return item->time() == step * beatsPerStep; });
        if (existingItem == track.items().end()) {
            track.items().append(new Track::Item(step * beatsPerStep, Note(key, beatsPerStep)));
        } else {
            (*existingItem)->note().setKey(key);
        }
    } else {
        track.removeItem(step * beatsPerStep, key);
    }
    _stepKeysWidget->update();
    _activeChannelWidget->update();

}



