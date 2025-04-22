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
#include "pianorollpitchwidget.h"
#include "commands/addnotecommand.h"
#include "commands/removenotecommand.h"
#include "commands/editnotecommand.h"
#include "commands/addtrackitemscommand.h"
#include "commands/removetrackitemscommand.h"
#include "commands/removetrackpitchchangescommand.h"
#include "commands/composite/replacetrackitemscommand.h"
#include "commands/addtracksettingschangecommand.h"
#include "commands/removetracksettingschangecommand.h"
#include "commands/addpitchchangecommand.h"
#include "commands/removepitchchangecommand.h"
#include "formats/bson.h"
#include "formats/midi/midi.h"
#include "formats/midi/midifile.h"
#include "common/mdiarea/mdisubwindow.h"
#include "pianorollvelocitydialog.h"
#include "pianorollquantizationdivisiondialog.h"

namespace Ui {
    class PianoRollWidget;
}

class PianoRollWidget : public QMainWindow
{
        Q_OBJECT
        Q_PROPERTY(QColor settingsChangeColor READ settingsChangeColor WRITE setSettingsChangeColor)

    public:
        explicit PianoRollWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~PianoRollWidget();

        static constexpr int Rows = 12 * 9;
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

        void doUpdate(const float position, bool full = false);
        void setCellMajors(const QList<int>& majors);

        ChannelSettings& currentSettings();

    signals:
        void keyOn(const int key, const int velocity);
        void keyOff(const int key);
        void sizeUpNeeded();
        void sizeDownNeeded();

    private:
        Application* _app;
        float _appPosition;

        Ui::PianoRollWidget *ui;
        PianoRollKeysWidget* _keysWidget;
        PianoRollVelocitiesWidget* _velocitiesWidget;
        PianoRollPitchWidget* _pitchWidget;

        int _channel;
        Pattern* _pattern;
        Track* _track;
        const GanttItem* _itemLastClicked;

        QMenu _noteMenu;
        QAction _velocityAction;
        Track::Item* _contextItem;

        Track::SettingsChange* _editingSettingsChange;

        PianoRollVelocityDialog* _velocityDialog;

        PianoRollQuantizationDivisionDialog* _quantDivisionDialog;

        void loadMIDI(const MIDIFile& file);

        const QColor& settingsChangeColor() const;
        void setSettingsChangeColor(const QColor& color);

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
        void velocityDialogAccepted();
        void quantizeDivisionTriggered();
        void quantizeKeyOn();
        void quantizeKeyOnAndDuration();
        void pitchChangeAdded(float time, float pitch);
        void pitchChangeRemoved(Track::PitchChange *change);

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
        void closeEvent(QCloseEvent* event);
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);
};

#endif // PIANOROLLWIDGET_H
