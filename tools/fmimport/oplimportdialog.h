#ifndef OPLIMPORTDIALOG_H
#define OPLIMPORTDIALOG_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMessageBox>

#include "application.h"
#include "patchtablemodel.h"
#include "formats/bson.h"
#include "tableview.h"

#include "commands/editoplsettingscommand.h"

namespace Ui {
    class OPLImportDialog;
}

class OPLImportDialog : public QMainWindow
{
        Q_OBJECT

    public:
        explicit OPLImportDialog(QWidget *parent = nullptr);
        ~OPLImportDialog();

        void setApplication(Application* app);

    private:
        static constexpr quint8 VGM_HEADER_DATA_OFFSET = 0x34;

        Ui::OPLImportDialog *ui;
        Application* _app;

        PatchTableModel _tableModel;
        QList<OPLSettings> _patchSettings;
        QList<QString> _patchNames;
        QList<QList<int>> _patchOctaves;

        void load(const QString& path);
        void clear();
        int ensurePatch(const OPLSettings& settings);

    private slots:
        void openTriggered();
        void saveTriggered();
        void saveAllTriggered();
        void keyPressedOnTable(int key);
        void sendTriggered();

        // QWidget interface
    protected:
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);
};

#endif // OPLIMPORTDIALOG_H
