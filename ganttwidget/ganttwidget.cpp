#include "ganttwidget.h"
#include "ui_ganttwidget.h"

GanttWidget::GanttWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::GanttWidget),
    _app(app),
    _leftWidget(nullptr),
    _bottomWidget(nullptr),
    _rows(0),
    _rowHeight(16),
    _cellWidth(16),
    _cellBeats(1)
{
    ui->setupUi(this);
    ui->headerWidget->setApplication(_app);
    ui->headerWidget->setCellWidth(_cellWidth);
    ui->headerWidget->setCellBeats(_cellBeats);
    ui->editorWidget->setApplication(_app);
    ui->editorWidget->setRowHeight(_rowHeight);
    ui->editorWidget->setCellWidth(_cellWidth);
    ui->editorWidget->setCellBeats(_cellBeats);
    ui->editorWidget->setRows(_rows);
    ui->bottomWidget->setVisible(false);

    connect(ui->verticalScrollBar, &QScrollBar::valueChanged, this, &GanttWidget::verticalScroll);
    connect(ui->horizontalScrollBar, &QScrollBar::valueChanged, this, &GanttWidget::horizontalScroll);
    connect(ui->snapCheckBox, &QCheckBox::clicked, this, &GanttWidget::snapClicked);
    connect(ui->headerWidget, &GanttHeaderWidget::clicked, this, &GanttWidget::headerClicked);
    connect(ui->editorWidget, &GanttEditorWidget::clicked, this, &GanttWidget::editorClicked);
    connect(ui->editorWidget, &GanttEditorWidget::itemChanged, this, &GanttWidget::itemChanged);
    connect(ui->editorWidget, &GanttEditorWidget::horizontalScroll, this, &GanttWidget::wheelHorizontalScroll);
    connect(ui->editorWidget, &GanttEditorWidget::verticalScroll, this, &GanttWidget::wheelVerticalScroll);
    connect(ui->editorWidget, &GanttEditorWidget::itemReleased, this, &GanttWidget::itemReleased);
}

GanttWidget::~GanttWidget()
{
    delete ui;
}

void GanttWidget::setLeftWidget(GanttLeftWidget* const widget)
{
    _leftWidget = widget;
    _leftWidget->setRows(_rows);
    _leftWidget->setRowHeight(_rowHeight);
    layout()->replaceWidget(ui->leftWidget, widget);
}

void GanttWidget::setBottomWidget(GanttBottomWidget* const widget)
{
    _bottomWidget = widget;
    _bottomWidget->setScrollPercentage(ui->headerWidget->getScrollPercentage());
    layout()->replaceWidget(ui->bottomWidget, widget);
}

void GanttWidget::setParameters(int rows, int rowHeight, int cellWidth, float beatsPerCell)
{
    _rows = rows;
    _rowHeight = rowHeight;
    _cellWidth = cellWidth;
    _cellBeats = beatsPerCell;

    ui->headerWidget->setCellWidth(_cellWidth);
    ui->headerWidget->setCellBeats(_cellBeats);

    if (_leftWidget) _leftWidget->setRows(rows);
    if (_leftWidget) _leftWidget->setRowHeight(_rowHeight);

    if (_bottomWidget) _bottomWidget->setCellWidth(_cellWidth);
    if (_bottomWidget) _bottomWidget->setCellBeats(_cellBeats);

    ui->editorWidget->setRows(rows);
    ui->editorWidget->setRowHeight(_rowHeight);
    ui->editorWidget->setCellWidth(_cellWidth);
    ui->editorWidget->setCellBeats(_cellBeats);
}

void GanttWidget::setItems(QList<GanttItem*>* items)
{
    ui->headerWidget->setItems(items);
    ui->editorWidget->setItems(items);
    if (_bottomWidget) _bottomWidget->setItems(items);
}

void GanttWidget::setApplication(Application* app)
{
    _app = app;
    ui->headerWidget->setApplication(_app);
    ui->editorWidget->setApplication(_app);
}

void GanttWidget::invertRows(const bool invert)
{
    ui->editorWidget->invertRows(invert);
}

void GanttWidget::setItemsResizable(const bool resizable)
{
    ui->editorWidget->setItemsResizable(resizable);
}

void GanttWidget::setItemsMovableX(const bool movable)
{
    ui->editorWidget->setItemsMovableX(movable);
}

void GanttWidget::setItemsMovableY(const bool movable)
{
    ui->editorWidget->setItemsMovableY(movable);
}

void GanttWidget::setPositionFunction(std::function<float ()> func)
{
    ui->headerWidget->setPositionFunction(func);
    ui->editorWidget->setPositionFunction(func);
}

void GanttWidget::setHeaderPaintFunction(std::function<void (QPainter&, QRect, float, float, float)> func)
{
    ui->headerWidget->setHeaderPaintFunction(func);
}

void GanttWidget::scrollVertically(const float percentage)
{
    _leftWidget->setScrollPercentage(percentage);
    ui->editorWidget->setVerticalScrollPercentage(percentage);
    ui->horizontalScrollBar->blockSignals(true);
    ui->verticalScrollBar->setValue(percentage * ui->verticalScrollBar->maximum());
    ui->horizontalScrollBar->blockSignals(false);
}

void GanttWidget::verticalScroll(int amount)
{
    float vscroll = (float)amount / ui->verticalScrollBar->maximum();
    _leftWidget->setScrollPercentage(vscroll);
    ui->editorWidget->setVerticalScrollPercentage(vscroll);
}

void GanttWidget::horizontalScroll(int amount)
{
    float hscroll = (float)amount / ui->horizontalScrollBar->maximum();
    ui->headerWidget->setScrollPercentage(hscroll);
    ui->editorWidget->setHorizontalScrollPercentage(hscroll);
}

void GanttWidget::snapClicked()
{
    ui->headerWidget->setSnap(ui->snapCheckBox->isChecked());
    ui->editorWidget->setSnap(ui->snapCheckBox->isChecked());
}

void GanttWidget::wheelHorizontalScroll(int pixels)
{
    ui->headerWidget->scrollBy(pixels);

    if (_bottomWidget)
        dynamic_cast<GanttBottomWidget*>(_bottomWidget)->scrollBy(pixels);

    ui->horizontalScrollBar->blockSignals(true);
    ui->horizontalScrollBar->setValue(ui->headerWidget->getScrollPercentage()*ui->horizontalScrollBar->maximum());
    ui->horizontalScrollBar->blockSignals(false);
}

void GanttWidget::wheelVerticalScroll(int pixels)
{
    if (_leftWidget)
        dynamic_cast<GanttLeftWidget*>(_leftWidget)->scrollBy(pixels);

    ui->verticalScrollBar->blockSignals(true);
    ui->verticalScrollBar->setValue(_leftWidget->getScrollPercentage()*ui->verticalScrollBar->maximum());
    ui->verticalScrollBar->blockSignals(false);
}
