#include "ganttwidget.h"
#include "ui_ganttwidget.h"

GanttWidget::GanttWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::GanttWidget),
    _app(app),
    _leftWidget(0),
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

    connect(ui->verticalScrollBar, &QScrollBar::valueChanged, this, &GanttWidget::verticalScroll);
    connect(ui->horizontalScrollBar, &QScrollBar::valueChanged, this, &GanttWidget::horizontalScroll);
    connect(ui->snapCheckBox, &QCheckBox::clicked, this, &GanttWidget::snapClicked);
    connect(ui->editorWidget, &GanttEditorWidget::clicked, this, &GanttWidget::clicked);
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

    ui->editorWidget->setRows(rows);
    ui->editorWidget->setRowHeight(_rowHeight);
    ui->editorWidget->setCellWidth(_cellWidth);
    ui->editorWidget->setCellBeats(_cellBeats);
}

void GanttWidget::setItems(QList<GanttItem*>* items)
{
    ui->editorWidget->setItems(items);
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

void GanttWidget::setLength(const float length)
{
    ui->headerWidget->setLength(length);
    ui->editorWidget->setLength(length);
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
    ui->editorWidget->setSnap(ui->snapCheckBox->isChecked());
}
