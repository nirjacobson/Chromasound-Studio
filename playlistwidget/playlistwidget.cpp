#include "playlistwidget.h"
#include "ui_playlistwidget.h"

PlaylistWidget::PlaylistWidget(QWidget *parent, Application* app) :
    QMainWindow(parent),
    _app(app),
    ui(new Ui::PlaylistWidget),
    _patternsWidget(new PlaylistPatternsWidget(this, app)),
    _loopColor(128, 192, 224)
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
                painter.setPen(QColor(_loopColor).darker());
                painter.setBrush(QColor(_loopColor));

                QPoint textPoint = loopRect.bottomLeft() + QPoint(4, -4);
                painter.fillRect(loopRect, painter.brush());
                painter.drawRect(loopRect);
                painter.drawText(textPoint, "L");
            }
        }
    });

    connect(ui->ganttWidget, &GanttWidget::headerClicked, this, &PlaylistWidget::ganttHeaderClicked);
    connect(ui->ganttWidget, &GanttWidget::editorClicked, this, &PlaylistWidget::ganttEditorClicked);
    connect(ui->ganttWidget, &GanttWidget::itemChanged, this, &PlaylistWidget::ganttItemChanged);

    connect(ui->actionCopy, &QAction::triggered, this, &PlaylistWidget::copy);
    connect(ui->actionPaste, &QAction::triggered, this, &PlaylistWidget::paste);
    connect(ui->actionSelectAll, &QAction::triggered, this, &PlaylistWidget::selectAll);
    connect(ui->actionDelete, &QAction::triggered, this, &PlaylistWidget::deleteTriggered);

    ui->actionCopy->setShortcuts(QKeySequence::Copy);
    ui->actionPaste->setShortcuts(QKeySequence::Paste);
    ui->actionSelectAll->setShortcuts(QKeySequence::SelectAll);
    ui->actionDelete->setShortcuts(QKeySequence::Delete);
}

PlaylistWidget::~PlaylistWidget()
{
    delete ui;
    delete _patternsWidget;
}

const QColor& PlaylistWidget::loopColor() const
{
    return _loopColor;
}

void PlaylistWidget::setLoopColor(const QColor& color)
{
    _loopColor = color;
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
            _app->undoStack().push(new AddPlaylistItemCommand(_app->window(), _app->project().playlist(), time, row));
        }
    } else {
        _app->undoStack().push(new RemovePlaylistItemCommand(_app->window(), _app->project().playlist(), time, row));
    }
}

void PlaylistWidget::ganttItemChanged(GanttItem* item, const float toTime, const int toRow, const float toDuration)
{
    _app->undoStack().push(new EditPlaylistCommand(_app->window(), dynamic_cast<Playlist::Item*>(item), toTime, reinterpret_cast<const QList<Playlist::Item*>&>(ui->ganttWidget->selectedItems())));
}

void PlaylistWidget::copy()
{
    bson_writer_t* writer;

    uint8_t* buf = nullptr;
    size_t buflen = 0;
    bson_t* doc;

    writer = bson_writer_new(&buf, &buflen, 0, bson_realloc_ctx, NULL);

    bson_writer_begin(writer, &doc);

    bson_t items;

    uint32_t i = 0;

    BSON_APPEND_ARRAY_BEGIN(doc, "playlistItems", &items);
    for (const GanttItem* const item : ui->ganttWidget->selectedItems()) {
        const Playlist::Item* playlistItem = dynamic_cast<const Playlist::Item*>(item);

        bson_t b_item;
        bson_init(&b_item);
        BSON::fromPlaylistItem(&b_item, playlistItem);

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i++, &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&items, key, &b_item);
    }
    bson_append_array_end(doc, &items);

    bson_writer_end(writer);
    bson_writer_destroy(writer);

    QMimeData* mimeData = new QMimeData;
    mimeData->setData("application/bson", QByteArray((const char*)buf, buflen));

    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setMimeData(mimeData);
}

void PlaylistWidget::paste()
{
    QClipboard* clipboard = QGuiApplication::clipboard();

    const QMimeData* mimeData = clipboard->mimeData();
    if (!mimeData->hasFormat("application/bson")) {
        return;
    }

    QByteArray data = mimeData->data("application/bson");

    const uint8_t* cdata = reinterpret_cast<const uint8_t*>(data.constData());

    bson_reader_t* reader;
    const bson_t* b;

    reader = bson_reader_new_from_data(cdata, data.size());
    b = bson_reader_read(reader, NULL);

    bson_iter_t iter;
    bson_iter_init(&iter, b);

    bson_iter_t items;
    bson_iter_t child;
    bson_iter_t item;

    QList<Playlist::Item*> pastedItems;

    if (bson_iter_find_descendant(&iter, "playlistItems", &items) && BSON_ITER_HOLDS_ARRAY(&items) && bson_iter_recurse(&items, &child)) {
        while (bson_iter_next(&child)) {
            bson_iter_recurse(&child, &item);
            Playlist::Item* playlistItem = new Playlist::Item(BSON::toPlaylistItem(item, &_app->project()));
            pastedItems.append(playlistItem);
        }
    }

    bson_reader_destroy(reader);

    _app->undoStack().push(new AddPlaylistItemsCommand(_app->window(), _app->project().playlist(), ui->ganttWidget->mousePosition(), pastedItems));

    ui->ganttWidget->selectItems(reinterpret_cast<QList<GanttItem*>&>(pastedItems));
    ui->ganttWidget->update();

}

void PlaylistWidget::selectAll()
{
    ui->ganttWidget->selectAllItems();
}

void PlaylistWidget::deleteTriggered()
{
    _app->undoStack().push(new RemovePlaylistItemsCommand(_app->window(), _app->project().playlist(), reinterpret_cast<const QList<Playlist::Item*>&>(ui->ganttWidget->selectedItems())));
}

void PlaylistWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

