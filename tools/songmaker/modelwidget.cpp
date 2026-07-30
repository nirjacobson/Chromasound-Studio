#include "modelwidget.h"
#include "ui_modelwidget.h"

ModelWidget::ModelWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModelWidget)
{
    ui->setupUi(this);

    ui->upPushButton->setIcon(ui->upPushButton->style()->standardIcon(QStyle::SP_ArrowUp));
    ui->downPushButton->setIcon(ui->downPushButton->style()->standardIcon(QStyle::SP_ArrowDown));

    connect(ui->layersListWidget, &QListWidget::itemDoubleClicked, this, &ModelWidget::layerDoubleClicked);
    connect(ui->layersListWidget, &QListWidget::itemChanged, this, &ModelWidget::updateDisplay);
    connect(ui->upPushButton, &QPushButton::clicked, this, &ModelWidget::upClicked);
    connect(ui->downPushButton, &QPushButton::clicked, this, &ModelWidget::downClicked);
    connect(ui->addPushButton, &QPushButton::clicked, this, &ModelWidget::addClicked);
    connect(ui->removePushButton, &QPushButton::clicked, this, &ModelWidget::removedClicked);

    updateDisplay();
}

ModelWidget::~ModelWidget()
{
    delete ui;
}

std::vector<int> ModelWidget::layerSizes()
{
    std::vector<int> sizes;
    for (int i = 0; i < ui->layersListWidget->count(); i++) {
        sizes.push_back(ui->layersListWidget->item(i)->text().toInt());
    }
    return sizes;
}

void ModelWidget::setLayerSizes(const std::vector<int> &layerSizes)
{
    ui->layersListWidget->clear();
    for (int i = 0; i < layerSizes.size(); i++) {
        QListWidgetItem* newItem = new QListWidgetItem();
        newItem->setText(QString::number(layerSizes[i]));
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        ui->layersListWidget->addItem(newItem);
    }
    updateDisplay();
}

void ModelWidget::setLayerSizes(const std::vector<at::Tensor> &layerSizes)
{
    ui->layersListWidget->clear();
    for (int i = 0; i < layerSizes.size(); i++) {
        QListWidgetItem* newItem = new QListWidgetItem();
        newItem->setText(QString::number(layerSizes[i].item().toInt()));
        newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        ui->layersListWidget->addItem(newItem);
    }
    updateDisplay();
}

void ModelWidget::updateDisplay()
{
    ui->displayWidget->setLayerSizes(layerSizes());
}

void ModelWidget::layerDoubleClicked(QListWidgetItem *item)
{
    ui->layersListWidget->editItem(item);
}

void ModelWidget::upClicked()
{
    if (ui->layersListWidget->count() > 1) {
        int currentRow = ui->layersListWidget->currentRow();
        if (currentRow > 0) {
            QListWidgetItem* item = ui->layersListWidget->takeItem(currentRow);
            ui->layersListWidget->insertItem(currentRow - 1, item);
            ui->layersListWidget->setCurrentRow(currentRow - 1);
            updateDisplay();
        }
    }
}

void ModelWidget::downClicked()
{
    if (ui->layersListWidget->count() > 1) {
        int currentRow = ui->layersListWidget->currentRow();
        if (currentRow >= 0 && currentRow < ui->layersListWidget->count() - 1) {
            QListWidgetItem* item = ui->layersListWidget->takeItem(currentRow);
            ui->layersListWidget->insertItem(currentRow + 1, item);
            ui->layersListWidget->setCurrentRow(currentRow + 1);
            updateDisplay();
        }
    }
}

void ModelWidget::addClicked()
{
    QListWidgetItem* newItem = new QListWidgetItem();
    newItem->setText("8");
    newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    ui->layersListWidget->insertItem(ui->layersListWidget->count(), newItem);
    updateDisplay();
}

void ModelWidget::removedClicked()
{
    if (ui->layersListWidget->selectedItems().size() == 1) {
        int currentRow = ui->layersListWidget->currentRow();
        ui->layersListWidget->takeItem(currentRow);
        updateDisplay();
    }
}
