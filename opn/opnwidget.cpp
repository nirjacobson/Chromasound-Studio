#include "opnwidget.h"
#include "ui_opnwidget.h"

OPNWidget::OPNWidget(QWidget *parent, Application* app)
    : QWidget(parent)
    , ui(new Ui::OPNWidget)
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

    connect(ui->pianoWidget, &PianoWidget::keyPressed, this, &OPNWidget::keyPressed);
    connect(ui->pianoWidget, &PianoWidget::keyReleased, this, &OPNWidget::keyReleased);
}

OPNWidget::~OPNWidget()
{
    delete ui;
}

void OPNWidget::setApplication(Application* app)
{
    _app = app;

    ui->operatorWidget1->setApplication(app);
    ui->operatorWidget2->setApplication(app);
    ui->operatorWidget3->setApplication(app);
    ui->operatorWidget4->setApplication(app);

    ui->lfoWidget->setApplication(app);
    ui->algorithmWidget->setApplication(app);
}

void OPNWidget::setSettings(FMChannelSettings* settings)
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

void OPNWidget::pressKey(const int key)
{
    ui->pianoWidget->pressKey(key);
}

void OPNWidget::releaseKey(const int key)
{
    ui->pianoWidget->releaseKey(key);
}

void OPNWidget::doUpdate()
{
    if (_settings) setSettings(_settings);
}

void OPNWidget::newTriggered()
{
    _app->undoStack().push(new SetFMChannelSettingsCommand(_app->window(), *_settings, FMChannelSettings()));
}

void OPNWidget::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "YM2612 Patch (*.opn)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        FMChannelSettings* settings = BSON::decodeOPNPatch(path);
        _app->undoStack().push(new SetFMChannelSettingsCommand(_app->window(), *_settings, *settings));
        delete settings;

        setSettings(_settings);
    }
}

void OPNWidget::saveTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "YM2612 Patch (*.opn)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write(BSON::encodeSettings(_settings));
        file.close();
    }
}

void OPNWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void OPNWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void OPNWidget::dropEvent(QDropEvent* event)
{
    QByteArray data = event->mimeData()->data("text/uri-list");
    QString path(data);
    path = path.mid(QString("file://").length());
    path = path.replace("%20", " ");
    path = path.split("\r\n").at(0);

    QFile file(path);
    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "opn") {
        FMChannelSettings* settings = BSON::decodeOPNPatch(path);
        _app->undoStack().push(new SetFMChannelSettingsCommand(_app->window(), *_settings, *settings));
        delete settings;

        setSettings(_settings);

        event->acceptProposedAction();
    }
}
