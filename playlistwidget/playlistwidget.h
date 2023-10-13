#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>

#include "application.h"
#include "playlistpatternswidget.h"

namespace Ui {
    class PlaylistWidget;
}

class PlaylistWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit PlaylistWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~PlaylistWidget();

        static constexpr int Rows = 100;
        static constexpr int RowHeight = 24;
        static constexpr int CellWidth = 24;

        void setTempo(int bpm);

    private:
        Application* _app;
        Ui::PlaylistWidget *ui;
        PlaylistPatternsWidget* _patternsWidget;

    private slots:
        void ganttHeaderClicked(Qt::MouseButton button, float time);
        void ganttEditorClicked(Qt::MouseButton button, int row, float time);
        void ganttItemsChanged();
};

#endif // PLAYLISTWIDGET_H
