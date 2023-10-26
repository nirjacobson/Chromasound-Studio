#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QMainWindow>
#include <QStyleOption>
#include <QMimeData>
#include <QClipboard>

#include "application.h"
#include "playlistpatternswidget.h"
#include "commands/addplaylistitemcommand.h"
#include "commands/addplaylistitemscommand.h"
#include "commands/removeplaylistitemcommand.h"
#include "commands/removeplaylistitemscommand.h"
#include "commands/editplaylistcommand.h"
#include "bson.h"

namespace Ui {
class PlaylistWidget;
}

class PlaylistWidget : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(QColor loopColor READ loopColor WRITE setLoopColor)

public:
    explicit PlaylistWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~PlaylistWidget();

    static constexpr int Rows = 100;
    static constexpr int RowHeight = 24;
    static constexpr int CellWidth = 24;

    void setTempo(int bpm);

private:
    QColor _loopColor;

    const QColor& loopColor() const;
    void setLoopColor(const QColor& color);

    Application* _app;
    Ui::PlaylistWidget *ui;
    PlaylistPatternsWidget* _patternsWidget;

private slots:
    void ganttHeaderClicked(Qt::MouseButton button, float time);
    void ganttEditorClicked(Qt::MouseButton button, int row, float time);
    void ganttItemChanged(GanttItem* item, const float toTime, const int toRow, const float toDuration);
    void copy();
    void paste();
    void deleteTriggered();

    // QWidget interface
protected:
    void paintEvent(QPaintEvent* event);
};

#endif // PLAYLISTWIDGET_H
