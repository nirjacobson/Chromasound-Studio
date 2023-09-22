#include "channelswidget.h"
#include "ui_channelswidget.h"

ChannelsWidget::ChannelsWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::ChannelsWidget),
    _app(app),
    _activeChannelWidget(nullptr),
    _stepKeysWidget(new StepKeys(this, app)),
    _stepVelocitiesWidget(new StepVelocities(this, app))
{
    ui->setupUi(this);

    rebuild();

    connect(ui->pianoButton, &QPushButton::clicked, this, &ChannelsWidget::pianoButtonClicked);
    connect(ui->velocityButton, &QPushButton::clicked, this, &ChannelsWidget::velocityButtonClicked);
    connect(ui->addButton, &QPushButton::clicked, this, &ChannelsWidget::channelAdded);

    _stepKeysWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    _stepVelocitiesWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    connect(_stepKeysWidget, &StepKeys::clicked, this, &ChannelsWidget::pianoKeyClicked);
    connect(_stepVelocitiesWidget, &StepVelocities::clicked, this, &ChannelsWidget::velocityClicked);
}

ChannelsWidget::~ChannelsWidget()
{
    for (ChannelWidget* channelWidget : _channelWidgets) {
        delete channelWidget;
    }

    delete _stepVelocitiesWidget;
    delete _stepKeysWidget;

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
        connect(channelWidget, &ChannelWidget::nameChanged, this, [=](){ emit nameChanged(channelWidget->index()); });
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
    for (ChannelWidget* cw : _channelWidgets) {
        cw->setChecked(false);
    }
    _channelWidgets[index]->setChecked(true);
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
    _stepVelocitiesWidget->setVisible(false);
    _activeChannelWidget = nullptr;

    if (selected) {
        _activeChannelWidget = channelWidget;

        const Pattern& pattern = _app->project().getFrontPattern();
        if (pattern.hasTrack(_activeChannelWidget->index())) {
            const Track& track = pattern.getTrack(_activeChannelWidget->index());
            if (!track.doesUsePianoRoll()) {
                if (ui->pianoButton->isChecked()) {
                    adjustPopup();
                    _stepKeysWidget->setChannel(channelWidget->index());
                    _stepKeysWidget->setVisible(true);
                    _stepKeysWidget->raise();
                } else if (ui->velocityButton->isChecked() && !track.items().empty()) {
                    adjustPopup();
                    _stepVelocitiesWidget->setChannel(channelWidget->index());
                    _stepVelocitiesWidget->setVisible(true);
                    _stepVelocitiesWidget->raise();
                }
            }
        } else {
            if (ui->pianoButton->isChecked()) {
                adjustPopup();
                _stepKeysWidget->setChannel(channelWidget->index());
                _stepKeysWidget->setVisible(true);
                _stepKeysWidget->raise();
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

void ChannelsWidget::adjustPopup()
{
    if (_activeChannelWidget) {
        const QRect& rect = _activeChannelWidget->getSequencerGeometry();
        QPoint bottomLeft = rect.bottomLeft();
        _stepKeysWidget->setGeometry(QRect(bottomLeft, QSize(rect.width(), 128)));
        _stepVelocitiesWidget->setGeometry(QRect(bottomLeft, QSize(rect.width(), 128)));
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

        if (_activeChannelWidget) {
            const Pattern& pattern = _app->project().getFrontPattern();
            if (pattern.hasTrack(_activeChannelWidget->index())) {
                const Track& track = pattern.getTrack(_activeChannelWidget->index());
                if(!track.doesUsePianoRoll()) {
                    adjustPopup();
                    _stepVelocitiesWidget->setVisible(false);
                    _stepKeysWidget->setChannel(_activeChannelWidget->index());
                    _stepKeysWidget->setVisible(true);
                    _stepKeysWidget->raise();
                }
            } else {
                adjustPopup();
                _stepKeysWidget->setChannel(_activeChannelWidget->index());
                _stepKeysWidget->setVisible(true);
                _stepKeysWidget->raise();
            }
        }
    }
}

void ChannelsWidget::velocityButtonClicked()
{
    _stepVelocitiesWidget->setVisible(false);

    if (ui->velocityButton->isChecked()) {
        ui->pianoButton->setChecked(false);

        if (_activeChannelWidget) {
            const Pattern& pattern = _app->project().getFrontPattern();
            if (pattern.hasTrack(_activeChannelWidget->index())) {
                const Track& track = pattern.getTrack(_activeChannelWidget->index());
                if (!track.doesUsePianoRoll() && !track.items().empty()) {
                    adjustPopup();
                    _stepKeysWidget->setVisible(false);
                    _stepVelocitiesWidget->setChannel(_activeChannelWidget->index());
                    _stepVelocitiesWidget->setVisible(true);
                    _stepVelocitiesWidget->raise();
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
    _stepKeysWidget->update();
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

    _stepVelocitiesWidget->update();
    _activeChannelWidget->update();
}

void ChannelsWidget::showEvent(QShowEvent*)
{
    adjustSize();
}



