#include "tableview.h"

TableView::TableView(QWidget* parent)
    : QTableView(parent)
{

}

void TableView::keyPressEvent(QKeyEvent* event)
{
    emit keyPressed(event->key());
}
