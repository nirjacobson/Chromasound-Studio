#include "ssgglobalswidget.h"
#include "ui_ssgglobalswidget.h"

SSGGlobalsWidget::SSGGlobalsWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::SSGGlobalsWidget),
    _app(app)
{
    ui->setupUi(this);

    ui->envShapeWidget->set(_app->project().ssgEnvelopeShape());
    ui->envFreqWidget->set(_app->project().ssgEnvelopeFrequency());
    ui->noiseFreqWidget->set(_app->project().ssgNoiseFrequency());

    connect(ui->envShapeWidget, &SSGEnvelopeShapeWidget::changed, this, &SSGGlobalsWidget::envelopeShapeChanged);
    connect(ui->envFreqWidget, &SSGEnvelopeFreqWidget::changed, this, &SSGGlobalsWidget::envelopeFrequencyChanged);
    connect(ui->noiseFreqWidget, &SSGNoiseFreqWidget::changed, this, &SSGGlobalsWidget::noiseFrequencyChanged);
}

SSGGlobalsWidget::~SSGGlobalsWidget()
{
    delete ui;
}

void SSGGlobalsWidget::doUpdate()
{
    ui->envShapeWidget->set(_app->project().ssgEnvelopeShape());
    ui->envFreqWidget->set(_app->project().ssgEnvelopeFrequency());
    ui->noiseFreqWidget->set(_app->project().ssgNoiseFrequency());
}

void SSGGlobalsWidget::envelopeShapeChanged()
{
    _app->undoStack().push(new EditSSGEnvelopeSettingsCommand(_app->window(), _app->project().ssgEnvelopeShape(), ui->envShapeWidget->settings()));
}

void SSGGlobalsWidget::envelopeFrequencyChanged()
{
    _app->undoStack().push(new EditProjectEnvelopeFrequencyCommand(_app->window(), _app->project(), ui->envFreqWidget->setting()));
}

void SSGGlobalsWidget::noiseFrequencyChanged()
{
    _app->undoStack().push(new EditProjectNoiseFrequencyCommand(_app->window(), _app->project(), ui->noiseFreqWidget->setting()));
}

void SSGGlobalsWidget::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}
