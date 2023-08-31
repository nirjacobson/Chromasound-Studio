#include "pianorollwidget.h"
#include "ui_pianorollwidget.h"

PianoRollWidget::PianoRollWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    _app(app),
    ui(new Ui::PianoRollWidget),
    _keysWidget(new PianoRollKeysWidget(this))
{
    ui->setupUi(this);

    ui->ganttWidget->setApplication(_app);
    ui->ganttWidget->setLeftWidget(_keysWidget);
    ui->ganttWidget->setParameters(Rows, RowHeight, CellWidth, 0.25);
    ui->ganttWidget->invertRows(true);

    connect(ui->ganttWidget, &GanttWidget::clicked, this, &PianoRollWidget::ganttClicked);
}

PianoRollWidget::~PianoRollWidget()
{
    delete ui;
}

void PianoRollWidget::setTrack(const int pattern, const int track)
{
    _track = &_app->project().getPattern(pattern).getTrack(track);
    ui->ganttWidget->setItems(reinterpret_cast<QList<GanttItem*>*>(&_track->items()));
    update();
}

void PianoRollWidget::ganttClicked(Qt::MouseButton button, int row, float time)
{
    if (button == Qt::LeftButton) {
        _track->addItem(time, Note(row, 1));
    } else {
        _track->removeItem(time, row);
    }
    ui->ganttWidget->setLength(_track->length());
    update();
}
