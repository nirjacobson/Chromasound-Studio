#ifndef ALGORITHMWIDGET_H
#define ALGORITHMWIDGET_H

#include <QWidget>
#include <QCommonStyle>

namespace Ui {
    class AlgorithmWidget;
}

class AlgorithmWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit AlgorithmWidget(QWidget *parent = nullptr);
        ~AlgorithmWidget();

        void setAlgorithm(const int alg);
        void setFeedback(const int fb);

    signals:
        void algorithmChanged(const int a);
        void feedbackChanged(const int fb);

    private:
        Ui::AlgorithmWidget *ui;
        int _algorithm;
        int _fb;

    private slots:
        void prevAlgorithm();
        void nextAlgorithm();
        void feedbackDialChanged(const int feedback);
};

#endif // ALGORITHMWIDGET_H
