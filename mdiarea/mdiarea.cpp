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
    if (name == "windows") {
        // cannot dynamically change view mode
        // rely on signal below to cause replacement with a new MdiArea
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

