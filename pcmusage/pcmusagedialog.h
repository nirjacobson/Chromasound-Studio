#ifndef PCMUSAGEDIALOG_H
#define PCMUSAGEDIALOG_H

#include <QWidget>
#include <QFileInfo>
#include <QFile>

#include "application.h"
#include "pcmusagedisplayitem.h"

namespace Ui {
class PCMUsageDialog;
}

class PCMUsageDialog : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor color1 READ color1 WRITE setColor1)
    Q_PROPERTY(QColor color2 READ color2 WRITE setColor2)
    Q_PROPERTY(QColor color3 READ color3 WRITE setColor3)
    Q_PROPERTY(QColor color4 READ color4 WRITE setColor4)
    Q_PROPERTY(QColor color5 READ color5 WRITE setColor5)
    Q_PROPERTY(QColor color6 READ color6 WRITE setColor6)
    Q_PROPERTY(QColor color7 READ color7 WRITE setColor7)
    Q_PROPERTY(QColor color8 READ color8 WRITE setColor8)
    Q_PROPERTY(QColor freeSpaceColor READ freeSpaceColor WRITE setFreeSpaceColor)

public:
    static constexpr quint32 MAX_SIZE = 524288;

    explicit PCMUsageDialog(QWidget *parent = nullptr, Application* app = nullptr);
    ~PCMUsageDialog();

    void doUpdate();
private:
    Ui::PCMUsageDialog *ui;
    Application* _app;

    QColor _color1;
    QColor _color2;
    QColor _color3;
    QColor _color4;
    QColor _color5;
    QColor _color6;
    QColor _color7;
    QColor _color8;
    QList<QColor*> _colors;
    QColor _freeSpaceColor;

    QList<PCMUsageDisplayItem> _items;

    const QColor& color1() const;
    const QColor& color2() const;
    const QColor& color3() const;
    const QColor& color4() const;
    const QColor& color5() const;
    const QColor& color6() const;
    const QColor& color7() const;
    const QColor& color8() const;

    const QColor& freeSpaceColor() const;

    void setColor1(const QColor& color);
    void setColor2(const QColor& color);
    void setColor3(const QColor& color);
    void setColor4(const QColor& color);
    void setColor5(const QColor& color);
    void setColor6(const QColor& color);
    void setColor7(const QColor& color);
    void setColor8(const QColor& color);

    void setFreeSpaceColor(const QColor& color);

    QString sizeString(quint32 bytes);
};

#endif // PCMUSAGEDIALOG_H
