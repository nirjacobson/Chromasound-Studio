#include "lfowidget.h"
#include "ui_lfowidget.h"

LFOWidget::LFOWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LFOWidget)
{
    ui->setupUi(this);

    connect(ui->modeComboBox, &QComboBox::currentIndexChanged, this, &LFOWidget::modeComboBoxCurrentIndexChanged);
}

LFOWidget::~LFOWidget()
{
    delete ui;
}

int LFOWidget::setting() const
{
    return ui->modeComboBox->currentIndex();
}

void LFOWidget::set(const int mode)
{
    ui->modeComboBox->blockSignals(true);
    ui->modeComboBox->setCurrentIndex(mode);
    ui->modeComboBox->blockSignals(false);
}


void LFOWidget::modeComboBoxCurrentIndexChanged(const int index)
{
    emit changed();
}

