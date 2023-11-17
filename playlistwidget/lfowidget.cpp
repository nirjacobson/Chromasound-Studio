#include "lfowidget.h"
#include "ui_lfowidget.h"

LFOWidget::LFOWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LFOWidget)
    , _lfoChange(nullptr)
{
    ui->setupUi(this);

    connect(ui->modeComboBox, &QComboBox::currentIndexChanged, this, &LFOWidget::modeComboBoxCurrentIndexChanged);
}

LFOWidget::~LFOWidget()
{
    delete ui;
}

void LFOWidget::setLFOChange(Playlist::LFOChange* change)
{
    _lfoChange = change;
    ui->modeComboBox->setCurrentIndex(change->mode());
}

void LFOWidget::modeComboBoxCurrentIndexChanged(const int index)
{
    _lfoChange->setMode(index);
}

