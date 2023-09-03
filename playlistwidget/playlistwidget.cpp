#include "playlistwidget.h"
#include "ui_playlistwidget.h"

PlaylistWidget::PlaylistWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    _app(app),
    ui(new Ui::PlaylistWidget),
    _patternsWidget(new PlaylistPatternsWidget(this))
{
    ui->setupUi(this);
    ui->ganttWidget->setApplication(_app);
    ui->ganttWidget->setLeftWidget(_patternsWidget);
    ui->ganttWidget->setItems(reinterpret_cast<QList<GanttItem*>*>(&app->project().playlist()));
    ui->ganttWidget->setParameters(Rows, RowHeight, CellWidth, 1);
    ui->ganttWidget->setItemsMovableX(true);
    ui->ganttWidget->setPositionFunction([&](){ return _app->playMode() == Application::PlayMode::Song ? _app->position() : 0.0f; });

    connect(ui->ganttWidget, &GanttWidget::clicked, this, &PlaylistWidget::ganttClicked);
    connect(ui->ganttWidget, &GanttWidget::itemsChanged, this, &PlaylistWidget::ganttItemsChanged);
}

PlaylistWidget::~PlaylistWidget()
{
    delete ui;
    delete _patternsWidget;
}

void PlaylistWidget::ganttClicked(Qt::MouseButton button, int row, float time)
{
    if (button == Qt::LeftButton) {
        if (_app->project().getPattern(row).getLength() > 0) {
            _app->project().addPlaylistItem(time, row);
        }
    } else {
        _app->project().removePlaylistItem(time, row);
    }
    update();
}

void PlaylistWidget::ganttItemsChanged()
{
    update();
}
