#ifndef PIANOROLLWIDGET_H
#define PIANOROLLWIDGET_H

#include <QMainWindow>
#include <QStyleOption>
#include <QMenu>
#include <QInputDialog>
#include <QMimeData>
#include <QClipboard>
#include <QFileDialog>

#include "application.h"
#include "pianorollkeyswidget.h"
#include "pianorollvelocitieswidget.h"
#include "commands/addnotecommand.h"
#include "commands/removenotecommand.h"
#include "commands/editnotecommand.h"
#include "commands/addtrackitemscommand.h"
#include "commands/removetrackitemscommand.h"
#include "commands/composite/replacetrackitemscommand.h"
#include "commands/addtracksettingschangecommand.h"
#include "commands/removetracksettingschangecommand.h"
#include "bson.h"
#include "midi/midi.h"
#include "midi/midifile.h"
#include "mdiarea/mdisubwindow.h"

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
    int channel() const;
    Pattern& pattern() const;

    void pressKey(const int key);
    void releaseKey(const int key);

    bool hasLoop() const;
    float loopStart() const;
    float loopEnd() const;

signals:
    void keyOn(const int key, const int velocity);
    void keyOff(const int key);

private:
    Application* _app;
    Ui::PianoRollWidget *ui;
    PianoRollKeysWidget* _keysWidget;
    PianoRollVelocitiesWidget* _velocitiesWidget;

    int _channel;
    Pattern* _pattern;
    Track* _track;
    const GanttItem* _itemLastClicked;

    QMenu _noteMenu;
    QAction _velocityAction;
    Track::Item* _contextItem;

    Track::SettingsChange* _editingSettingsChange;

    void loadMIDI(const MIDIFile& file);

private slots:
    void ganttMarkerClicked(GanttMarker* marker);
    void ganttHeaderClicked(Qt::MouseButton button, float time);
    void ganttEditorClicked(Qt::MouseButton button, int row, float time);
    void ganttItemChanged(GanttItem* item, const float toTime, const int toRow, const float toDuration);
    void ganttItemReleased(const GanttItem* item);
    void contextMenuRequested(GanttItem* item, const QPoint& location);
    void velocityTriggered();
    void newTriggered();
    void openTriggered();
    void saveTriggered();
    void copy();
    void paste();
    void selectAll();
    void deleteTriggered();
    void doneButtonClicked();
    void removeButtonClicked();

    // QWidget interface
protected:
    void paintEvent(QPaintEvent* event);
    void closeEvent(QCloseEvent* event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
};

#endif // PIANOROLLWIDGET_H
