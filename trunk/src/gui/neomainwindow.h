// #########################################################################

#ifndef NEOMAINWINDOW_H
#define NEOMAINWINDOW_H

// #########################################################################

#include <QMainWindow>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QFileDialog>
#include <QThread>

#include <neoutil.h>
#include "n64romproperties.h"

// #########################################################################

namespace Ui
{
    class neoMainWindow;
}

// #########################################################################

typedef QVector<int> romIdVector;

// #########################################################################

class qtui_progress_indicator:public QObject,public progress_indicator
{
    Q_OBJECT
private:
    int prevRange;
    virtual void onUpdate(char*desc,int pr,int rng);
public:
    qtui_progress_indicator();
signals:
    void setText(const QString&);
    void subTaskComplete(const QString&);
    void setRange(int,int);
    void setValue(int);
};

// #########################################################################

class qtui_workerThread:public QThread
{
    Q_OBJECT
private:
    int action;
    virtual void run();
public:
    int interrupted;
    qtui_workerThread();
    void burn();
    void format();
    virtual ~qtui_workerThread();
};

// #########################################################################

class neoMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    neoMainWindow(QWidget*parent=0);
    ~neoMainWindow();
    int qtui_MessageBox(const QString&str,int flags=0);

protected:
    void changeEvent(QEvent*e);

private:
    Ui::neoMainWindow*ui;
    QStandardItemModel*model_flash;
    QSortFilterProxyModel*proxyModel_flash;
    QStandardItemModel*model_sram;
    QSortFilterProxyModel*proxyModel_sram;
    QString selectedFilter,openFilesPath;
    QString prevDirectory;

    QMessageBox*msgBox;
    qtui_progress_indicator prgind1;
    qtui_progress_indicator prgind2;
    qtui_workerThread workerThread;

    n64romProperties*n64p;

    void loadFlashItems(int reloadFromFlash);
    void changePage(int index);
    void enableActions(int flags);
    romIdVector getSelectedFiles();
    QStringList getFileNames(const romIdVector&romIds,int flags);

private slots:

    void on_actionRomProperties_triggered();
    void on_actionProperties_triggered();
    void on_actionCartid_triggered();
    void flash_itemChanged(QStandardItem*item);
    void on_actionFormatTypeMD_triggered();
    void addProgressLogEntry(const QString&text);
    void on_actionInitialize_sram_triggered();
    void on_actionLoad_sram_triggered();
    void on_actionBackup_sram_triggered();
    void rom_selectionChanged();
    void on_burnCompleted();
    void on_actionDelete_rom_triggered();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void on_burnButton_clicked();
    void on_reloadButton_clicked();
    void on_modeSelector_currentRowChanged(int currentRow);
    void on_action_Add_rom_triggered();
    void on_actionBackup_rom_triggered();
    void on_actionReplace_menu_triggered();
};

// #########################################################################

#endif // NEOMAINWINDOW_H
