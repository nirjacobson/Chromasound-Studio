#include "playlistwidget.h"
#include "ui_playlistwidget.h"

PlaylistWidget::PlaylistWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    _app(app),
    ui(new Ui::PlaylistWidget),
    _patternsWidget(new PlaylistPatternsWidget(this, app))
{
    ui->setupUi(this);
    ui->ganttWidget->setApplication(_app);
    ui->ganttWidget->setLeftWidget(_patternsWidget);
    ui->ganttWidget->setItems(reinterpret_cast<QList<GanttItem*>*>(&app->project().playlist()));
    ui->ganttWidget->setParameters(Rows, RowHeight, CellWidth, 1);
    ui->ganttWidget->setItemsMovableX(true);
    ui->ganttWidget->setPositionFunction([&](){ return _app->project().playMode() == Project::PlayMode::SONG ? _app->position() : 0.0f; });
    ui->ganttWidget->setHeaderPaintFunction([&](QPainter& painter, QRect rect, float leftPosition, float rightPosition, float beatsPerPixel) {
        if (_app->project().playlist().doesLoop()) {
            float loopOffset = _app->project().playlist().loopOffset();

            if (leftPosition <= loopOffset && loopOffset <= rightPosition) {
                int loopOffsetPixel = (loopOffset - leftPosition) / beatsPerPixel;

                QRect loopRect = QRect(QPoint(loopOffsetPixel, 0), QSize(rect.height(), rect.height()));
                painter.setPen(QColor(LoopColor).darker());
                painter.setBrush(QColor(LoopColor));

                QPoint textPoint = loopRect.bottomLeft() + QPoint(4, -4);
                painter.fillRect(loopRect, painter.brush());
                painter.drawRect(loopRect);
                painter.drawText(textPoint, "L");
            }
        }
    });

    connect(ui->ganttWidget, &GanttWidget::headerClicked, this, &PlaylistWidget::ganttHeaderClicked);
    connect(ui->ganttWidget, &GanttWidget::editorClicked, this, &PlaylistWidget::ganttEditorClicked);
    connect(ui->ganttWidget, &GanttWidget::itemsChanged, this, &PlaylistWidget::ganttItemsChanged);
}

PlaylistWidget::~PlaylistWidget()
{
    delete ui;
    delete _patternsWidget;
}

void PlaylistWidget::ganttHeaderClicked(Qt::MouseButton button, float time)
{
    if (button == Qt::RightButton) {
        if (time == _app->project().playlist().loopOffset()) {
            _app->project().playlist().setLoopOffset(-1);
        } else {
            _app->project().playlist().setLoopOffset(time);
        }
        update();
    }
}

void PlaylistWidget::ganttEditorClicked(Qt::MouseButton button, int row, float time)
{
    if (button == Qt::LeftButton) {
        if (_app->project().getPattern(row).getLength() > 0) {
            _app->project().playlist().addItem(time, row);
        }
    } else {
        _app->project().playlist().removeItem(time, row);
    }
    update();
}

void PlaylistWidget::ganttItemsChanged()
{
    update();
}
