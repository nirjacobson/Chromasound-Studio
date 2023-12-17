#include "pcmwidget.h"
#include "ui_pcmwidget.h"

PCMWidget::PCMWidget(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::PCMWidget)
    , _app(app)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    connect(ui->actionOpen, &QAction::triggered, this, &PCMWidget::openTriggered);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);
}

PCMWidget::~PCMWidget()
{
    delete ui;
}

void PCMWidget::setSettings(PCMChannelSettings* settings)
{
    _settings = settings;
    ui->pathLabel->setText(QFileInfo(_settings->path()).fileName());
}

void PCMWidget::doUpdate()
{
    setSettings(_settings);
}

void PCMWidget::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "PCM audio (*.pcm)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        _settings->setPath(path);
        ui->pathLabel->setText(QFileInfo(path).fileName());
    }
}

void PCMWidget::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}

void PCMWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void PCMWidget::dropEvent(QDropEvent* event)
{
    QByteArray data = event->mimeData()->data("text/uri-list");
    QString path(data);
    path = path.mid(QString("file://").length());
    path = path.replace("%20", " ");
    path = path.replace("\r\n", "");

    PCMChannelSettings* settings = new PCMChannelSettings;
    settings->setPath(path);

    _app->undoStack().push(new SetPCMChannelSettingsCommand(_app->window(), *_settings, *settings));
    delete settings;

    setSettings(_settings);

    event->acceptProposedAction();
}
