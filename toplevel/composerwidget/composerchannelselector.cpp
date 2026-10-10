#include "composerchannelselector.h"
#include "ui_composerchannelselector.h"

ComposerChannelSelector::ComposerChannelSelector(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ComposerChannelSelector)
{
    ui->setupUi(this);
}

ComposerChannelSelector::~ComposerChannelSelector()
{
    delete ui;
}
