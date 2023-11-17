#include "fmwidget.h"
#include "ui_fmwidget.h"

FMWidget::FMWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::FMWidget),
    _app(app)
{
    ui->setupUi(this);

    ui->operatorWidget1->setApplication(app);
    ui->operatorWidget2->setApplication(app);
    ui->operatorWidget3->setApplication(app);
    ui->operatorWidget4->setApplication(app);

    ui->algorithmWidget->setApplication(app);

    setAcceptDrops(true);

    connect(ui->pianoWidget, &PianoWidget::keyPressed, this, &FMWidget::keyPressed);
    connect(ui->pianoWidget, &PianoWidget::keyReleased, this, &FMWidget::keyReleased);
}

FMWidget::~FMWidget()
{
    delete ui;
}

void FMWidget::setApplication(Application* app)
{
    _app = app;

    ui->operatorWidget1->setApplication(app);
    ui->operatorWidget2->setApplication(app);
    ui->operatorWidget3->setApplication(app);
    ui->operatorWidget4->setApplication(app);

    ui->algorithmWidget->setApplication(app);
}

void FMWidget::setSettings(FMChannelSettings* settings)
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

    ui->algorithmWidget->blockSignals(true);
    ui->algorithmWidget->setSettings(&settings->algorithm());
    ui->algorithmWidget->blockSignals(false);
}

void FMWidget::pressKey(const int key)
{
    ui->pianoWidget->pressKey(key);
}

void FMWidget::releaseKey(const int key)
{
    ui->pianoWidget->releaseKey(key);
}

void FMWidget::doUpdate()
{
    setSettings(_settings);
}

void FMWidget::newTriggered()
{
    _app->undoStack().push(new SetFMChannelSettingsCommand(_app->window(), *_settings, FMChannelSettings()));
}

void FMWidget::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "YM2612 Patch (*.fm)");

    if (!path.isNull()) {
        FMChannelSettings* settings = BSON::decodePatch(path);
        _app->undoStack().push(new SetFMChannelSettingsCommand(_app->window(), *_settings, *settings));
        delete settings;

        setSettings(_settings);
    }
}

void FMWidget::saveTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "YM2612 Patch (*.fm)");

    if (!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write(BSON::encodePatch(_settings));
        file.close();
    }
}

void FMWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FMWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void FMWidget::dropEvent(QDropEvent* event)
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
