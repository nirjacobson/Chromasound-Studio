#include "channelswidget.h"
#include "ui_channelswidget.h"

ChannelsWidget::ChannelsWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::ChannelsWidget),
    _app(app),
    _activeChannelWidget(nullptr)
{
    ui->setupUi(this);

    rebuild();

    connect(ui->pianoButton, &QPushButton::clicked, this, &ChannelsWidget::pianoButtonClicked);
    connect(ui->velocityButton, &QPushButton::clicked, this, &ChannelsWidget::velocityButtonClicked);
    connect(ui->addButton, &QPushButton::clicked, this, &ChannelsWidget::channelAdded);
}

ChannelsWidget::~ChannelsWidget()
{
    for (ChannelWidget* channelWidget : _channelWidgets) {
        delete channelWidget;
    }

    delete ui;
}

void ChannelsWidget::rebuild()
{
    QList<QLayoutItem*> items;
    for (int i = 0; i < ui->verticalLayout->count(); i++) {
        items.append(ui->verticalLayout->itemAt(i));
    }
    for (QLayoutItem* item : items) {
        ui->verticalLayout->removeItem(item);
        delete item->widget();
        delete item;
    }

    _channelWidgets.clear();

    for (int i = 0; i < _app->project().channels(); i++) {
        _channelWidgets.append(new ChannelWidget(this, _app, i));
    }

    for (ChannelWidget* channelWidget : _channelWidgets) {
        ui->verticalLayout->addWidget(channelWidget);
        connect(channelWidget, &ChannelWidget::ledShiftClicked, this, [=](){ toggleSolo(channelWidget); });
        connect(channelWidget, &ChannelWidget::toggled, this, [=](bool selected){ handleToggle(channelWidget, selected); });
        connect(channelWidget, &ChannelWidget::rectLedClicked, this, [=](){ handleSelect(channelWidget); });
        connect(channelWidget, &ChannelWidget::rectLedDoubleClicked, this, &ChannelsWidget::handleSelectAll);
        connect(channelWidget, &ChannelWidget::pianoRollTriggered, this, [=](){ emit pianoRollTriggered(channelWidget->index()); });
        connect(channelWidget, &ChannelWidget::moveUpTriggered, this, [=](){ emit moveUpTriggered(channelWidget->index()); });
        connect(channelWidget, &ChannelWidget::moveDownTriggered, this, [=](){ emit moveDownTriggered(channelWidget->index()); });
        connect(channelWidget, &ChannelWidget::deleteTriggered, this, [=](){ emit deleteTriggered(channelWidget->index()); });
        connect(channelWidget, &ChannelWidget::toneTriggered, this, [=](){ emit toneTriggered(channelWidget->index()); });
        connect(channelWidget, &ChannelWidget::noiseTriggered, this, [=](){ emit noiseTriggered(channelWidget->index()); });
        connect(channelWidget, &ChannelWidget::fmTriggered, this, [=](){ emit fmTriggered(channelWidget->index()); });
        connect(channelWidget, &ChannelWidget::nameChanged, this, [=](){ emit nameChanged(channelWidget->index()); });
        connect(channelWidget, &ChannelWidget::pianoKeyClicked, this, &ChannelsWidget::pianoKeyClicked);
        connect(channelWidget, &ChannelWidget::velocityClicked, this, &ChannelsWidget::velocityClicked);
    }

    if (!_channelWidgets.isEmpty()) {
        StepCursorWidget* stepCursorWidget = new StepCursorWidget(this, _app);
        QWidget* widget = new QWidget();
        widget->setLayout(new QHBoxLayout);
        widget->layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));
        widget->layout()->addWidget(stepCursorWidget);
        ui->verticalLayout->addWidget(widget);
    }

}

void ChannelsWidget::remove(const int index)
{
    QLayoutItem* item = ui->verticalLayout->itemAt(index);
    ui->verticalLayout->removeItem(item);
    item->widget()->deleteLater();
    delete item;
    _channelWidgets.removeAt(index);

    for (int i = index; i < _channelWidgets.size(); i++) {
        _channelWidgets[i]->setIndex(_channelWidgets[i]->index() - 1);
    }
}

void ChannelsWidget::add(const int index)
{
    ChannelWidget* cw = new ChannelWidget(this, _app, index);
    connect(cw, &ChannelWidget::ledShiftClicked, this, [=](){ toggleSolo(cw); });
    connect(cw, &ChannelWidget::toggled, this, [=](bool selected){ handleToggle(cw, selected); });
    connect(cw, &ChannelWidget::rectLedClicked, this, [=](){ handleSelect(cw); });
    connect(cw, &ChannelWidget::rectLedDoubleClicked, this, &ChannelsWidget::handleSelectAll);
    connect(cw, &ChannelWidget::pianoRollTriggered, this, [=](){ emit pianoRollTriggered(cw->index()); });
    connect(cw, &ChannelWidget::moveUpTriggered, this, [=](){ emit moveUpTriggered(cw->index()); });
    connect(cw, &ChannelWidget::moveDownTriggered, this, [=](){ emit moveDownTriggered(cw->index()); });
    connect(cw, &ChannelWidget::deleteTriggered, this, [=](){ emit deleteTriggered(cw->index()); });
    connect(cw, &ChannelWidget::toneTriggered, this, [=](){ emit toneTriggered(cw->index()); });
    connect(cw, &ChannelWidget::noiseTriggered, this, [=](){ emit noiseTriggered(cw->index()); });
    connect(cw, &ChannelWidget::nameChanged, this, [=](){ emit nameChanged(cw->index()); });
    connect(cw, &ChannelWidget::pianoKeyClicked, this, &ChannelsWidget::pianoKeyClicked);
    connect(cw, &ChannelWidget::velocityClicked, this, &ChannelsWidget::velocityClicked);
    ui->verticalLayout->insertWidget(index, cw);

    _channelWidgets.insert(index, cw);

    for (int i = index + 1; i < _channelWidgets.size(); i++) {
        _channelWidgets[i]->setIndex(i);
    }
}

void ChannelsWidget::update()
{
    QWidget::update();
}

void ChannelsWidget::update(const int index)
{
    _channelWidgets[index]->setIndex(index);
}

void ChannelsWidget::select(const int index)
{
    _channelWidgets[index]->setChecked(true);
    handleToggle(_channelWidgets[index], true);
}

int ChannelsWidget::selected() const
{
    for (int i = 0; i < _channelWidgets.size(); i++) {
        if (_channelWidgets[i]->checked()) {
            return i;
        }
    }

    return -1;
}

int ChannelsWidget::activeChannel() const
{
    if (!_activeChannelWidget) return -1;

    return _activeChannelWidget->index();
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

    if (_activeChannelWidget) {
        _activeChannelWidget->hideStepWidgets();
    }

    _activeChannelWidget = nullptr;

    if (selected) {
        _activeChannelWidget = channelWidget;
        _activeChannelWidget->setChecked(true);

        const Pattern& pattern = _app->project().getFrontPattern();
        if (pattern.hasTrack(_activeChannelWidget->index())) {
            const Track& track = pattern.getTrack(_activeChannelWidget->index());
            if (!track.doesUsePianoRoll()) {
                if (ui->pianoButton->isChecked()) {
                    _activeChannelWidget->showStepKeysWidget();
                } else if (ui->velocityButton->isChecked() && !track.items().empty()) {
                    _activeChannelWidget->showStepVelsWidget();
                }
            }
        } else {
            if (ui->pianoButton->isChecked()) {
                _activeChannelWidget->showStepKeysWidget();
            }
        }

        emit channelSelected(channelWidget->index());
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

void ChannelsWidget::pianoButtonClicked()
{
    if (_activeChannelWidget) {
        _activeChannelWidget->hideStepWidgets();
    }

    if (ui->pianoButton->isChecked()) {
        ui->velocityButton->setChecked(false);

        if (_activeChannelWidget) {
            const Pattern& pattern = _app->project().getFrontPattern();
            if (pattern.hasTrack(_activeChannelWidget->index())) {
                const Track& track = pattern.getTrack(_activeChannelWidget->index());
                if(!track.doesUsePianoRoll()) {
                    _activeChannelWidget->showStepKeysWidget();
                }
            } else {
                _activeChannelWidget->showStepKeysWidget();
            }
        }
    }
}

void ChannelsWidget::velocityButtonClicked()
{
    if (_activeChannelWidget) {
        _activeChannelWidget->hideStepWidgets();
    }

    if (ui->velocityButton->isChecked()) {
        ui->pianoButton->setChecked(false);

        if (_activeChannelWidget) {
            const Pattern& pattern = _app->project().getFrontPattern();
            if (pattern.hasTrack(_activeChannelWidget->index())) {
                const Track& track = pattern.getTrack(_activeChannelWidget->index());
                if (!track.doesUsePianoRoll() && !track.items().empty()) {
                    _activeChannelWidget->showStepVelsWidget();
                }
            }
        }
    }
}

void ChannelsWidget::pianoKeyClicked(const Qt::MouseButton button, const int step, const int key)
{
    float beatsPerStep = 0.25;
    Track& track = _app->project().getFrontPattern().getTrack(_activeChannelWidget->index());

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
    _activeChannelWidget->update();

}

void ChannelsWidget::velocityClicked(const int step, const int velocity)
{
    float beatsPerStep = 0.25;
    Track& track = _app->project().getFrontPattern().getTrack(_activeChannelWidget->index());

    auto existingItem = std::find_if(track.items().begin(), track.items().end(), [=](const Track::Item* item){ return item->time() == step * beatsPerStep; });
    if (existingItem != track.items().end()) {
        (*existingItem)->note().setVelocity(velocity);
    }

    _activeChannelWidget->update();
}

void ChannelsWidget::showEvent(QShowEvent*)
{
    adjustSize();
}



