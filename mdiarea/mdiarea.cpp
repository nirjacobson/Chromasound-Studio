#include "mdiarea.h"

MdiArea::MdiArea(QWidget* parent)
    : QMdiArea(parent)
{

}

QString MdiArea::viewModeName() const
{
    switch (viewMode()) {
        case QMdiArea::SubWindowView:
            return "windows";
        case QMdiArea::TabbedView:
            return "tabs";
    }

    return QString();
}

void MdiArea::setViewModeName(const QString& name)
{
    closeAllSubWindows();

    if (name == "windows") {

    } else if (name == "tabs") {
        setViewMode(QMdiArea::TabbedView);
        setTabsClosable(true);
        setTabsMovable(true);
    }

    emit viewModeChanged(name);
}

const QColor& MdiArea::backgroundColor() const
{
    return background().color();
}

void MdiArea::setBackgroundColor(const QColor& color)
{
    setBackground(QBrush(color));
}

void MdiArea::resetBackgroundColor()
{
    setBackground(QBrush(Qt::gray));
}
