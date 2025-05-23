#include "ganttwidget.h"
#include "ui_ganttwidget.h"
#include "dialogs/pianorollwidget/pianorollvelocitieswidget.h"

GanttWidget::GanttWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::GanttWidget),
    _app(app),
    _leftWidget(nullptr),
    _rows(0),
    _rowHeight(16),
    _cellWidth(16),
    _cellBeats(1),
    _defaultCellWidth(16)
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
    ui->headerWidget->setMarkers(&_markersMap);
    ui->editorWidget->setMarkers(&_markersMap);

    connect(ui->verticalScrollBar, &QScrollBar::valueChanged, this, &GanttWidget::verticalScroll);
    connect(ui->horizontalScrollBar, &QScrollBar::valueChanged, this, &GanttWidget::horizontalScroll);
    connect(ui->snapCheckBox, &QCheckBox::clicked, this, &GanttWidget::snapClicked);
    connect(ui->headerWidget, &GanttHeaderWidget::loopChanged, this, &GanttWidget::loopChanged);
    connect(ui->headerWidget, &GanttHeaderWidget::markerClicked, this, &GanttWidget::markerClicked);
    connect(ui->headerWidget, &GanttHeaderWidget::clicked, this, &GanttWidget::headerClicked);
    connect(ui->editorWidget, &GanttEditorWidget::clicked, this, &GanttWidget::editorClicked);
    connect(ui->editorWidget, &GanttEditorWidget::itemChanged, this, &GanttWidget::itemChanged);
    connect(ui->editorWidget, &GanttEditorWidget::itemReleased, this, &GanttWidget::itemReleased);
    connect(ui->editorWidget, &GanttEditorWidget::horizontalScroll, this, &GanttWidget::wheelHorizontalScroll);
    connect(ui->editorWidget, &GanttEditorWidget::verticalScroll, this, &GanttWidget::wheelVerticalScroll);
    connect(ui->editorWidget, &GanttEditorWidget::contextMenuRequested, this, &GanttWidget::contextMenuRequested);
    connect(ui->scaleWidget, &GanttScaleWidget::scaleChanged, this, &GanttWidget::scaleChanged);
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

void GanttWidget::addBottomWidget(GanttBottomWidget * const widget, const QString& name)
{
    widget->setScrollPercentage(ui->headerWidget->getScrollPercentage());

    _bottomWidgets.append(widget);

    if (_bottomWidgets.size() > 1) {
        if (_bottomWidgets.size() == 2) {
            ui->bottomWidget->removeWidget(ui->bottomWidget->widget(0));
            ui->bottomWidget->insertWidget(0, new QWidget);
            ui->tabWidget->addTab(_bottomWidgets.first(), _bottomWidgetName);
        }
        ui->tabWidget->addTab(widget, name);
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
        ui->bottomWidget->setCurrentIndex(1);
        ui->tabWidget->setCurrentIndex(0);
    } else {
        _bottomWidgetName = name;
        ui->bottomWidget->removeWidget(ui->bottomWidget->widget(0));
        ui->bottomWidget->insertWidget(0, widget);
        ui->bottomWidget->setCurrentIndex(0);
        ui->bottomWidget->setVisible(true);
    }
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

    for (GanttBottomWidget* bw : _bottomWidgets) {
        bw->setCellWidth(_cellWidth);
        bw->setCellBeats(_cellBeats);
    }

    ui->editorWidget->setRows(rows);
    ui->editorWidget->setRowHeight(_rowHeight);
    ui->editorWidget->setCellWidth(_cellWidth);
    ui->editorWidget->setCellBeats(_cellBeats);
}

void GanttWidget::addMarkers(QList<GanttMarker*>* markers)
{
    _markers.append(markers);
}

void GanttWidget::setItems(QList<GanttItem*>* items)
{
    ui->headerWidget->setItems(items);
    ui->editorWidget->setItems(items);
    for (GanttBottomWidget* bw : _bottomWidgets) {
        PianoRollVelocitiesWidget* w;
        if ((w = dynamic_cast<PianoRollVelocitiesWidget*>(bw))) {
            w->setItems(items);
        }
    }
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

const QList<GanttItem*>& GanttWidget::selectedItems() const
{
    return ui->editorWidget->selectedItems();
}

void GanttWidget::selectItems(const QList<GanttItem*>& items)
{
    ui->editorWidget->selectItems(items);
}

void GanttWidget::selectAllItems()
{
    ui->editorWidget->selectAllItems();
}

void GanttWidget::setCellWidth(const int width)
{
    _cellWidth = width;

    ui->headerWidget->setCellWidth(width);
    ui->editorWidget->setCellWidth(width);
    for (GanttBottomWidget* bw : _bottomWidgets) {
        bw->setCellWidth(width);
    }
}

void GanttWidget::setCellMajors(const QList<int>& majors)
{
    ui->editorWidget->setCellMajors(majors);
}

void GanttWidget::setRowMajors(const QList<int>& majors)
{
    ui->editorWidget->setRowMajors(majors);
}

float GanttWidget::mousePosition() const
{
    return ui->editorWidget->mousePosition();
}

bool GanttWidget::hasLoop() const
{
    return ui->headerWidget->hasLoop();
}

float GanttWidget::loopStart() const
{
    return ui->headerWidget->loopStart();
}

float GanttWidget::loopEnd() const
{
    return ui->headerWidget->loopEnd();
}

void GanttWidget::setDefaultCellWidth(int width)
{
    _defaultCellWidth = width;
}

void GanttWidget::doUpdate(bool full)
{
    if (full) {
        ui->headerWidget->setNeedsFullPaint();
        ui->editorWidget->setNeedsFullPaint();

        GanttLeftWidget* leftWidget;
        if ((leftWidget = dynamic_cast<GanttLeftWidget*>(_leftWidget))) {
            leftWidget->setNeedsFullPaint();
        }

        for (GanttBottomWidget* bw : _bottomWidgets) {
            bw->setNeedsFullPaint();
        }
    }
    update();
}

const QColor& GanttWidget::cursorColor() const
{
    return ui->headerWidget->cursorColor();
}

const QColor& GanttWidget::selectionColor() const
{
    return ui->headerWidget->selectionColor();
}

void GanttWidget::setCursorColor(const QColor& color)
{
    ui->headerWidget->setCursorColor(color);
    ui->editorWidget->setCursorColor(color);
}

void GanttWidget::setSelectionColor(const QColor& color)
{
    ui->headerWidget->setSelectionColor(color);
    ui->editorWidget->setSelectionColor(color);
}

void GanttWidget::rebuildMarkersMap()
{
    _markersMap.clear();
    for (const QList<GanttMarker*>* markers : _markers) {
        for (GanttMarker* marker : *markers) {
            _markersMap[marker->time()].append(marker);
        }
    }
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

    for (GanttBottomWidget* bw : _bottomWidgets) {
        bw->setScrollPercentage(hscroll);
    }
}

void GanttWidget::snapClicked()
{
    ui->headerWidget->setSnap(ui->snapCheckBox->isChecked());
    ui->editorWidget->setSnap(ui->snapCheckBox->isChecked());
    for (GanttBottomWidget* bw : _bottomWidgets) {
        bw->setSnap(ui->snapCheckBox->isChecked());
    }
}

void GanttWidget::wheelHorizontalScroll(int pixels)
{
    ui->headerWidget->scrollBy(pixels);

    for (GanttBottomWidget* bw : _bottomWidgets) {
        bw->scrollBy(pixels);
    }

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

void GanttWidget::loopChanged()
{
    ui->editorWidget->setLoop(ui->headerWidget->loopStart(), ui->headerWidget->loopEnd());
}

void GanttWidget::scaleChanged(float scale)
{
    setCellWidth(scale * _defaultCellWidth);
}

void GanttWidget::paintEvent(QPaintEvent* event)
{
    rebuildMarkersMap();
    QWidget::paintEvent(event);
}
