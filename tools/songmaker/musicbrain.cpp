//
// Created by nir on 9/30/26.
//

#include "musicbrain.h"

MusicBrain::Model MusicBrain::Model::cpu() const
{
    const Tensors& _W1 = std::get<0>(*this);
    const Tensors& _b1 = std::get<1>(*this);
    const Tensors& _W = std::get<2>(*this);
    const Tensors& _b = std::get<3>(*this);
    const Tensors& _W2 = std::get<4>(*this);
    const Tensors& _b2 = std::get<5>(*this);
    const Tensors& _classes = std::get<6>(*this);
    const Tensors& _layerSizes = std::get<7>(*this);

    Tensors W1;
    for (const torch::Tensor& t : _W1)
    {
        W1.push_back(t.cpu());
    }

    Tensors b1;
    for (const torch::Tensor& t : _b1)
    {
        b1.push_back(t.cpu());
    }

    Tensors W;
    for (const torch::Tensor& t : _W)
    {
        W.push_back(t.cpu());
    }

    Tensors b;
    for (const torch::Tensor& t : _b)
    {
        b.push_back(t.cpu());
    }

    Tensors W2;
    for (const torch::Tensor& t : _W2)
    {
        W2.push_back(t.cpu());
    }

    Tensors b2;
    for (const torch::Tensor& t : _b2)
    {
        b2.push_back(t.cpu());
    }

    Tensors classes;
    for (const torch::Tensor& t : _classes)
    {
        classes.push_back(t.cpu());
    }

    Tensors layerSizes;
    for (const torch::Tensor& t : _layerSizes)
    {
        layerSizes.push_back(t.cpu());
    }

    return MusicBrain::Model(std::make_tuple(W1, b1, W, b, W2, b2, classes, layerSizes));
}

MusicBrain::Model MusicBrain::Model::cuda() const
{
    const Tensors& _W1 = std::get<0>(*this);
    const Tensors& _b1 = std::get<1>(*this);
    const Tensors& _W = std::get<2>(*this);
    const Tensors& _b = std::get<3>(*this);
    const Tensors& _W2 = std::get<4>(*this);
    const Tensors& _b2 = std::get<5>(*this);
    const Tensors& _classes = std::get<6>(*this);
    const Tensors& _layerSizes = std::get<7>(*this);

    Tensors W1;
    for (const torch::Tensor& t : _W1)
    {
        W1.push_back(t.cuda());
    }

    Tensors b1;
    for (const torch::Tensor& t : _b1)
    {
        b1.push_back(t.cuda());
    }

    Tensors W;
    for (const torch::Tensor& t : _W)
    {
        W.push_back(t.cuda());
    }

    Tensors b;
    for (const torch::Tensor& t : _b)
    {
        b.push_back(t.cuda());
    }

    Tensors W2;
    for (const torch::Tensor& t : _W2)
    {
        W2.push_back(t.cuda());
    }

    Tensors b2;
    for (const torch::Tensor& t : _b2)
    {
        b2.push_back(t.cuda());
    }

    Tensors classes;
    for (const torch::Tensor& t : _classes)
    {
        classes.push_back(t.cuda());
    }

    Tensors layerSizes;
    for (const torch::Tensor& t : _layerSizes)
    {
        layerSizes.push_back(t.cuda());
    }

    return MusicBrain::Model(std::make_tuple(W1, b1, W, b, W2, b2, classes, layerSizes));
}

MusicBrain::Model MusicBrain::Model::mps() const
{
    const Tensors& _W1 = std::get<0>(*this);
    const Tensors& _b1 = std::get<1>(*this);
    const Tensors& _W = std::get<2>(*this);
    const Tensors& _b = std::get<3>(*this);
    const Tensors& _W2 = std::get<4>(*this);
    const Tensors& _b2 = std::get<5>(*this);
    const Tensors& _classes = std::get<6>(*this);
    const Tensors& _layerSizes = std::get<7>(*this);

    Tensors W1;
    for (const torch::Tensor& t : _W1)
    {
        W1.push_back(t.to("mps"));
    }

    Tensors b1;
    for (const torch::Tensor& t : _b1)
    {
        b1.push_back(t.to("mps"));
    }

    Tensors W;
    for (const torch::Tensor& t : _W)
    {
        W.push_back(t.to("mps"));
    }

    Tensors b;
    for (const torch::Tensor& t : _b)
    {
        b.push_back(t.to("mps"));
    }

    Tensors W2;
    for (const torch::Tensor& t : _W2)
    {
        W2.push_back(t.to("mps"));
    }

    Tensors b2;
    for (const torch::Tensor& t : _b2)
    {
        b2.push_back(t.to("mps"));
    }

    Tensors classes;
    for (const torch::Tensor& t : _classes)
    {
        classes.push_back(t.to("mps"));
    }

    Tensors layerSizes;
    for (const torch::Tensor& t : _layerSizes)
    {
        layerSizes.push_back(t.to("mps"));
    }

    return MusicBrain::Model(std::make_tuple(W1, b1, W, b, W2, b2, classes, layerSizes));
}

torch::Tensor MusicBrain::Worker::moved_tensor(const torch::Tensor& t)
{
    switch (_backend)
    {
    case CPU:
        return t.cpu();
    case CUDA:
        return t.cuda();
    case MPS:
        return t.to("mps");
    default:
        return t;
    }
}

MusicBrain::Model MusicBrain::TrainingWorker::train(const torch::Tensor classes, const torch::Tensor& ll_sizes, const int iters, const torch::Tensor &x, const at::Tensor &y)
{
    torch::Tensor Classes = moved_tensor(classes);
    torch::Tensor LlSizes = moved_tensor(ll_sizes);
    torch::Tensor X = moved_tensor(x.to(torch::kFloat32));
    torch::Tensor Y = moved_tensor(y.to(torch::kFloat32));

    std::vector<std::vector<torch::Tensor>> W1;
    std::vector<std::vector<torch::Tensor>> b1;
    std::vector<std::vector<torch::Tensor>> W;
    std::vector<std::vector<torch::Tensor>> b;
    std::vector<std::vector<torch::Tensor>> W2;
    std::vector<std::vector<torch::Tensor>> b2;
    W2.push_back({});
    b2.push_back({});

    std::vector<torch::Tensor> probs = {};
    for (int i = 0; i < classes.size(0); i++) {
        probs.push_back({});
    }

    std::vector<torch::Tensor> loss;

    for (int i = 0; i < iters; i++) {
        std::vector<torch::Tensor> h;
        torch::Tensor h1;
        torch::Tensor h11;
        for (int j = 0; j < ll_sizes.size(0); j++) {
            if (j == 0) {
                if (W1.size() == j) {
                    W1.push_back({});
                }
                if (b1.size() == j) {
                    b1.push_back({});
                }
            } else {
                if (W.size() == j-1) {
                    W.push_back({});
                }
                if (b.size() == j-1) {
                    b.push_back({});
                }
            }
            torch::Tensor xview = X.view({x.size(0), -1});

            for (int k = 0; k < classes.size(0); k++) {
                // Forward pass
                if (j == 0) {
                    if (W1[j].size() == k) {
                        torch::Tensor l_size = torch::tensor(x.sizes()[1] * x.sizes()[2]);
                        l_size = moved_tensor(l_size);
                        torch::Tensor mat = torch::randn({l_size.item().toInt(), ll_sizes[j].item().toInt()});
                        W1[j].push_back(moved_tensor(mat));
                        W1[j][k].set_requires_grad(true);
                    }
                } else {
                    if (W[j-1].size() == k) {
                        torch::Tensor l_size = ll_sizes[j-1];
                        torch::Tensor mat = torch::randn({l_size.item().toInt(), ll_sizes[j].item().toInt()});
                        W[j-1].push_back(moved_tensor(mat));
                        W[j-1][k].set_requires_grad(true);
                    }
                }

                if (j == 0) {
                    if (b1[j].size() == k) {
                        torch::Tensor vec = torch::randn({ll_sizes[j].item().toInt()});
                        b1[j].push_back(moved_tensor(vec));
                        b1[j][k].set_requires_grad(true);
                    }
                } else {
                    if (b[j-1].size() == k) {
                        torch::Tensor vec = torch::randn({ll_sizes[j].item().toInt()});
                        b[j-1].push_back(moved_tensor(vec));
                        b[j-1][k].set_requires_grad(true);
                    }
                }

                if (j == 0) {
                    h1 = xview.matmul(W1[j][k]) + (b1[j][k]);
                    h.push_back(h1);
                } else if (j > 0) {
                    h1 = h[k].matmul(W[j-1][k]) + (b[j-1][k]);
                    h[k] = h1;
                }

                if (j == ll_sizes.size(0)-1) {
                    if (W2[0].size() == k) {
                        torch::Tensor mat = torch::randn({ll_sizes[-1].item().toInt(), classes[k].item().toInt()});
                        W2[0].push_back(moved_tensor(mat));
                        W2[0][k].set_requires_grad(true);
                    }
                    if (b2[0].size() == k) {
                        torch::Tensor vec = torch::randn({classes[k].item().toInt()});
                        b2[0].push_back(moved_tensor(vec));
                        b2[0][k].set_requires_grad(true);
                    }

                    torch::Tensor h2 = tanh(h[k].matmul(W2[0][k]) + b2[0][k]);

                    torch::Tensor counts = h2.exp();
                    probs[k] = (counts / counts.sum(1, true)).view({-1, Classes[k].item().toInt()});
                }
            }
        }

        for (int k = 0; k < Classes.size(0); k++) {
            torch::Tensor y_param = Y.matmul(moved_tensor(torch::nn::functional::one_hot(torch::tensor(k), classes.size(0)).to(torch::kFloat32)));
            std::vector<std::pair<int, int>> pairs;
            for (int l = 0; l < x.size(0); l++) {
                pairs.push_back(std::make_pair(l, y_param.cpu()[l].item().toInt()));
            }
            std::vector<torch::Tensor> _losses;
            for (int l = 0; l < x.size(0); l++) {
                torch::Tensor __loss = -probs[k][pairs[l].first][pairs[l].second].log();
                _losses.push_back(moved_tensor(__loss));
            }
            torch::Tensor _loss = torch::stack(_losses).mean();

            if (loss.size() == k) {
                loss.push_back(_loss);
            } else {
                loss[k] = _loss;
            }
        }
        emit lossUpdate(loss);

        // Backward pass
        for (auto& v : {&(W1[0]), &(b1[0])}) {
            for (int prm = 0; prm < classes.size(0); prm++) {
                if ((*v)[prm].grad().defined()) {
                    (*v)[prm].grad().zero_();
                }
            }
        }
        for (auto& v : {&W, &b}) {
            for (int lyr = 0; lyr < ll_sizes.size(0) - 1; lyr++) {
                for (int prm = 0; prm < classes.size(0); prm++) {
                    if ((*v)[lyr][prm].grad().defined()) {
                        (*v)[lyr][prm].grad().zero_();
                    }
                }
            }
        }
        for (auto& v : {&(W2[0]), &(b2[0])}) {
            for (int prm = 0; prm < classes.size(0); prm++) {
                if ((*v)[prm].grad().defined()) {
                    (*v)[prm].grad().zero_();
                }
            }
        }

        for (int prm = 0; prm < classes.size(0); prm++) {
            loss[prm].backward({}, true);
        }

        for (auto& v : {&(W1[0]), &(b1[0])}) {
            for (int prm = 0; prm < classes.size(0); prm++) {
                if ((*v)[prm].grad().defined()) {
                    (*v)[prm].data() += -0.25 * (*v)[prm].grad();
                }
            }
        }
        for (auto& v : {&W, &b}) {
            for (int lyr = 0; lyr < ll_sizes.size(0) - 1; lyr++) {
                for (int prm = 0; prm < classes.size(0); prm++) {
                    if ((*v)[lyr][prm].grad().defined()) {
                        (*v)[lyr][prm].data() += -0.25 * (*v)[lyr][prm].grad();
                    }
                }
            }
        }
        for (auto& v : {&(W2[0]), &(b2[0])}) {
            for (int prm = 0; prm < classes.size(0); prm++) {
                if ((*v)[prm].grad().defined()) {
                    (*v)[prm].data() += -0.25 * (*v)[prm].grad();
                }
            }
        }
        updateProgress((int)(((float)i/(float)iters) * 100));
    }
    updateProgress(100);
    emit lossUpdate({torch::zeros(5).unbind()});

    torch::Tensor layer_models;
    Tensors parameter_models_W1;
    for (int m = 0; m < classes.size(0); m++) {
        if (layer_models.size(0) == 0) {
            layer_models = torch::unsqueeze(W1[0][m], 0);
        } else {
            layer_models = torch::cat({layer_models, torch::unsqueeze(W1[0][m], 0)});
        }
    }
    parameter_models_W1 = {layer_models};

    layer_models = torch::tensor({});

    Tensors parameter_models_b1;
    for (int m = 0; m < classes.size(0); m++) {
        if (layer_models.size(0) == 0) {
            layer_models = torch::unsqueeze(b1[0][m], 0);
        } else {
            layer_models = torch::cat({layer_models, torch::unsqueeze(b1[0][m], 0)});
        }
    }
    parameter_models_b1 = { layer_models };

    Tensors parameter_models_W;
    for (int l = 0; l < ll_sizes.size(0) - 1; l++) {
        layer_models = torch::tensor({});
        for (int m = 0; m < classes.size(0); m++) {
            if (layer_models.size(0) == 0) {
                layer_models = torch::unsqueeze(W[l][m], 0);
            } else {
                layer_models = torch::cat({layer_models, torch::unsqueeze(W[l][m], 0)});
            }
        }
        if (parameter_models_W.size() == 0) {
            parameter_models_W = { layer_models };
        } else {
            parameter_models_W.push_back(layer_models);
        }
    }

    Tensors parameter_models_b;
    for (int l = 0; l < ll_sizes.size(0) - 1; l++) {
        layer_models = torch::tensor({});
        for (int m = 0; m < classes.size(0); m++) {
            if (layer_models.size(0) == 0) {
                layer_models = torch::unsqueeze(b[l][m], 0);
            } else {
                layer_models = torch::cat({layer_models, torch::unsqueeze(b[l][m], 0)});
            }
        }

        if (parameter_models_b.size() == 0) {
            parameter_models_b = { layer_models };
        } else {
            parameter_models_b.push_back(layer_models);
        }
    }

    layer_models = torch::tensor({});

    Tensors parameter_models_W2;
    for (int m = 0; m < classes.size(0); m++) {
        parameter_models_W2.push_back(W2[0][m]);
    }

    layer_models = torch::tensor({});

    Tensors parameter_models_b2;
    for (int m = 0; m < classes.size(0); m++) {
        parameter_models_b2.push_back(b2[0][m]);
    }

    std::vector<torch::Tensor> classes_vec = { Classes };
    std::vector<torch::Tensor> ll_sizes_vec = { LlSizes };

    MusicBrain::Model m = MusicBrain::Model(std::make_tuple(parameter_models_W1, parameter_models_b1, parameter_models_W, parameter_models_b, parameter_models_W2, parameter_models_b2, classes_vec, ll_sizes_vec));

    return m;
}

std::vector<torch::Tensor> MusicBrain::GenerationWorker::forward(const MusicBrain::Model &model, const at::Tensor &x)
{
    torch::Tensor _x = x.to(torch::kFloat32);
    const torch::Tensor X = moved_tensor(_x);
    const torch::Tensor& classes = std::get<6>(model)[0].cpu();
    int layers = std::get<2>(model).size() + 1;

    torch::Tensor xview = X.view({X.size(0), -1});
    std::vector<torch::Tensor> probs;
    for (int i = 0; i < classes.size(0); i++) {
        torch::Tensor h1;
        torch::Tensor _h1;
        for (int j = 0; j < layers; j++) {
            // Forward pass
            torch::Tensor logits = (j == 0) ? xview : h1;
            if (j == 0) {
                _h1 = torch::tanh(logits.matmul(std::get<0>(model)[j][i]) + std::get<1>(model)[j][i]);
            } else {
                _h1 = torch::tanh(logits.matmul(std::get<2>(model)[j-1][i]) + std::get<3>(model)[j-1][i]);
            }
            h1 = moved_tensor(_h1);

            if (j == layers-1) {
                torch::Tensor h2 = tanh(h1.matmul(std::get<4>(model)[i]) + std::get<5>(model)[i]);
                h2 = moved_tensor(h2);

                torch::Tensor counts = h2.exp();
                if (probs.size() == i) {
                    probs.push_back({});
                }
                probs[i] = (counts / counts.sum(1, true)).view({-1, classes[i].item().toInt()});
                probs[i] = moved_tensor(probs[i]);
            }
        }
    }

    return probs;
}

std::vector<torch::Tensor> MusicBrain::ChordGenerationWorker::generate_chords(const MusicBrain::Model &model, const int bars)
{
    int lowest_root = 100;
    int highest_root = -1;
    std::vector<torch::Tensor> out;
    torch::Tensor context = torch::zeros(Chord::ChordDuration+1).unsqueeze(0);
    for (int i = 0; i < CONTEXT_LENGTH-1; i++) {
        context = torch::cat({context, torch::zeros(Chord::ChordDuration+1).unsqueeze(0)}, 0);
    }
    auto sum = [&out]() {
        return (out.empty()
         ? 0
         : ((out.size() == 1)
                ? out[0][Chord::ChordDuration].item().toInt()
                : torch::stack(out).sum(0)[Chord::ChordDuration].item().toInt()));
    };
    int _sum;
    while ((_sum = sum()) < bars * 1920) {
        updateProgress((int)(((float)_sum/((float)bars * 1920)) * 100));
        // Generate a message
        torch::Tensor msg = torch::zeros(Chord::ChordDuration+1);
        auto probs = forward(model, context.unsqueeze(0));
        for (int i = 0; i < Chord::ChordDuration+1; i++) {
            msg[i] = torch::multinomial(probs[i], 1).item().toInt();
            if (i == Chord::Root) {
                if (msg[i].item().toInt() < lowest_root) {
                    lowest_root = msg[i].item().toInt();
                } else if (msg[i].item().toInt() > highest_root) {
                    highest_root = msg[i].item().toInt();
                }
            }
        }


        msg[Chord::ChordDuration] = ((torch::rand({1}) * 2).item().toInt() * 2 + 2) * 480;
        context = torch::cat({context.slice(0, 1), torch::unsqueeze(msg, 0)});
        msg[Chord::Root] += 36;
        out.push_back(msg);
    }

    std::vector<int> deltas;
    for (torch::Tensor& msg : out) {
        deltas.push_back(msg[Chord::ChordDuration].item().toInt());
    }
    int length = torch::tensor(deltas).sum().item().toInt();
    int rem = length % 1920;

    (*(--out.end()))[Chord::ChordDuration] -= rem;

    updateProgress(100);

    return out;
}

torch::Tensor MusicBrain::PhraseGenerationWorker::generate_phrases(const MusicBrain::Model &model, const int bars)
{
    torch::Tensor out;
    torch::Tensor msg;

    int diff;

    auto sum = [&out]() {
        return ((out.size(0) == 0)
                ? 0
                : ((out.size(0) == 1)
                       ? out[0][Phrase::PhraseDuration].item().toInt()
                       : out.sum(0)[Phrase::PhraseDuration].item().toInt()));
    };

    torch::Tensor context;
    auto clear_context = [&context]() {
        std::vector<torch::Tensor> elems;
        for (int i = 0; i < CONTEXT_LENGTH; i++) {
            elems.push_back(torch::zeros(Phrase::HistDir+1));
        }
        context = torch::stack(elems, 0);
    };

    clear_context();

    while (sum() < bars * 1920) {
        msg = torch::zeros(Phrase::HistDir+1);
        std::vector<torch::Tensor> probs = forward(model, context.unsqueeze(0));
        for (int i = 0; i < 4; i++) {
            int ix = torch::multinomial(probs[i], 1).item().toInt();
            if (i == Phrase::Direction || i == Phrase::HistDir) {
                ix -= 1;
            }

            msg[i] = ix;
        }
        diff = (out.size(0) < PHRASE_HISTORY) ? 0 : (msg[Phrase::FirstNote]  - out[-PHRASE_HISTORY][Phrase::FirstNote]).item().toInt();
        msg[Phrase::HistDir] = (diff == 0) ? 0 : (diff / abs(diff));
        context = torch::cat({context.slice(0, 1), torch::unsqueeze(msg, 0)}, 0);

        if (out.size(0) == 0) {
            out = torch::unsqueeze(msg, 0);
        } else {
            out = torch::cat({out, torch::unsqueeze(msg, 0)});
        }
    }

    int rem = sum() % 1920;
    out[-1][Phrase::PhraseDuration] -= rem;

    return out;
}

torch::Tensor MusicBrain::TimeGenerationWorker::generate_times(const MusicBrain::Model &model, const int bars)
{
    torch::Tensor out;

    torch::Tensor context;
    auto clear_context = [&context]() {
        std::vector<torch::Tensor> elems;
        for (int i = 0; i < CONTEXT_LENGTH; i++) {
            elems.push_back(torch::unsqueeze(torch::tensor(0), 0));
        }
        context = torch::stack(elems);
    };
    clear_context();


    auto sum = [&out]() {
        return ((out.size(0) == 0)
                ? 0
                : ((out.size(0) == 1)
                       ? out[0].item().toInt()
                       : out.sum(0).item().toInt()));
    };

    while (sum() < bars * 1920) {
        std::vector<torch::Tensor> probs = forward(model, context.unsqueeze(0));
        torch::Tensor pprobs = probs[0];
        int ix = qMax(120, ((torch::multinomial(pprobs, 1) / 120).item().toInt() * 120));

        context = torch::cat({context.slice(0, 1), torch::unsqueeze(torch::unsqueeze(torch::tensor(ix), 0), 0)}, 0);

        if (out.size(0) == 0) {
            out = torch::unsqueeze(torch::tensor(ix), 0);
        } else {
            out = torch::cat({out, torch::unsqueeze(torch::tensor(ix), 0)});
        }
    }

    int rem = sum() % 1920;
    out[-1] -= rem;

    return out;
}

at::Tensor MusicBrain::NoteGenerationWorker::generate_notes(const std::vector<std::pair<torch::Tensor, torch::Tensor>> &chords_phrases, const at::Tensor &times)
{
    torch::Tensor times_copy = times.clone();
    torch::Tensor notes;
    torch::Tensor melody_notes;

    int chord_time = 0;

    std::vector<int> last_names = {-1, -1, -1};
    int name = chords_phrases[0].first[Chord::ChordDuration].item().toInt();
    for (const auto& [chord, phrases] : chords_phrases) {
        chord_time = 0;

        std::vector<torch::Tensor> chord_notes;
        chord_notes.push_back(chord[Chord::Root]);
        int add = (chord[Chord::MajorMinor].item().toBool() ? 4 : 3);
        chord_notes.push_back(chord[Chord::Root] + add);
        chord_notes.push_back(chord[Chord::Root] + add + (7 - add));
        std::vector<torch::Tensor> chord_notes_inverted;
        for (int i = chord[Chord::Inversion].item().toInt(); i < 3; i++) {
            chord_notes_inverted.push_back(chord_notes[i]);
        }
        for (int i = 0; i < chord[Chord::Inversion].item().toInt(); i++) {
            torch::Tensor note = chord_notes[i] + 12;

            chord_notes_inverted.push_back(note);
        }
        chord_notes_inverted.push_back(chord_notes_inverted[0] + 12);

        while (chord_time < chord[Chord::ChordDuration].item().toInt()) {
            // qDebug() << "chord_time = " << chord_time << ", chord_duration = " << chord[Chord::ChordDuration].item().toInt();
            int p = 0;
            for (p = 0; p < phrases.size(0); p++) {
                torch::Tensor phrase = phrases[p];
                int phrase_count = 0;
                int direction = phrase[Phrase::Direction].item().toInt();
                // qDebug() << "phrase_count = " << phrase_count << ", phrase[Phrase::Length] = " << phrase[Phrase::Length].item().toInt();
                while (phrase_count < phrase[Phrase::Length].item().toInt()) {
                    if (direction == 0) {
                        direction = std::vector<int>{-1, 1}[(torch::rand(1).item().toInt() * 2)];
                    }
                    name += direction;
                    torch::Tensor _notes = torch::stack(chord_notes_inverted, 0);
                    while (!torch::eq(_notes, name).any().item().toBool() || name == 0 || std::find(last_names.begin(), last_names.end(), name) != last_names.end()) {
                        if (direction == 0) {
                            direction = std::vector<int>{-1, 1}[(torch::rand(1).item().toInt() * 2)];
                        }
                        if (name > _notes.max().item().toInt()) {
                            direction = -1;
                        }
                        if (name < _notes.min().item().toInt()) {
                            direction = 1;
                        }
                        name += direction;
                    }
                    if ((chord_time + times_copy[0].item().toInt()) > chord[Chord::ChordDuration].item().toInt()) {
                        int rem = ((chord_time + times_copy[0]) - chord[Chord::ChordDuration]).item().toInt();
                        if (times_copy.size(0) == 0) {
                            times_copy = torch::unsqueeze(torch::tensor(rem), 0);
                        } else {
                            if (times_copy[0].item().toInt() == rem) {
                                // exactly twice the length of the distance to the end of the chord
                                // keep times_copy[0] as is
                            } else {
                                times_copy[0] -= rem;
                                if (times_copy.size(0) > 1) {
                                    times_copy[1] += rem;
                                }
                            }
                        }
                    }
                    last_names = std::vector<int>(last_names.begin() + 1, last_names.end());
                    last_names.push_back(name);
                    int _name = name;
                    if (melody_notes.size(0) > 0) {
                        while (torch::abs(name - melody_notes[melody_notes.size(0)-1][0]).item().toInt() > 6) {
                            if (_name > melody_notes[melody_notes.size(0)-1][0].item().toInt()) {
                                name -= 12;
                            } else {
                                name += 12;
                            }
                        }
                    }
                    torch::Tensor note = torch::tensor({name, times_copy[0].item().toInt()});
                    chord_time += times_copy[0].item().toInt();
                    // qDebug() << "!!" << chord[Chord::ChordDuration].item().toInt() << ", times_copy[0] = " << times_copy[0].item().toInt();

                    times_copy = times_copy.slice(0, 1);

                    if (melody_notes.size(0) == 0) {
                        melody_notes = torch::unsqueeze(note, 0);
                    } else {
                        melody_notes = torch::cat({melody_notes, torch::unsqueeze(note, 0)});
                    }

                    phrase_count++;

                    if (chord_time >= chord[Chord::ChordDuration].item().toInt()) {
                        break;
                    }
                }

                if (chord_time >= chord[Chord::ChordDuration].item().toInt()) {
                    break;
                }
            }

            if (chord_time >= chord[Chord::ChordDuration].item().toInt()) {
                // qDebug() << "breaking because chord_time = " << chord_time;
                break;
            }
        }
    }

    return melody_notes;
}