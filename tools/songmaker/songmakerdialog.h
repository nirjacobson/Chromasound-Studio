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

#undef slots
#include <torch/torch.h>
#define slots Q_SLOTS

#define CONTEXT_LENGTH 4
#define PHRASE_HISTORY 2

class MainWindow;

typedef std::vector<torch::Tensor> Tensors;
typedef std::tuple<Tensors, Tensors, Tensors, Tensors, Tensors, Tensors, Tensors, Tensors> Model;

namespace Ui {
class SongMakerDialog;
}

enum Chord {
    Root,
    MajorMinor,
    Inversion,
    ChordDuration
};

enum Phrase {
    Direction,
    Length,
    PhraseDuration,
    FirstNote,
    HistDir
};

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

private:
    Ui::SongMakerDialog *ui;
    QMap<QString, QString> _chordsPaths;
    QMap<QString, QString> _melodySetsPaths;
    QMap<QString, QString> _drumkitsPaths;

    Model _chordsModel;
    Model _phrasesModel;
    Model _durationsModel;

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

class TrainingWorker : public QObject {
    Q_OBJECT
public:
    typedef Model Result;
    explicit TrainingWorker(QObject* parent = nullptr) {
        _dialog = dynamic_cast<SongMakerDialog*>(parent);
    }

    void doTrain(const torch::Tensor &classes, const at::Tensor &ll_sizes, const int iters, const torch::Tensor &X, const torch::Tensor &Y);

private:
    SongMakerDialog* _dialog;
    Result _result;

    void updateProgress(const int progress) {
        emit progressUpdated(progress);
    }

    Model train(const torch::Tensor classes, const at::Tensor &ll_sizes, const int iters, const at::Tensor &x, const at::Tensor &y);

signals:
    void lossUpdate(const std::vector<torch::Tensor> losses);

public:

signals:
    void progressUpdated(const int progress);
    void finished(Result result);

};

class ChordGenerationWorker : public QObject {
    Q_OBJECT

    friend class PhraseGenerationWorker;
    friend class TimeGenerationWorker;
public:
    typedef std::vector<torch::Tensor> Result;
    explicit ChordGenerationWorker(QObject* parent = nullptr) {
        _dialog = dynamic_cast<SongMakerDialog*>(parent);
    }

    void doGenerateChords(const Model &model, const int bars) {
        _result = generate_chords(model, bars);

        emit finished(_result);
    }

private:
    SongMakerDialog* _dialog;
    void updateProgress(const int progress) {
        if (progress != _progress) {
            emit progressUpdated(progress);
            _progress = progress;
        }
    }
    int _progress = 0;

    static std::vector<torch::Tensor> forward(const Model &model, const torch::Tensor& x);
    std::vector<torch::Tensor> generate_chords(const Model &model, const int bars);
    torch::Tensor clean_chord(const torch::Tensor& chord);

    Result _result;

public:

signals:
    void progressUpdated(const int progress);
    void finished(Result result);
};

class PhraseGenerationWorker : public QObject {
    Q_OBJECT
public:
    typedef torch::Tensor Result;
    explicit PhraseGenerationWorker(QObject* parent = nullptr) {
        _dialog = dynamic_cast<SongMakerDialog*>(parent);
    }

    void doGeneratePhrases(const Model &model, const int bars) {
        _result = generate_phrases(model, bars);

        emit finished(_result);
    }

private:
    SongMakerDialog* _dialog;
    void updateProgress(const int progress) {
        if (progress != _progress) {
            emit progressUpdated(progress);
            _progress = progress;
        }
    }
    int _progress = 0;

    torch::Tensor generate_phrases(const Model &model, const int bars);

    Result _result;

public:

signals:
    void progressUpdated(const int progress);
    void finished(Result result);
};

class TimeGenerationWorker : public QObject {
    Q_OBJECT
public:
    typedef torch::Tensor Result;
    explicit TimeGenerationWorker(QObject* parent = nullptr) {
        _dialog = dynamic_cast<SongMakerDialog*>(parent);
    }

    void doGenerateTimes(const Model &model, const int bars) {
        _result = generate_times(model, bars);

        emit finished(_result);
    }

private:
    SongMakerDialog* _dialog;
    void updateProgress(const int progress) {
        if (progress != _progress) {
            emit progressUpdated(progress);
            _progress = progress;
        }
    }
    int _progress = 0;

    torch::Tensor generate_times(const Model &model, const int bars);

    Result _result;

public:

signals:
    void progressUpdated(const int progress);
    void finished(Result result);
};

class NoteGenerationWorker : public QObject {
    Q_OBJECT
public:
    typedef torch::Tensor Result;
    explicit NoteGenerationWorker(QObject* parent = nullptr) {
        _dialog = dynamic_cast<SongMakerDialog*>(parent);
    }

    void doGenerateNotes(const std::vector<std::pair<torch::Tensor, torch::Tensor>>& chords_phrases, const torch::Tensor& times) {
        _result = generate_notes(chords_phrases, times);

        emit finished(_result);
    }

private:
    SongMakerDialog* _dialog;
    void updateProgress(const int progress) {
        emit progressUpdated(progress);
    }

    torch::Tensor generate_notes(const std::vector<std::pair<torch::Tensor, torch::Tensor>>& chords_phrases, const torch::Tensor& times);

    Result _result;

public:

signals:
    void progressUpdated(const int progress);
    void finished(Result result);
};

#endif // SONGMAKERDIALOG_H
