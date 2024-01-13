#include "fm4widget.h"
#include "ui_fm4widget.h"

FM4Widget::FM4Widget(QWidget *parent, Application* app)
    : QWidget(parent)
    , ui(new Ui::FM4Widget)
    , _app(app)
    , _settings(nullptr)
{
    ui->setupUi(this);

    ui->operatorWidget1->setApplication(app);
    ui->operatorWidget2->setApplication(app);
    ui->operatorWidget3->setApplication(app);
    ui->operatorWidget4->setApplication(app);

    ui->lfoWidget->setApplication(app);
    ui->algorithmWidget->setApplication(app);

    setAcceptDrops(true);

    connect(ui->pianoWidget, &PianoWidget::keyPressed, this, &FM4Widget::keyPressed);
    connect(ui->pianoWidget, &PianoWidget::keyReleased, this, &FM4Widget::keyReleased);
}

FM4Widget::~FM4Widget()
{
    delete ui;
}

void FM4Widget::setApplication(Application* app)
{
    _app = app;

    ui->operatorWidget1->setApplication(app);
    ui->operatorWidget2->setApplication(app);
    ui->operatorWidget3->setApplication(app);
    ui->operatorWidget4->setApplication(app);

    ui->lfoWidget->setApplication(app);
    ui->algorithmWidget->setApplication(app);
}

void FM4Widget::setSettings(FMChannelSettings* settings)
{
    _settings = settings;

    ui->operatorWidget1->blockSignals(true);
    ui->operatorWidget1->setSettings(&settings->operators()[0]);
    ui->operatorWidget1->blockSignals(false);

    ui->operatorWidget2->blockSignals(true);
    ui->operatorWidget2->setSettings(&settings->operators()[1]);
    ui->operatorWidget2->blockSignals(false);

    ui->operatorWidget3->blockSignals(true);
    ui->operatorWidget3->setSettings(&settings->operators()[2]);
    ui->operatorWidget3->blockSignals(false);

    ui->operatorWidget4->blockSignals(true);
    ui->operatorWidget4->setSettings(&settings->operators()[3]);
    ui->operatorWidget4->blockSignals(false);

    ui->lfoWidget->blockSignals(true);
    ui->lfoWidget->setSettings(&settings->lfo());
    ui->lfoWidget->blockSignals(false);

    ui->algorithmWidget->blockSignals(true);
    ui->algorithmWidget->setSettings(&settings->algorithm());
    ui->algorithmWidget->blockSignals(false);
}

void FM4Widget::pressKey(const int key)
{
    ui->pianoWidget->pressKey(key);
}

void FM4Widget::releaseKey(const int key)
{
    ui->pianoWidget->releaseKey(key);
}

void FM4Widget::doUpdate()
{
    if (_settings) setSettings(_settings);
}

void FM4Widget::newTriggered()
{
    _app->undoStack().push(new SetFMChannelSettingsCommand(_app->window(), *_settings, FMChannelSettings()));
}

void FM4Widget::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "YM2612 Patch (*.fm)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        FMChannelSettings* settings = BSON::decodePatch(path);
        _app->undoStack().push(new SetFMChannelSettingsCommand(_app->window(), *_settings, *settings));
        delete settings;

        setSettings(_settings);
    }
}

void FM4Widget::saveTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "YM2612 Patch (*.fm)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write(BSON::encodePatch(_settings));
        file.close();
    }
}

void FM4Widget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FM4Widget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void FM4Widget::dropEvent(QDropEvent* event)
{
    QByteArray data = event->mimeData()->data("text/uri-list");
    QString path(data);
    path = path.mid(QString("file://").length());
    path = path.replace("%20", " ");
    path = path.replace("\r\n", "");

    QFile file(path);
    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "fm") {
        FMChannelSettings* settings = BSON::decodePatch(path);
        _app->undoStack().push(new SetFMChannelSettingsCommand(_app->window(), *_settings, *settings));
        delete settings;

        setSettings(_settings);

        event->acceptProposedAction();
    }
}
