#include "pcmwidget.h"
#include "ui_pcmwidget.h"

PCMWidget::PCMWidget(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::PCMWidget)
    , _app(app)
{
    ui->setupUi(this);

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

void PCMWidget::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "PCM audio (*.pcm)");

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
