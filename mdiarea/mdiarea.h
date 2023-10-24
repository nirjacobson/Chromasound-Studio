#ifndef MDIAREA_H
#define MDIAREA_H

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QLayout>

class MdiArea : public QMdiArea
{
    Q_OBJECT
    Q_PROPERTY(QString viewMode READ viewModeName WRITE setViewModeName)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor);

public:
    MdiArea(QWidget* parent = nullptr);

signals:
    void viewModeChanged(const QString& viewMode);

private:
    QString viewModeName() const;
    void setViewModeName(const QString& name);

    const QColor& backgroundColor() const;
    void setBackgroundColor(const QColor& color);
};

#endif // MDIAREA_H
