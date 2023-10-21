#include "pianorollwidget.h"
#include "ui_pianorollwidget.h"

PianoRollWidget::PianoRollWidget(QWidget *parent, Application* app)
    : QWidget(parent)
    , _app(app)
    , ui(new Ui::PianoRollWidget)
    , _keysWidget(new PianoRollKeysWidget(this))
    , _velocitiesWidget(new PianoRollVelocitiesWidget(this))
    , _itemLastClicked(nullptr)
{    
    _velocitiesWidget->setApplication(app);

    ui->setupUi(this);

    ui->ganttWidget->setApplication(_app);
    ui->ganttWidget->setLeftWidget(_keysWidget);
    ui->ganttWidget->setBottomWidget(_velocitiesWidget);
    ui->ganttWidget->setParameters(Rows, RowHeight, CellWidth, 0.25);
    ui->ganttWidget->invertRows(true);
    ui->ganttWidget->setItemsResizable(true);
    ui->ganttWidget->setItemsMovableX(true);
    ui->ganttWidget->setItemsMovableY(true);
    ui->ganttWidget->scrollVertically(0.25);
    
    connect(ui->ganttWidget, &GanttWidget::editorClicked, this, &PianoRollWidget::ganttClicked);
    connect(ui->ganttWidget, &GanttWidget::itemChanged, this, &PianoRollWidget::ganttItemChanged);
    connect(ui->ganttWidget, &GanttWidget::itemReleased, this, &PianoRollWidget::ganttItemReleased);

    connect(_keysWidget, &PianoRollKeysWidget::keyOn, this, &PianoRollWidget::keyOn);
    connect(_keysWidget, &PianoRollKeysWidget::keyOff, this, &PianoRollWidget::keyOff);
}

PianoRollWidget::~PianoRollWidget()
{
    delete ui;

    delete _velocitiesWidget;
    delete _keysWidget;
}

void PianoRollWidget::setTrack(const int pattern, const int track)
{
    _track = &_app->project().getPattern(pattern).getTrack(track);
    ui->ganttWidget->setItems(reinterpret_cast<QList<GanttItem*>*>(&_track->items()));
    ui->ganttWidget->setPositionFunction([=](){
        float appPosition = _app->position();

        if (_app->project().playMode() == Project::PlayMode::SONG) {
            QMap<int, float> activePatterns = _app->project().playlist().activePatternsAtTime(appPosition);

            if (activePatterns.contains(pattern)) {
                float delta = appPosition - activePatterns[pattern];
                QList<int> activeTracks = _app->project().patterns()[pattern]->activeTracksAtTime(delta);

                if (activeTracks.contains(track)) {
                    return delta;
                }
            }
        } else {
            return appPosition;
        }

        return -1.0f;
    });
    update();
}

void PianoRollWidget::pressKey(const int key)
{
    _keysWidget->pressKey(key);
}

void PianoRollWidget::releaseKey(const int key)
{
    _keysWidget->releaseKey(key);
}

void PianoRollWidget::ganttClicked(Qt::MouseButton button, int row, float time)
{
    if (button == Qt::LeftButton) {
        _app->undoStack().push(new AddNoteCommand(_app->window(), *_track, time, Note(row, (_itemLastClicked ? _itemLastClicked->duration() : 1))));
    } else {
        _track->removeItem(time, row);
        _app->undoStack().push(new RemoveNoteCommand(_app->window(), *_track, time, Note(row, (_itemLastClicked ? _itemLastClicked->duration() : 1))));
    }
}

void PianoRollWidget::ganttItemChanged(GanttItem* item, const float toTime, const int toRow, const float toDuration)
{
    _app->undoStack().push(new EditNoteCommand(_app->window(), dynamic_cast<Track::Item*>(item), toTime, Note(toRow, toDuration, item->velocity())));
}

void PianoRollWidget::ganttItemReleased(const GanttItem* item)
{
    _itemLastClicked = item;
}

