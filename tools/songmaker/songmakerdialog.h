#ifndef SONGMAKERDIALOG_H
#define SONGMAKERDIALOG_H

#include <QMainWindow>
#include <QDirListing>
#include <QFile>
#include <QFileInfo>
#include <QThread>
#include <QDirListing>

#include <filesystem>
#include <functional>

#include "formats/midi/midifile.h"
#include "formats/opm.h"
#include "formats/bson.h"

#include "application.h"
#include "formats/bson.h"
#include "toplevel/channelswidget/channelswidget.h"

#include "commands/addchannelcommand.h"
#include "commands/setchanneltypecommand.h"
#include "commands/setfmchannelsettingscommand.h"
#include "commands/composite/setpcmchannelcommand.h"

#include "musicbrain.h"

class MainWindow;

namespace Ui {
class SongMakerDialog;
}

class SongMakerDialog : public QMainWindow
{
    Q_OBJECT

    friend class TrainingWorker;
    friend class ChordGenerationWorker;
    friend class ChordCleaningWorker;

public:
    explicit SongMakerDialog(QWidget *parent = nullptr);
    ~SongMakerDialog();

    MainWindow* _mainWindow;

private slots:
    void chordsSelectionChanged(const int index);
    void okButtonClicked();
    void random();
    void openModels();
    void saveModels();
    void clearModels();
    void lossUpdated(std::vector<torch::Tensor> losses);
    void cpuSelected();
    void cudaSelected();

private:
    Ui::SongMakerDialog *ui;
    QMap<QString, QString> _chordsPaths;
    QMap<QString, QString> _melodySetsPaths;
    QMap<QString, QString> _drumkitsPaths;

    MusicBrain::Model _chordsModel;
    MusicBrain::Model _phrasesModel;
    MusicBrain::Model _durationsModel;

    MusicBrain::Backend _backend;
    QList<QIcon> _backendIcons;

    bool _retrain;

    std::pair<torch::Tensor, torch::Tensor> build_chords_dataset(const std::string& path);
    std::pair<torch::Tensor, torch::Tensor> build_phrases_dataset(const std::string& path);
    std::pair<torch::Tensor, torch::Tensor> build_times_dataset(const std::string& path);

    std::vector<std::pair<torch::Tensor, torch::Tensor>> phrases_by_chord(const std::vector<torch::Tensor>& chords, const torch::Tensor& phrases);

    void load(const QString& path);

    void disable_fields();
    void enable_fields();

    void models_to_bson(bson_t *dst);
    void models_from_bson(bson_t merged_bson);

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // SONGMAKERDIALOG_H
