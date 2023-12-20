#include "player.h"
#include "ui_player.h"

Player::Player(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::Player)
    , _app(app)
    , _playlistTableModel(this, _playlist)
    , _currentTrack(-1)
    , _isPlaying(false)
{
    ui->setupUi(this);

    ui->openButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_DirOpenIcon));
    ui->prevButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->playButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_MediaPlay));
    ui->stopButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_MediaStop));
    ui->nextButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_MediaSkipForward));

    ui->playlistTableView->setModel(&_playlistTableModel);
    ui->playlistTableView->setColumnWidth(3, 64);
    ui->playlistTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->playlistTableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    ui->playlistTableView->verticalHeader()->setDefaultSectionSize(16);

    connect(ui->actionOpenFiles, &QAction::triggered, this, &Player::openFiles);
    connect(ui->actionOpenFolder, &QAction::triggered, this, &Player::openFolder);
    connect(ui->actionAddFiles, &QAction::triggered, this, &Player::addFiles);
    connect(ui->actionAddFolder, &QAction::triggered, this, &Player::addFolder);

    connect(ui->openButton, &QToolButton::clicked, this, &Player::openFiles);
    connect(ui->addButton, &QToolButton::clicked, this, &Player::addFiles);

    connect(ui->prevButton, &QToolButton::clicked, this, &Player::prev);
    connect(ui->playButton, &QToolButton::clicked, this, &Player::playPause);
    connect(ui->stopButton, &QToolButton::clicked, this, &Player::stop);
    connect(ui->nextButton, &QToolButton::clicked, this, &Player::next);

    connect(ui->playlistTableView, &QTableView::doubleClicked, this, &Player::itemDoubleClicked);
}

Player::~Player()
{
    delete ui;
}

PlaylistItem Player::getVGMPlaylistItem(const QString& path)
{
    GD3 gd3 = GD3::parseGd3(path);

    QFile file(path);
    file.open(QIODevice::ReadOnly);

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);
    in.skipRawData(0x18);
    quint32 totalSamples;
    in >> totalSamples;
    file.close();

    return PlaylistItem(path, gd3.title(), gd3.author(), gd3.game(), totalSamples / 44100);
}

PlaylistItem Player::getPCMPlaylistItem(const QString& path)
{
    QFile file(path);
    QFileInfo fileInfo(file);

    return PlaylistItem(path, fileInfo.fileName(), "", "", fileInfo.size() / 44100);
}

void Player::openFiles_(const QStringList& files, const bool clear)
{
    if (clear) {
        _playlistTableModel.clear();
    }

    for (const QString& file : files) {
        QString suffix = QFileInfo(file).suffix().toLower();

        if (suffix == "vgm") {
            _playlistTableModel.insertRow(getVGMPlaylistItem(file));
        } else if (suffix == "pcm") {
            _playlistTableModel.insertRow(getPCMPlaylistItem(file));
        }
    }
}

void Player::play(const int index)
{
    QString path = _playlist[index].path();

    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QByteArray vgm = file.readAll();
    file.close();

    _isPlaying = true;
    _currentTrack = index;
    ui->playlistTableView->selectionModel()->setCurrentIndex(_playlistTableModel.index(index, 0), QItemSelectionModel::ClearAndSelect);
    _app->ignoreFMPSGTime(true);
    _app->fmPSG().play(vgm, true, 0, 0);
    ui->playButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_MediaPause));

}

void Player::playPause()
{
    if (_isPlaying) {
        _isPaused = true;
        _isPlaying = false;
        _app->fmPSG().pause();
    } else {
        if (_isPaused) {
            _isPaused = false;
            _isPlaying = true;
            _app->fmPSG().play();
        } else {
            _isPlaying = true;
            play(0);
        }
    }

    ui->playButton->setIcon(ui->playButton->style()->standardIcon(_isPlaying ? QStyle::SP_MediaPause : QStyle::SP_MediaPlay));
}

void Player::stop()
{
    _isPlaying = false;
    _isPaused = false;
    _currentTrack = 0;
    _app->fmPSG().stop();
    ui->playButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_MediaPlay));
}

void Player::prev()
{
    if (_currentTrack > 0) {
        play(--_currentTrack);
    }
}

void Player::next()
{
    if (_currentTrack < _playlist.size() - 1) {
        play(++_currentTrack);
    }
}

void Player::openFiles()
{
    QStringList paths = QFileDialog::getOpenFileNames(this, tr("Open files"), "", "VGM files (*.vgm);;PCM files (*.pcm)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!paths.isEmpty()) {
        openFiles_(paths);
    }
}

void Player::openFolder()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Open folder"), "", QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);

    if (!path.isNull()) {
        QFileInfoList infoList = QDir(path).entryInfoList();
        QStringList paths;
        for (const QFileInfo& fileInfo : infoList) {
            paths.append(fileInfo.absoluteFilePath());
        }
        openFiles_(paths);
    }
}

void Player::addFiles()
{
    QStringList paths = QFileDialog::getOpenFileNames(this, tr("Add files"), "", "VGM files (*.vgm);;PCM files (*.pcm)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!paths.isEmpty()) {
        openFiles_(paths, false);
    }
}

void Player::addFolder()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Add folder"), "", QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);

    if (!path.isNull()) {
        QFileInfoList infoList = QDir(path).entryInfoList();
        QStringList paths;
        for (const QFileInfo& fileInfo : infoList) {
            paths.append(fileInfo.absoluteFilePath());
        }
        openFiles_(paths, false);
    }
}

void Player::itemDoubleClicked(const QModelIndex& index)
{
    play(index.row());
}
