#ifndef PIANOROLLWIDGET_H
#define PIANOROLLWIDGET_H

#include <QWidget>

#include "application.h"
#include "pianorollkeyswidget.h"

namespace Ui {
    class PianoRollWidget;
}

class PianoRollWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit PianoRollWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~PianoRollWidget();

        static constexpr int Rows = 12 * 8;
        static constexpr int RowHeight = 14;
        static constexpr int CellWidth = 16;

        void setTrack(const int pattern, const int track);

    private:
        Application* _app;
        Ui::PianoRollWidget *ui;
        PianoRollKeysWidget* _keysWidget;

        Track* _track;

    private slots:
        void ganttClicked(Qt::MouseButton button, int row, float time);
        void ganttItemsChanged();
};

#endif // PIANOROLLWIDGET_H
