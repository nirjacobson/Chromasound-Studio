#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QList>
#include <QFileDialog>
#include <QTimer>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

#include "application.h"
#include "playlistitem.h"
#include "playlisttablemodel.h"
#include "formats/gd3.h"

namespace Ui {
    class Player;
}

class Player : public QMainWindow
{
        Q_OBJECT

    public:
        explicit Player(QWidget *parent = nullptr, Application* app = nullptr);
        ~Player();

    private:
        Ui::Player *ui;
        Application* _app;

        QList<PlaylistItem> _playlist;
        PlaylistTableModel _playlistTableModel;

        int _currentTrack;
        bool _isPlaying;
        bool _isPaused;

        QTimer _timer;

        PlaylistItem getVGMPlaylistItem(const QString& path);
        PlaylistItem getPCMPlaylistItem(const QString& path);

        void openFiles_(const QStringList& files, const bool clear = true);
        void play(const int index);

        void playPause();
        void stop();
        void prev();
        void next();

        QByteArray pcmToVgm(const QString& path);

    private slots:
        void openFiles();
        void openFolder();
        void addFiles();
        void addFolder();

        void itemDoubleClicked(const QModelIndex& index);
        void frame();

        void chromasoundStopped();

        // QWidget interface
    protected:
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);
};

#endif // PLAYER_H
