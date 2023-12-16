#ifndef FMIMPORTDIALOG_H
#define FMIMPORTDIALOG_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMessageBox>

#include "application.h"
#include "patchtablemodel.h"
#include "bson.h"
#include "tableview.h"
#include "commands/addchannelcommand.h"
#include "commands/setchanneltypecommand.h"
#include "commands/setfmchannelsettingscommand.h"
#include "commands/setchannelnamecommand.h"
#include "commands/composite/addfmchannelcommand.h"
#include "commands/composite/setfmchannelcommand.h"

namespace Ui {
    class FMImportDialog;
}

class FMImportDialog : public QMainWindow
{
        Q_OBJECT

    public:
        explicit FMImportDialog(QWidget *parent = nullptr);
        ~FMImportDialog();

        void setApplication(Application* app);

    private:
        static constexpr quint8 VGM_HEADER_DATA_OFFSET = 0x34;

        Ui::FMImportDialog *ui;
        Application* _app;

        PatchTableModel _tableModel;
        QList<FMChannelSettings> _patchSettings;
        QList<QString> _patchNames;
        QList<QList<int>> _patchOctaves;

        void load(const QString& path);
        void clear();
        int ensurePatch(const FMChannelSettings& settings);

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

#endif // FMIMPORTDIALOG_H
