#ifndef MODELWIDGET_H
#define MODELWIDGET_H

#include <QWidget>
#include <QListWidgetItem>

#undef slots
#include <torch/torch.h>
#define slots Q_SLOTS

namespace Ui {
class ModelWidget;
}

class ModelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModelWidget(QWidget *parent = nullptr);
    ~ModelWidget();

    std::vector<int> layerSizes();
    void setLayerSizes(const std::vector<int>& layerSizes);
    void setLayerSizes(const std::vector<torch::Tensor>& layerSizes);

private:
    Ui::ModelWidget *ui;

private slots:
    void layerDoubleClicked(QListWidgetItem *item);
    void upClicked();
    void downClicked();
    void addClicked();
    void removedClicked();
    void updateDisplay();
};

#endif // MODELWIDGET_H
