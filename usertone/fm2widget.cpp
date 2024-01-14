#include "fm2widget.h"
#include "ui_fm2widget.h"

FM2Widget::FM2Widget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::FM2Widget),
    _app(app)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    connect(ui->operatorWidget1, &FM2OperatorWidget::changed, this, &FM2Widget::operator1Changed);
    connect(ui->operatorWidget2, &FM2OperatorWidget::changed, this, &FM2Widget::operator2Changed);
    connect(ui->tlDial, &QDial::valueChanged, this, &FM2Widget::tlDialChanged);
    connect(ui->fbDial, &QDial::valueChanged, this, &FM2Widget::fbDialChanged);
}

FM2Widget::~FM2Widget()
{
    delete ui;
}

void FM2Widget::setApplication(Application* app)
{
    _app = app;
}

const FM2Settings& FM2Widget::settings() const
{
    return *_settings;
}

void FM2Widget::setSettings(FM2Settings* settings)
{
    _settings = settings;

    ui->operatorWidget1->blockSignals(true);
    ui->operatorWidget1->set(_settings->operators()[0]);
    ui->operatorWidget1->blockSignals(false);

    ui->operatorWidget2->blockSignals(true);
    ui->operatorWidget2->set(_settings->operators()[1]);
    ui->operatorWidget2->blockSignals(false);

    ui->tlDial->blockSignals(true);
    ui->tlDial->setValue(_settings->tl());
    ui->tlDial->blockSignals(false);

    ui->fbDial->blockSignals(true);
    ui->fbDial->setValue(_settings->fb());
    ui->fbDial->blockSignals(false);

    ui->tlValueLabel->setText(QString::number(_settings->tl()));
    ui->fbDial->setValue(_settings->fb());
    ui->fbValueLabel->setText(QString::number(_settings->fb()));
}

void FM2Widget::operator1Changed()
{
    _settings->operators()[0] = ui->operatorWidget1->settings();
    emit changed();
}

void FM2Widget::operator2Changed()
{
    _settings->operators()[1] = ui->operatorWidget2->settings();
    emit changed();
}

void FM2Widget::tlDialChanged(const int value)
{
    _settings->setTl(value);

    emit changed();
}

void FM2Widget::fbDialChanged(const int value)
{
    _settings->setFb(value);

    emit changed();
}

void FM2Widget::newTriggered()
{
    _app->undoStack().push(new EditFM2SettingsCommand(_app->window(), _app->project().userTone(), FM2Settings()));
}

void FM2Widget::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "YM2413 Patch (*.opl)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        FM2Settings* settings = BSON::decodeFM2Patch(path);
        _app->undoStack().push(new EditFM2SettingsCommand(_app->window(), _app->project().userTone(), *settings));
        delete settings;

        setSettings(_settings);
    }
}

void FM2Widget::saveTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "YM2413 Patch (*.opl)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write(BSON::encodePatch(_settings));
        file.close();
    }
}

void FM2Widget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FM2Widget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void FM2Widget::dropEvent(QDropEvent* event)
{
    QByteArray data = event->mimeData()->data("text/uri-list");
    QString path(data);
    path = path.mid(QString("file://").length());
    path = path.replace("%20", " ");
    path = path.replace("\r\n", "");

    QFile file(path);
    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "opl") {
        FM2Settings* settings = BSON::decodeFM2Patch(path);
        _app->undoStack().push(new EditFM2SettingsCommand(_app->window(), *_settings, *settings));
        delete settings;

        setSettings(_settings);

        event->acceptProposedAction();
    }
}
