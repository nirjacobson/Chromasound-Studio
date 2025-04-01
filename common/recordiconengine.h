#ifndef RECORDICONENGINE_H
#define RECORDICONENGINE_H

#include <QIconEngine>
#include <QPainter>
#include <QPushButton>

class RecordIconEngine : public QIconEngine
{
public:
    RecordIconEngine();

    // QIconEngine interface
public:
    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);
    QIconEngine *clone() const;
};

#endif // RECORDICONENGINE_H
