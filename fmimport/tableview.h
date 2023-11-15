#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>
#include <QKeyEvent>

class TableView : public QTableView
{
    Q_OBJECT

public:
    TableView(QWidget* parent = nullptr);

signals:
    void keyPressed(int key);

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent* event);
};

#endif // TABLEVIEW_H
