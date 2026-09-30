#ifndef MUSICBRAIN_H
#define MUSICBRAIN_H

#include <QObject>

#undef slots
#include <torch/torch.h>
#define slots Q_SLOTS

#define CONTEXT_LENGTH 4
#define PHRASE_HISTORY 2

typedef std::vector<torch::Tensor> Tensors;

namespace MusicBrain
{
    typedef std::tuple<Tensors, Tensors, Tensors, Tensors, Tensors, Tensors, Tensors, Tensors> _Model;

    enum Backend {
        CPU,
        CUDA
    };

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

    class Model : public _Model
    {
    public:
        Model cpu() const;
        Model cuda() const;

        bool operator==(const Model& b) const
        {
            return false;
        }
    };

    class Worker : public QObject
    {
        Q_OBJECT
    public:
        Worker(const Backend backend, QObject* parent = nullptr)
            : QObject(parent)
            , _backend(backend)
        { }
    protected:
        Backend _backend;

        torch::Tensor moved_tensor(const torch::Tensor& t);
    };

    class GenerationWorker : public Worker
    {
        Q_OBJECT
    public:
        GenerationWorker(const Backend backend, QObject* parent = nullptr)
            : Worker(backend, parent)
        { }

        std::vector<torch::Tensor> forward(const Model &model, const torch::Tensor& x);
    };

    class TrainingWorker : public Worker {
        Q_OBJECT
    public:
        typedef Model Result;
        TrainingWorker(const Backend backend, QObject* parent = nullptr)
            : Worker(backend, parent)
        { }

        void doTrain(const torch::Tensor &classes, const at::Tensor &ll_sizes, const int iters, const torch::Tensor &X, const torch::Tensor &Y);

    private:
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

    class ChordGenerationWorker : public GenerationWorker {
        Q_OBJECT

        friend class PhraseGenerationWorker;
        friend class TimeGenerationWorker;
    public:
        ChordGenerationWorker(const Backend backend, QObject* parent = nullptr)
            : GenerationWorker(backend, parent)
        { }

        typedef std::vector<torch::Tensor> Result;

        void doGenerateChords(const Model &model, const int bars) {
            Result result = generate_chords(model, bars);

            emit finished(result);
        }

    private:
        void updateProgress(const int progress) {
            if (progress != _progress) {
                emit progressUpdated(progress);
                _progress = progress;
            }
        }
        int _progress = 0;

        std::vector<torch::Tensor> generate_chords(const Model &model, const int bars);
        torch::Tensor clean_chord(const torch::Tensor& chord);

    public:

    signals:
        void progressUpdated(const int progress);
        void finished(Result result);
    };

    class PhraseGenerationWorker : public GenerationWorker {
        Q_OBJECT
    public:
        PhraseGenerationWorker(const Backend backend, QObject* parent = nullptr)
            : GenerationWorker(backend, parent)
        { }

        typedef torch::Tensor Result;

        void doGeneratePhrases(const Model &model, const int bars) {
            Result result = generate_phrases(model, bars);

            emit finished(result);
        }

    private:
        void updateProgress(const int progress) {
            if (progress != _progress) {
                emit progressUpdated(progress);
                _progress = progress;
            }
        }
        int _progress = 0;

        torch::Tensor generate_phrases(const Model &model, const int bars);

    public:

    signals:
        void progressUpdated(const int progress);
        void finished(Result result);
    };

    class TimeGenerationWorker : public GenerationWorker {
        Q_OBJECT
    public:
        TimeGenerationWorker(const Backend backend, QObject* parent = nullptr)
            : GenerationWorker(backend, parent)
        { }

        typedef torch::Tensor Result;

        void doGenerateTimes(const Model &model, const int bars) {
            Result result = generate_times(model, bars);

            emit finished(result);
        }

    private:
        void updateProgress(const int progress) {
            if (progress != _progress) {
                emit progressUpdated(progress);
                _progress = progress;
            }
        }
        int _progress = 0;

        torch::Tensor generate_times(const Model &model, const int bars);

    public:

    signals:
        void progressUpdated(const int progress);
        void finished(Result result);
    };

    class NoteGenerationWorker : public GenerationWorker {
        Q_OBJECT
    public:
        NoteGenerationWorker(const Backend backend, QObject* parent = nullptr)
            : GenerationWorker(backend, parent)
        { }

        typedef torch::Tensor Result;

        void doGenerateNotes(const std::vector<std::pair<torch::Tensor, torch::Tensor>>& chords_phrases, const torch::Tensor& times) {
            Result result = generate_notes(chords_phrases, times);

            emit finished(result);
        }

    private:
        void updateProgress(const int progress) {
            emit progressUpdated(progress);
        }

        torch::Tensor generate_notes(const std::vector<std::pair<torch::Tensor, torch::Tensor>>& chords_phrases, const torch::Tensor& times);

    public:

    signals:
        void progressUpdated(const int progress);
        void finished(Result result);
    };
}


#endif //MUSICBRAIN_H
