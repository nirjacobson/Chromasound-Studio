#include "channelwidget.h"
#include "ui_channelwidget.h"
#include <QtDebug>

ChannelWidget::ChannelWidget(QWidget *parent, Application* app, int index) :
    QWidget(parent),
    ui(new Ui::ChannelWidget),
    _app(app),
    _index(index),
    _contextMenu(tr("Context menu"), this),
    _pianoRollAction("Piano roll", this)
{
    ui->setupUi(this);

    ui->stepSequencer->setApplication(app);
    ui->stepSequencer->setIndex(index);

    ui->prDisplay->setApplication(app);
    ui->prDisplay->setIndex(index);

    ui->led->setOn(_app->project().getChannel(_index).enabled());
    ui->rectLed->setOnFunction([=](){
        QMap<int, float> activePatterns = _app->project().activePatternsAtTime(_app->position());
        const Pattern& activePattern = _app->project().patterns()[_app->activePattern()];
        const Track& track = activePattern.getTrack(index);
        if (_app->playMode() == Application::PlayMode::Pat) {
            return _app->isPlaying() &&
                   _app->project().getChannel(_index).enabled() &&
                    activePattern.activeTracksAtTime(_app->position()).contains(index) &&
                    std::find_if(track.items().begin(),
                              track.items().end(),
                     [&](const Track::Item* item){ return qAbs(item->time() - _app->position()) <= 0.0625f; }) == track.items().end();
        } else {
           return _app->isPlaying() &&
                   activePatterns.contains(_app->activePattern()) &&
                  _app->project().getChannel(_index).enabled() &&
                  activePattern.activeTracksAtTime(_app->position() - activePatterns[_app->activePattern()]).contains(index) &&
                  std::find_if(track.items().begin(), track.items().end(),
                  [&](const Track::Item* item){
                    float delta = item->time() - (_app->position() - activePatterns[_app->activePattern()]);
                    return delta >= 0 && delta <= 0.0625f; }) == track.items().end();
        }
    });

    connect(ui->pushButton, &QPushButton::pressed, this, &ChannelWidget::buttonPressed);
    connect(ui->pushButton, &QWidget::customContextMenuRequested, this, &ChannelWidget::buttonContextMenuRequested);
    connect(ui->led, &LED::clicked, this, &ChannelWidget::ledClicked);

    connect(ui->rectLed, &RectLED::clicked, this, &ChannelWidget::rectLedClicked);
    connect(ui->rectLed, &RectLED::doubleClicked, this, &ChannelWidget::rectLedDoubleClicked);

    connect(&_pianoRollAction, &QAction::triggered, this, &ChannelWidget::pianoRollTriggered);

    _contextMenu.addAction(&_pianoRollAction);
}

ChannelWidget::~ChannelWidget()
{
    delete ui;
}

void ChannelWidget::setOn(const bool on)
{
    Channel& channel = _app->project().getChannel(_index);
    channel.setEnabled(on);

    ui->led->setOn(on);
}

void ChannelWidget::setChecked(const bool checked)
{
    ui->pushButton->setChecked(checked);
}

void ChannelWidget::setSelected(const bool selected)
{
    ui->rectLed->setSelected(selected);
}

bool ChannelWidget::on() const
{
    return _app->project().getChannel(_index).enabled();
}

int ChannelWidget::index() const
{
    return _index;
}

const QRect ChannelWidget::getSequencerGeometry()
{
    QRect rect;
    rect.setBottomLeft(ui->stepSequencer->mapToGlobal(ui->stepSequencer->geometry().bottomLeft()));
    rect.setWidth(ui->stepSequencer->width());
    return rect;
}

void ChannelWidget::ledClicked(bool shift)
{
    Channel& channel = _app->project().getChannel(_index);
    channel.setEnabled(!channel.enabled());

    ui->led->setOn(channel.enabled());

    if (shift) {
        emit ledShiftClicked();
    }
}

void ChannelWidget::buttonPressed()
{
    if (Qt::ShiftModifier == QApplication::keyboardModifiers()) {
        bool ok;
        const QString name = QInputDialog::getText(this, tr("Change Channel Name"), tr("Channel name:"), QLineEdit::Normal, "", &ok);
        if (ok && !name.isEmpty()) {
            ui->pushButton->setText(name);
        }
    } else {
        bool state = ui->pushButton->isChecked();
        emit toggled(!state);
    }
}

void ChannelWidget::buttonContextMenuRequested(const QPoint& p)
{
    _contextMenu.exec(mapToGlobal(p));
}

void ChannelWidget::paintEvent(QPaintEvent* event)
{
    const Pattern& activePattern = _app->project().getPattern(_app->activePattern());
    ui->stackedWidget->setCurrentIndex(activePattern.hasTrack(_index) && activePattern.getTrack(_index).doesUsePianoRoll());

    QWidget::paintEvent(event);
}
