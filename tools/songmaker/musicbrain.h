#ifndef MUSICBRAIN_H
#define MUSICBRAIN_H

#include <QThread>

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
        CUDA,
        MPS
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
        Model mps() const;

        bool operator==(const Model& b) const
        {
            return false;
        }
    };

    struct TrainingArgs
    {
        torch::Tensor classes;
        torch::Tensor ll_sizes;
        int trainingIters;
        torch::Tensor X;
        torch::Tensor Y;
    };

    struct GeneratingArgs
    {
        Model* model;
        int bars;
    };

    struct GeneratingNotesArgs
    {
        std::vector<std::pair<torch::Tensor, torch::Tensor>> chords_phrases;
        torch::Tensor times;
    };

    class Worker : public QThread
    {
        Q_OBJECT
    public:
        Worker(const Backend backend, QObject* parent = nullptr)
            : QThread(parent)
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

        void doTrain(const TrainingArgs& args)
        {
            _classes = &args.classes;
            _ll_sizes = &args.ll_sizes;
            _iters = args.trainingIters;
            _X = &args.X;
            _Y = &args.Y;

            start();
        }

    protected:
        void run() override
        {
            MusicBrain::Model result = train(*_classes, *_ll_sizes, _iters, *_X, *_Y);
            emit finished(result);
        }

    private:
        const torch::Tensor* _classes;
        const torch::Tensor* _ll_sizes;
        int _iters;
        const torch::Tensor* _X;
        const torch::Tensor* _Y;

        void updateProgress(const int progress) {
            emit progressUpdated(progress);
        }

        Model train(const torch::Tensor classes, const at::Tensor &ll_sizes, const int iters, const at::Tensor &x, const at::Tensor &y);

    signals:
        void lossUpdate(const std::vector<torch::Tensor> losses);
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

        void doGenerateChords(const GeneratingArgs& args) {
            _model = args.model;
            _bars = args.bars;

            start();
        }

    protected:
        void run() override
        {
            Result result = generate_chords(*_model, _bars);

            emit finished(result);
        }

    private:
        const Model* _model;
        int _bars;

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

        void doGeneratePhrases(const GeneratingArgs& args) {
            _model = args.model;
            _bars = args.bars;

            start();
        }

    protected:
        void run() override
        {
            Result result = generate_phrases(*_model, _bars);

            emit finished(result);
        }

    private:
        const Model* _model;
        int _bars;

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

        void doGenerateTimes(const GeneratingArgs& args) {
            _model = args.model;
            _bars = args.bars;

            start();
        }

    protected:
        void run() override
        {
            Result result = generate_times(*_model, _bars);

            emit finished(result);
        }

    private:
        const Model* _model;
        int _bars;

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

        void doGenerateNotes(const GeneratingNotesArgs& args) {
            _chords_phrases = &args.chords_phrases;
            _times = &args.times;

            start();
        }

    protected:
        void run() override
        {
            Result result = generate_notes(*_chords_phrases, *_times);

            emit finished(result);
        }

    private:
        const std::vector<std::pair<torch::Tensor, torch::Tensor>>* _chords_phrases;
        const torch::Tensor* _times;

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
