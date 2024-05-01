#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QMainWindow>
#include <QStyleOption>
#include <QMimeData>
#include <QClipboard>
#include <QMenu>

#include "application.h"
#include "playlistpatternswidget.h"
#include "commands/addplaylistitemcommand.h"
#include "commands/addplaylistitemscommand.h"
#include "commands/removeplaylistitemcommand.h"
#include "commands/removeplaylistitemscommand.h"
#include "commands/editplaylistcommand.h"
#include "commands/addplaylistlfochangecommand.h"
#include "commands/addplaylistnoisefrequencychangecommand.h"
#include "commands/addplaylistenvelopefrequencychangecommand.h"
#include "commands/addplaylistenvelopeshapechangecommand.h"
#include "commands/addplaylistusertonechangecommand.h"
#include "commands/removeplaylistlfochangecommand.h"
#include "commands/removeplaylistnoisefrequencychangecommand.h"
#include "commands/removeplaylistenvelopefrequencychangecommand.h"
#include "commands/removeplaylistenvelopeshapechangecommand.h"
#include "commands/removeplaylistusertonechangecommand.h"
#include "formats/bson.h"
#include "common/mdiarea/mdisubwindow.h"

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

        static constexpr int Rows = 99;
        static constexpr int RowHeight = 24;
        static constexpr int CellWidth = 24;

        bool hasLoop() const;
        float loopStart() const;
        float loopEnd() const;

        void doUpdate(const float position);
        void setCellMajors(const QList<int>& majors);
    signals:
        void patternClicked(const int pattern);

    private:
        QColor _loopColor;

        const QColor& loopColor() const;
        void setLoopColor(const QColor& color);

        Application* _app;
        float _appPosition;

        Ui::PlaylistWidget *ui;
        PlaylistPatternsWidget* _patternsWidget;

        Playlist::LFOChange* _editingLFOChange;
        Playlist::NoiseFrequencyChange* _editingNoiseFrequencyChange;
        Playlist::EnvelopeFrequencyChange* _editingEnvelopeFrequencyChange;
        Playlist::EnvelopeShapeChange* _editingEnvelopeShapeChange;
        Playlist::UserToneChange* _editingUserToneChange;

        QMenu _markerMenu;
        QAction _lfoChangeAction;
        QAction _noiseFreqChangeAction;
        QAction _envFreqChangeAction;
        QAction _envShapeChangeAction;
        QAction _userToneAction;
        float _markerMenuTime;

    private slots:
        void ganttMarkerClicked(GanttMarker* marker);
        void ganttHeaderClicked(Qt::MouseButton button, float time, QPoint location);
        void ganttEditorClicked(Qt::MouseButton button, int row, float time);
        void ganttItemChanged(GanttItem* item, const float toTime, const int toRow, const float toDuration);
        void copy();
        void paste();
        void selectAll();
        void deleteTriggered();
        void doneButtonClicked();
        void removeButtonClicked();

        void lfoChangeTriggered();
        void noiseFreqChangeTriggered();
        void envFreqChangeTriggered();
        void envShapeChangeTriggered();
        void userToneChangeTriggered();

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
        void closeEvent(QCloseEvent* event);
};

#endif // PLAYLISTWIDGET_H
