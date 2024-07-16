#ifndef OPNIMPORTDIALOG_H
#define OPNIMPORTDIALOG_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMessageBox>
#include <QRegularExpression>

#include "application.h"
#include "patchtablemodel.h"
#include "formats/bson.h"
#include "tableview.h"
#include "commands/addchannelcommand.h"
#include "commands/setchanneltypecommand.h"
#include "commands/setfmchannelsettingscommand.h"
#include "commands/setchannelnamecommand.h"
#include "commands/composite/addfmchannelcommand.h"
#include "commands/composite/setfmchannelcommand.h"

namespace Ui {
    class OPNImportDialog;
}

class OPNImportDialog : public QMainWindow
{
        Q_OBJECT

    public:
        explicit OPNImportDialog(QWidget *parent = nullptr);
        ~OPNImportDialog();

        void setApplication(Application* app);

    private:
        static constexpr quint8 VGM_HEADER_DATA_OFFSET = 0x34;

        Ui::OPNImportDialog *ui;
        Application* _app;

        PatchTableModel _tableModel;
        QList<FMChannelSettings> _patchSettings;
        QList<QString> _patchNames;
        QList<QList<int>> _patchOctaves;

        void load(const QString& path);
        void loadVGM(const QString& path);
        void loadOPM(const QString& path);
        void clear();
        int ensurePatch(const FMChannelSettings& settings, const QString& name = QString());

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

#endif // OPNIMPORTDIALOG_H
