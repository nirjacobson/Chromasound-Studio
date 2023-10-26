#ifndef PIANOROLLWIDGET_H
#define PIANOROLLWIDGET_H

#include <QMainWindow>
#include <QStyleOption>
#include <QMenu>
#include <QInputDialog>
#include <QMimeData>
#include <QClipboard>

#include "application.h"
#include "pianorollkeyswidget.h"
#include "pianorollvelocitieswidget.h"
#include "commands/addnotecommand.h"
#include "commands/removenotecommand.h"
#include "commands/editnotecommand.h"
#include "commands/addtrackitemscommand.h"
#include "commands/removetrackitemscommand.h"
#include "bson.h"

namespace Ui {
class PianoRollWidget;
}

class PianoRollWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit PianoRollWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~PianoRollWidget();

    static constexpr int Rows = 12 * 8;
    static constexpr int RowHeight = 14;
    static constexpr int CellWidth = 16;

    void setTrack(const int pattern, const int track);

    void pressKey(const int key);
    void releaseKey(const int key);

signals:
    void keyOn(const int key, const int velocity);
    void keyOff(const int key);

private:
    Application* _app;
    Ui::PianoRollWidget *ui;
    PianoRollKeysWidget* _keysWidget;
    PianoRollVelocitiesWidget* _velocitiesWidget;

    Track* _track;
    const GanttItem* _itemLastClicked;

    QMenu _noteMenu;
    QAction _velocityAction;
    Track::Item* _contextItem;

private slots:
    void ganttClicked(Qt::MouseButton button, int row, float time);
    void ganttItemChanged(GanttItem* item, const float toTime, const int toRow, const float toDuration);
    void ganttItemReleased(const GanttItem* item);
    void contextMenuRequested(GanttItem* item, const QPoint& location);
    void velocityTriggered();
    void copy();
    void paste();
    void deleteTriggered();

    // QWidget interface
protected:
    void paintEvent(QPaintEvent* event);
};

#endif // PIANOROLLWIDGET_H
