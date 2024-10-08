#ifndef PRDISPLAYWIDGET_H
#define PRDISPLAYWIDGET_H

#include <QPainter>

#include "application.h"
#include "common/damagewidget/damagewidget.h"

class PRDisplayWidget : public DamageWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
        Q_PROPERTY(QColor cursorColor READ cursorColor WRITE setCursorColor)
        Q_PROPERTY(QColor itemColor READ itemColor WRITE setItemColor)

    public:
        explicit PRDisplayWidget(QWidget *parent = nullptr, Application* app = nullptr, int index = 0);

        void setApplication(Application* app);
        void setIndex(const int i);

        void doUpdate(const float position, const bool full);

    signals:
        void pianoRollTriggered();

    protected:
        void mousePressEvent(QMouseEvent* event);

    private:
        QColor _borderColor = QColor(128,128,128);
        QColor _backgroundColor = QColor(255,255,255);
        QColor _cursorColor = QColor(64, 192, 64);
        QColor _itemColor = QColor(128, 128, 255);

        const QColor& borderColor() const;
        const QColor& backgroundColor() const;
        const QColor& cursorColor() const;
        const QColor& itemColor() const;

        void setBorderColor(const QColor& color);
        void setBackgroundColor(const QColor& color);
        void setCursorColor(const QColor& color);
        void setItemColor(const QColor& color);

        Application* _app;
        float _appPosition;

        int _index;

        QPair<int, int> range() const;

        // DamageWidget interface
    private:
        void paintFull(QPaintEvent* event);
        void paintPartial(QPaintEvent* event);
};

#endif // PRDISPLAYWIDGET_H
