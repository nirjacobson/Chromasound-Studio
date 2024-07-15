#include "oplwidget.h"
#include "ui_oplwidget.h"

OPLWidget::OPLWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::OPLWidget),
    _app(app)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    connect(ui->operatorWidget1, &OPLOperatorWidget::changed, this, &OPLWidget::operator1Changed);
    connect(ui->operatorWidget2, &OPLOperatorWidget::changed, this, &OPLWidget::operator2Changed);
    connect(ui->tlDial, &QDial::valueChanged, this, &OPLWidget::tlDialChanged);
    connect(ui->fbDial, &QDial::valueChanged, this, &OPLWidget::fbDialChanged);
}

OPLWidget::~OPLWidget()
{
    delete ui;
}

void OPLWidget::setApplication(Application* app)
{
    _app = app;
}

const OPLSettings& OPLWidget::settings() const
{
    return *_settings;
}

void OPLWidget::setSettings(OPLSettings* settings)
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

void OPLWidget::operator1Changed()
{
    _settings->operators()[0] = ui->operatorWidget1->settings();
    emit changed();
}

void OPLWidget::operator2Changed()
{
    _settings->operators()[1] = ui->operatorWidget2->settings();
    emit changed();
}

void OPLWidget::tlDialChanged(const int value)
{
    _settings->setTl(value);

    emit changed();
}

void OPLWidget::fbDialChanged(const int value)
{
    _settings->setFb(value);

    emit changed();
}

void OPLWidget::newTriggered()
{
    _app->undoStack().push(new EditOPLSettingsCommand(_app->window(), _app->project().userTone(), OPLSettings()));
}

void OPLWidget::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "YM2413 Patch (*.opl)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        OPLSettings* settings = BSON::decodeOPLPatch(path);
        _app->undoStack().push(new EditOPLSettingsCommand(_app->window(), _app->project().userTone(), *settings));
        delete settings;

        setSettings(_settings);
    }
}

void OPLWidget::saveTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "YM2413 Patch (*.opl)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write(BSON::encodePatch(_settings));
        file.close();
    }
}

void OPLWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void OPLWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void OPLWidget::dropEvent(QDropEvent* event)
{
    QByteArray data = event->mimeData()->data("text/uri-list");
    QString path(data);
    path = path.mid(QString("file://").length());
    path = path.replace("%20", " ");
    path = path.split("\r\n").at(0);

    QFile file(path);
    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "opl") {
        OPLSettings* settings = BSON::decodeOPLPatch(path);
        _app->undoStack().push(new EditOPLSettingsCommand(_app->window(), *_settings, *settings));
        delete settings;

        setSettings(_settings);

        event->acceptProposedAction();
    }
}
