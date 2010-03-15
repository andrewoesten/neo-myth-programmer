// #########################################################################

#include "neomainwindow.h"
#include "ui_neomainwindow.h"

#include <neoutil.h>
#include <neocart.h>
#include <romdetect.h>

// #########################################################################

neodriver*neodrv=0;
neocontrol*neoctrl=0;
neocart*ncart=0;
BYTE cartid[0x44];
int noErrorMsg;

// #########################################################################

#define open_files 0
#define save_files 1
#define single_filename 2
#define save_sram 4

// #########################################################################

void closedevice()
{
    if(ncart)
    {
        ncart->close();
        delete ncart;
        ncart=0;
    }
    if(neoctrl)
    {
        neoctrl->close();
        delete neoctrl;
        neoctrl=0;
    }
    if(neodrv)
    {
        neodrv->close();
        delete neodrv;
        neodrv=0;
    }
    cartid[0x40]=0;
}

// #########################################################################

int opendevice(neoMainWindow*parent)
{
    int deviceConnected=0;
    if(neoctrl)
    {
        if(neoctrl->isConnected())
        {
            deviceConnected=1;
        }
        else
        {
            neoctrl->close();
            cartid[0x40]=0;
            parent->qtui_MessageBox(QObject::tr("driver check failed"));
        }
    }
    if(deviceConnected)
    {
        return 1;
    }
    int success;
    if(neodrv)
    {
        neodrv->close();
        success=neodrv->open();
    }
    else
    {
        neodrv=getdriver();
        success=neodrv!=0;
    }
    if(!success)
    {
        parent->qtui_MessageBox(QObject::tr("device is not connected"));
        return 0;
    }
    if(neoctrl==0)
    {
        neoctrl=new neocontrol;
    }
    if(neoctrl==0)
    {
        return 0;
    }
    else
    {
        success=neoctrl->open(neodrv);
        neoctrl->getcartid(cartid);
        cartid[0x40]=1;
        if(!success)
        {
            parent->qtui_MessageBox(QObject::tr("device is connected, but not recognized"));
            return 0;
        }
    }
    if(ncart==0)
    {
        ncart=new neocart;
    }
    return 1;
}

// #########################################################################

enum
{
    id_flash,
    offset_flash,
    type_flash,
    filename_flash,
    size_flash,
    sramsize_flash,
    sramoffset_flash,
    flash_column_count
};

// #########################################################################

const char*columnf_str[flash_column_count]=
{
    "id",
    "offset",
    "type",
    "filename",
    "size",
    "sram size",
    "sram offset"
};

// #########################################################################

enum
{
    id_sram,
    filename_sram,
    sramsize_sram,
    sramoffset_sram,
    sram_column_count
};

// #########################################################################

const char*columns_str[sram_column_count]=
{
    "id",
    "filename",
    "sram size",
    "sram offset"
};

// #########################################################################

char*print_size_128KB(DWORD size)
{
    return print_size(Fix(size,128*KB));
}

// #########################################################################

void addRomInfo(QStandardItemModel*model_flash,selEntry*gSelection)
{
    char buffer[64];
    model_flash->insertRow(0);
    model_flash->setData(model_flash->index(0,id_flash),gSelection->id);
    int rt=Min(gSelection->type,6);
    rt=rt==0?md_rom:rt==1?s32x_rom:rt==2?sms_rom:rt==4?vgm_file :rt==5?n64_rom:unknown_rom;
    model_flash->setData(model_flash->index(0,type_flash),romtype_str[rt]);
    model_flash->setData(model_flash->index(0,filename_flash),gSelection->name);
    sprintf(buffer,"0x%.7x",gSelection->offset);
    model_flash->setData(model_flash->index(0,offset_flash),buffer);
    model_flash->setData(model_flash->index(0,size_flash),print_size_128KB(gSelection->length));
    if(gSelection->bsize)
    {
        sprintf(buffer,"0x%.5x",gSelection->bbank*save_block_size);
        model_flash->setData(model_flash->index(0,sramoffset_flash),buffer);
        model_flash->setData(model_flash->index(0,sramsize_flash),print_size(gSelection->bsize*save_block_size));
    }
    else
    {
        model_flash->setData(model_flash->index(0,sramoffset_flash),"");
        if(gSelection->run==5)
        {
            model_flash->setData(model_flash->index(0,sramsize_flash),"eeprom");
        }
        else
        {
            model_flash->setData(model_flash->index(0,sramsize_flash),"");
        }
    }
    QStandardItem*item;
    for(int i=0;i<flash_column_count;i++)
    {
        item=model_flash->item(0,i);
        if(item)
        {
            item->setFlags(item->flags()&(~Qt::ItemIsEditable));
        }
    }
    item=model_flash->item(0,filename_flash);
    if(item)
    {
        item->setFlags(item->flags()|(Qt::ItemIsEditable));
    }
}

// #########################################################################

void addSramInfo(QStandardItemModel*model_sram,selEntry*gSelection)
{
    if(gSelection->bsize)
    {
        char buffer[64];
        model_sram->insertRow(0);
        model_sram->setData(model_sram->index(0,id_sram),gSelection->id);
        model_sram->setData(model_sram->index(0,filename_sram),gSelection->name);
        sprintf(buffer,"0x%.5x",gSelection->bbank*save_block_size);
        model_sram->setData(model_sram->index(0,sramoffset_sram),buffer);
        model_sram->setData(model_sram->index(0,sramsize_sram),print_size(gSelection->bsize*save_block_size));
        QStandardItem*item;
        for(int i=0;i<sram_column_count;i++)
        {
            item=model_sram->item(0,i);
            if(item)
            {
                item->setFlags(item->flags()&(~Qt::ItemIsEditable));
            }
        }
    }
    if(gSelection->type==5&&getn64SramSize(gSelection->n64_saveType)!=0)
    {
        char buffer[64];
        model_sram->insertRow(0);
        model_sram->setData(model_sram->index(0,id_sram),gSelection->id);
        model_sram->setData(model_sram->index(0,filename_sram),gSelection->name);
        sprintf(buffer,"0x%.5x",gSelection->offset==0?0:128*KB);
        model_sram->setData(model_sram->index(0,sramoffset_sram),buffer);
        model_sram->setData(model_sram->index(0,sramsize_sram),print_size(getn64SramSize(gSelection->n64_saveType)));
        QStandardItem*item;
        for(int i=0;i<sram_column_count;i++)
        {
            item=model_sram->item(0,i);
            if(item)
            {
                item->setFlags(item->flags()&(~Qt::ItemIsEditable));
            }
        }
    }
}

// #########################################################################

void qtui_progress_indicator::onUpdate(char*desc,int pr,int rng)
{
    if(desc)
    {
        if(pr!=0)
        {
            emit subTaskComplete(desc);
        }
        else
        {
            emit setText(desc);
        }
    }
    if(prevRange!=rng)
    {
        prevRange=rng;
        emit setRange(0,rng);
    }
    emit setValue(pr);
}

qtui_progress_indicator::qtui_progress_indicator()
{
    prevRange=0;
}

// #########################################################################

void qtui_workerThread::run()
{
    int laction=action;
    action=0;
    if(laction==1)
    {
        ncart->burn();
    }
    if(laction==2)
    {
        ncart->format();
    }
}

qtui_workerThread::qtui_workerThread()
{
    action=0;
}

void qtui_workerThread::burn()
{
    wait();
    action=1;
    interrupted=0;
    start(QThread::HighPriority);
}

void qtui_workerThread::format()
{
    wait();
    action=2;
    interrupted=0;
    start(QThread::HighPriority);
}

qtui_workerThread::~qtui_workerThread()
{
    wait();
}

// #########################################################################

void neoMainWindow::loadFlashItems(int reloadFromFlash)
{
    int flashIndex=ui->flashRomList->selectionModel()->currentIndex().row();
    int sramIndex=ui->sramList->selectionModel()->currentIndex().row();

    ui->freeSpaceLabel->setText("");

    model_flash->clear();
    model_flash->setColumnCount(flash_column_count);
    for(int i=0;i<flash_column_count;i++)
    {
        model_flash->setHeaderData(i,Qt::Horizontal,tr(columnf_str[i]));
    }
    //ui->flashRomList->hideColumn(id_flash);

    model_sram->clear();
    model_sram->setColumnCount(sram_column_count);
    for(int i=0;i<sram_column_count;i++)
    {
        model_sram->setHeaderData(i,Qt::Horizontal,tr(columns_str[i]));
    }
    ui->sramList->hideColumn(id_sram);

    if(reloadFromFlash)
    {
        if(!opendevice(this))
        {
            return;
        }
        ncart->close();
        ncart->open(neoctrl);
    }
    else
    {
        if(!ncart)
        {
            return;
        }
    }

    int dMb=1;
    setoption(displaySzMb,&dMb);

    ui->freeSpaceLabel->setText(tr("Free: ")+QString(print_size_128KB(ncart->getfreespace())));

    selEntry*gSelection;
    for(int i=0;gSelection=ncart->getrominfo(i);i++)
    {
        addRomInfo(model_flash,gSelection);
        addSramInfo(model_sram,gSelection);
    }

    for(int i=0;i<flash_column_count;i++)
    {
        ui->flashRomList->resizeColumnToContents(i);
    }
    //proxyModel_flash->sort(filename_flash,Qt::AscendingOrder);
    for(int i=0;i<sram_column_count;i++)
    {
        ui->sramList->resizeColumnToContents(i);
    }
    //proxyModel_sram->sort(filename_sram,Qt::AscendingOrder);

    flashIndex=Min(flashIndex,ui->flashRomList->model()->rowCount()-1);
    ui->flashRomList->selectionModel()->setCurrentIndex(ui->flashRomList->model()->index(flashIndex,filename_flash),QItemSelectionModel::SelectCurrent|QItemSelectionModel::Rows);
    sramIndex=Min(sramIndex,ui->sramList->model()->rowCount()-1);
    ui->sramList->selectionModel()->setCurrentIndex(ui->sramList->model()->index(sramIndex,filename_sram),QItemSelectionModel::SelectCurrent|QItemSelectionModel::Rows);

    enableActions(1);
}

// #########################################################################

void neoMainWindow::changePage(int index)
{
    if(index>=0&&index<=1)
    {
        ui->viewArea->setCurrentIndex(index);
        ui->commandArea->setCurrentIndex(0);
        if(ui->modeSelector->currentRow()!=index)
        {
            ui->modeSelector->setCurrentRow(index);
        }
        enableActions(1);
    }
    if(index>=2&&index<=2)
    {
        ui->modeSelector->setCurrentRow(-1);
        ui->viewArea->setCurrentIndex(index);
        ui->commandArea->setCurrentIndex(1);
        enableActions(0);
    }
}

// #########################################################################

int neoMainWindow::qtui_MessageBox(const QString&str,int flags)
{
    if(noErrorMsg==0)
    {
        msgBox->setText(str);
        if(flags)
        {
            msgBox->setIcon(QMessageBox::Question);
            msgBox->setStandardButtons(QMessageBox::Yes|QMessageBox::No);
            msgBox->setDefaultButton(QMessageBox::No);
            return (msgBox->exec()==QMessageBox::Yes);
        }
        else
        {
            msgBox->setIcon(QMessageBox::Information);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->setDefaultButton(QMessageBox::Ok);
            msgBox->exec();
        }
    }
    return 0;
}

// #########################################################################

void neoMainWindow::enableActions(int flags)
{
    ui->action_Add_rom->setEnabled(false);
    ui->actionDelete_rom->setEnabled(false);
    ui->actionBackup_rom->setEnabled(false);
    ui->actionReplace_menu->setEnabled(false);
    ui->actionBackup_sram->setEnabled(false);
    ui->actionLoad_sram->setEnabled(false);
    ui->actionInitialize_sram->setEnabled(false);
    ui->actionFormatTypeMD->setEnabled(false);
    ui->actionCartid->setEnabled(false);
    ui->actionProperties->setEnabled(false);
    ui->actionRomProperties->setEnabled(false);

    if(flags==0)
    {
        return;
    }
    int index=ui->viewArea->currentIndex();
    if(index>=0&&index<=1)
    {
        ui->action_Add_rom->setEnabled(ncart!=0);
        ui->actionReplace_menu->setEnabled(ncart!=0);
        ui->actionFormatTypeMD->setEnabled(ncart!=0);
        ui->actionCartid->setEnabled(cartid[0x40]);
        ui->actionProperties->setEnabled(ncart!=0);

        QItemSelectionModel*selectionModel=0;
        if(index==0)
        {
            selectionModel=ui->flashRomList->selectionModel();
        }
        if(index==1)
        {
            selectionModel=ui->sramList->selectionModel();
        }
        if(selectionModel)
        {
            QModelIndexList indexes=selectionModel->selectedRows();
            if(!indexes.isEmpty())
            {
                ui->actionDelete_rom->setEnabled(true);
                ui->actionBackup_rom->setEnabled(true);
                ui->actionBackup_sram->setEnabled(index==1);
                ui->actionLoad_sram->setEnabled(indexes.count()==1&&index==1);
                ui->actionInitialize_sram->setEnabled(indexes.count()==1&&index==1);
                ui->actionRomProperties->setEnabled(true);
            }
        }
    }
}

// #########################################################################

romIdVector neoMainWindow::getSelectedFiles()
{
    romIdVector fileIds;
    QItemSelectionModel*selectionModel=0;
    QSortFilterProxyModel*proxyModel=0;
    QStandardItemModel*model=0;
    int id_index=0;
    if(ui->viewArea->currentIndex()==0)
    {
        selectionModel=ui->flashRomList->selectionModel();
        proxyModel=proxyModel_flash;
        model=model_flash;
        id_index=id_flash;
    }
    if(ui->viewArea->currentIndex()==1)
    {
        selectionModel=ui->sramList->selectionModel();
        proxyModel=proxyModel_sram;
        model=model_sram;
        id_index=id_sram;
    }
    if(selectionModel)
    {
        QModelIndexList indexes=selectionModel->selectedRows();
        QModelIndex index;
        foreach(index,indexes)
        {
            int row=proxyModel->mapToSource(index).row();
            QModelIndex rindex=model->index(row,id_index,QModelIndex());
            fileIds.append(model->data(rindex,Qt::DisplayRole).toInt());
        }
    }
    return fileIds;
}

// #########################################################################

QStringList neoMainWindow::getFileNames(const romIdVector&romIds,int flags)
{
    QFileDialog::Options options;
    QStringList files;
    if(flags&save_files)
    {
        if(romIds.count()<2)
        {
            QString saveFilesPath;
            if(romIds.count()==1)
            {
                selEntry*gSelection=ncart->getrominfo_byid(romIds[0]);
                if(gSelection)
                {
                    saveFilesPath=QString(gSelection->name);
                    if(flags&save_sram)
                    {
                        if(gSelection->type==2)
                        {
                            saveFilesPath+=".ssm";
                        }
                        else if(gSelection->type==5)
                        {
                            if(getn64SramSize(gSelection->n64_saveType)<=st_n64eep16k)
                            {
                                saveFilesPath+=".eep";
                            }
                            else
                            {
                                saveFilesPath+=".sra";
                            }
                        }
                        else
                        {
                            saveFilesPath+=".srm";
                        }
                    }
                    else
                    {
                        if(gSelection->type==0)
                        {
                            saveFilesPath+=".md";
                        }
                        else if(gSelection->type==1)
                        {
                            saveFilesPath+=".32x";
                        }
                        else if(gSelection->type==2)
                        {
                            saveFilesPath+=".sms";
                        }
                        else
                        {
                            saveFilesPath+=".bin";
                        }
                    }
                }
            }
            QString fname=QFileDialog::getSaveFileName(this,tr("Save as"),saveFilesPath,tr("All Files (*);;Text Files (*.txt)"),&selectedFilter,options);
            if(!fname.isEmpty())
            {
                files.append(fname);
            }
        }
        else
        {
            options=QFileDialog::ShowDirsOnly;
            QString directory=QFileDialog::getExistingDirectory(this,tr("Save in"),prevDirectory,options);
            if(!directory.isEmpty())
            {
                foreach(int romid,romIds)
                {
                    selEntry*gSelection=ncart->getrominfo_byid(romid);
                    if(gSelection)
                    {
                        QString filename=directory+dDelim0+QString(gSelection->name);
                        if(flags&save_sram)
                        {
                            if(gSelection->type==2)
                            {
                                filename+=".ssm";
                            }
                            else
                            {
                                filename+=".srm";
                            }
                        }
                        else
                        {
                            if(gSelection->type==0)
                            {
                                filename+=".md";
                            }
                            else if(gSelection->type==1)
                            {
                                filename+=".32x";
                            }
                            else if(gSelection->type==2)
                            {
                                filename+=".sms";
                            }
                            else
                            {
                                filename+=".bin";
                            }
                        }
                        files.append(filename);
                    }
                }
            }
        }
    }
    else
    {
        if(flags&single_filename)
        {
            QString fname=QFileDialog::getOpenFileName(this,tr("Open"),openFilesPath,tr("All Files (*);;Text Files (*.txt)"),&selectedFilter,options);
            if(!fname.isEmpty())
            {
                files.append(fname);
            }
        }
        else
        {
            files=QFileDialog::getOpenFileNames(this,tr("Open"),openFilesPath,tr("All Files (*);;Text Files (*.txt)"),&selectedFilter,options);
        }
    }
    return files;
}

// #########################################################################

neoMainWindow::neoMainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::neoMainWindow)
{
    noErrorMsg=1;
    ui->setupUi(this);
    msgBox=new QMessageBox(this);
    n64p=new n64romProperties(this);

    model_flash=new QStandardItemModel(this);
    proxyModel_flash=new QSortFilterProxyModel;
    proxyModel_flash->setSourceModel(model_flash);
    proxyModel_flash->setDynamicSortFilter(true);
    ui->flashRomList->setModel(proxyModel_flash);
    QObject::connect(ui->flashRomList->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(rom_selectionChanged()));
    QObject::connect(model_flash,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(flash_itemChanged(QStandardItem*)));

    model_sram=new QStandardItemModel(this);
    proxyModel_sram=new QSortFilterProxyModel;
    proxyModel_sram->setSourceModel(model_sram);
    proxyModel_sram->setDynamicSortFilter(true);
    ui->sramList->setModel(proxyModel_sram);
    QObject::connect(ui->sramList->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(rom_selectionChanged()));

    Qt::ConnectionType type=Qt::ConnectionType(Qt::QueuedConnection/*|Qt::UniqueConnection*/);
    QObject::connect(&prgind1, SIGNAL(setText(const QString&)),ui->progressLabel,SLOT(setText(const QString&)),type);
    QObject::connect(&prgind1, SIGNAL(subTaskComplete(const QString&)),this,SLOT(addProgressLogEntry(const QString&)),type);
    QObject::connect(&prgind1, SIGNAL(setValue(int)),ui->progressBar_1,SLOT(setValue(int)),type);
    QObject::connect(&prgind1, SIGNAL(setRange(int,int)),ui->progressBar_1,SLOT(setRange(int,int)),type);
    QObject::connect(&prgind2, SIGNAL(setValue(int)),ui->progressBar_2,SLOT(setValue(int)),type);
    QObject::connect(&prgind2, SIGNAL(setRange(int,int)),ui->progressBar_2,SLOT(setRange(int,int)),type);

    QObject::connect(&workerThread, SIGNAL(finished()),this,SLOT(on_burnCompleted()),type);
    QObject::connect(&workerThread, SIGNAL(terminated()),this,SLOT(on_burnCompleted()),type);

    loadFlashItems(1);

    proxyModel_flash->sort(filename_flash,Qt::AscendingOrder);
    proxyModel_sram->sort(filename_sram,Qt::AscendingOrder);

    ui->flashRomList->addAction(ui->action_Add_rom);
    ui->flashRomList->addAction(ui->actionDelete_rom);
    ui->flashRomList->addAction(ui->actionBackup_rom);
    ui->flashRomList->addAction(ui->actionRomProperties);

    ui->sramList->addAction(ui->actionDelete_rom);
    ui->sramList->addAction(ui->actionBackup_rom);
    ui->sramList->addAction(ui->actionBackup_sram);
    ui->sramList->addAction(ui->actionLoad_sram);
    ui->sramList->addAction(ui->actionInitialize_sram);

    //ui->modeSelector->setStyleSheet("QListWidget { border: none;background-color: #8EDE21;selection-background-color: #999}");

    int o_enableKnownPatches=1;
    setoption(enableKnownPatches,&o_enableKnownPatches);

    changePage(0);
    noErrorMsg=0;
}

// #########################################################################

neoMainWindow::~neoMainWindow()
{
    workerThread.wait();
    closedevice();
    delete ui;
}

// #########################################################################

void neoMainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

// #########################################################################

void neoMainWindow::on_modeSelector_currentRowChanged(int currentRow)
{
    changePage(currentRow);
}

// #########################################################################

void neoMainWindow::on_reloadButton_clicked()
{
    loadFlashItems(1);
}

// #########################################################################

void neoMainWindow::rom_selectionChanged()
{
    enableActions(1);
}

// #########################################################################

void neoMainWindow::flash_itemChanged(QStandardItem*item)
{
    if(ncart)
    {
        if(item->index().column()==filename_flash)
        {
            int row=item->index().row();
            QModelIndex rindex=model_flash->index(row,id_flash,QModelIndex());
            int romid=model_flash->data(rindex,Qt::DisplayRole).toInt();
            selEntry*gSelection=ncart->getrominfo_byid(romid);
            if(gSelection)
            {
                rindex=model_flash->index(row,filename_flash,QModelIndex());
                QString filename=model_flash->data(rindex,Qt::DisplayRole).toString();
                if(!filename.isEmpty())
                {
                    memcpy(gSelection->name,filename.toLocal8Bit().data(),24);
                    gSelection->name[23]=0;
                    if(filename.length()>23)
                    {
                        item->setData(gSelection->name,Qt::DisplayRole);
                    }
                }
            }
        }
    }
}

// #########################################################################

void neoMainWindow::on_burnButton_clicked()
{
    if(!opendevice(this))
    {
        return;
    }
    if(!workerThread.isRunning())
    {
        int n64count=0;
        selEntry*gSelection=0,*gs;
        for(int i=0;gs=ncart->getrominfo(i);i++)
        {
            if(gs->type==5)
            {
                gSelection=gs;
                n64count++;
            }
        }
        if(n64count==1&&gSelection->offset!=0)
        {
            qtui_MessageBox(tr("one n64rom at non-zero offset will not boot.\neither delete \"")+QString(gSelection->name)+tr("\" rom, or add another n64 rom."));
            return;
        }

        enableActions(0);

        ui->pauseButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
        ui->pauseButton->setChecked(false);
        ui->stopButton->setText(tr("Stop"));

        ui->progressLog->clear();

        prgind2.setSubProgress(&prgind1);
        ncart->setProgress(&prgind2);

        workerThread.burn();

        Sleep(10);
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        changePage(2);
    }
}

// #########################################################################

void neoMainWindow::on_actionFormatTypeMD_triggered()
{
    if(qtui_MessageBox(tr("are you sure you want to format cart?"),1))
    {
        if(!opendevice(this))
        {
            return;
        }
        if(!workerThread.isRunning())
        {
            enableActions(0);

            ui->pauseButton->setEnabled(true);
            ui->stopButton->setEnabled(true);
            ui->pauseButton->setChecked(false);
            ui->stopButton->setText(tr("Stop"));

            ui->progressLog->clear();

            prgind2.setSubProgress(&prgind1);
            ncart->setProgress(&prgind2);

            workerThread.format();

            Sleep(10);
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            changePage(2);
        }
    }
}

// #########################################################################

void neoMainWindow::on_burnCompleted()
{
    prgind2.setSubProgress(0);
    ncart->setProgress(0);
    ui->pauseButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->stopButton->setText(tr("Close"));
    if(workerThread.interrupted)
    {
        on_stopButton_clicked();
    }
}

// #########################################################################

void neoMainWindow::on_pauseButton_clicked()
{
    if(workerThread.isRunning())
    {
        prgind2.pause(ui->pauseButton->isChecked());
    }
}

// #########################################################################

void neoMainWindow::on_stopButton_clicked()
{
    if(workerThread.isRunning())
    {
        if(qtui_MessageBox(tr("are you sure you want to stop burn process?"),1))
        {
            workerThread.interrupted=1;
            prgind2.stop();
            ui->pauseButton->setEnabled(false);
            ui->stopButton->setEnabled(false);
        }
        else
        {
            return;
        }
    }
    if(!workerThread.isRunning())
    {
        loadFlashItems(0);
        changePage(0);
    }
}

// #########################################################################

void neoMainWindow::addProgressLogEntry(const QString&text)
{
    ui->progressLog->addItem(text);
    ui->progressLog->selectionModel()->clearSelection();
    ui->progressLog->selectionModel()->setCurrentIndex(ui->progressLog->model()->index(ui->progressLog->model()->rowCount()-1,0),QItemSelectionModel::SelectCurrent|QItemSelectionModel::Rows);
}

// #########################################################################

void neoMainWindow::on_action_Add_rom_triggered()
{
    if(!ncart)
    {
        qtui_MessageBox(tr("cart is not plugged in"));
        return;
    }
    QStringList files=getFileNames(romIdVector(),open_files);
    int fc=files.count();
    if(fc)
    {
        changePage(0);
        int once=1;
        DWORD fs;
        membuf buffer;
        for(int i=0;i<fc;i++)
        {
            membuf fname;
            fname.resize(files[i].length()+16);
            strcpy(fname,files[i].toLocal8Bit().data());
            LoadFile(fname,buffer,fs);
            if(fs==0)
            {
                qtui_MessageBox(tr("cannot load file: ")+QString(fname));
            }
            else
            {
                int romid=ncart->addrom(fname,buffer,fs);
                if(romid<0)
                {
                    if(once)
                    {
                        once=0;
                        qtui_MessageBox(tr("no more space on device: ")+QString(fname));
                    }
                }
                else
                {
                    selEntry*gSelection=ncart->getrominfo_byid(romid);
                    if(gSelection)
                    {
                        addRomInfo(model_flash,gSelection);
                        addSramInfo(model_sram,gSelection);
                        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
                    }
                }
            }
        }
        loadFlashItems(0);
    }
}

// #########################################################################

void neoMainWindow::on_actionDelete_rom_triggered()
{
    if(!ncart)
    {
        qtui_MessageBox(tr("cart is not plugged in"));
        return;
    }
    romIdVector fileIds=getSelectedFiles();
    if(fileIds.count())
    {
        foreach(int romid,fileIds)
        {
            int backupsram=0;
            selEntry*gSelection=ncart->getrominfo_byid(romid);
            if(gSelection)
            {
                if(gSelection->bsize||(gSelection->type==5&&getn64SramSize(gSelection->n64_saveType)!=0))
                {
                    if(qtui_MessageBox(tr("do you want to backup sram for ")+QString(gSelection->name)+QString("?"),1))
                    {
                        backupsram=1;
                    }
                }
            }
            if(backupsram)
            {
                romIdVector fileIds;
                fileIds.append(romid);
                QStringList files=getFileNames(fileIds,save_files|save_sram);
                if(files.count())
                {
                    membuf fname;
                    fname.resize(files[0].length()+16);
                    strcpy(fname,files[0].toLocal8Bit().data());
                    ncart->backupsram(fileIds[0],fname);
                }
            }
            ncart->delrom(romid);
        }
        loadFlashItems(0);
    }
}

// #########################################################################

void neoMainWindow::on_actionBackup_rom_triggered()
{
    if(!ncart)
    {
        qtui_MessageBox(tr("cart is not plugged in"));
        return;
    }
    romIdVector fileIds=getSelectedFiles();
    int fc=fileIds.count();
    if(fc)
    {
        QStringList files=getFileNames(fileIds,save_files);
        if(files.count())
        {
            membuf fname;
            for(int i=0;i<fc;i++)
            {
                fname.resize(files[i].length()+16);
                strcpy(fname,files[i].toLocal8Bit().data());
                ncart->backuprom(fileIds[i],fname);
            }
        }
    }
}

// #########################################################################

void neoMainWindow::on_actionReplace_menu_triggered()
{
    if(!ncart)
    {
        qtui_MessageBox(tr("cart is not plugged in"));
        return;
    }
    QStringList files=getFileNames(romIdVector(),open_files|single_filename);
    if(files.count())
    {
        DWORD fs;
        membuf buffer;
        membuf fname;
        fname.resize(files[0].length()+16);
        strcpy(fname,files[0].toLocal8Bit().data());
        LoadFile(fname,buffer,fs);
        if(fs==0)
        {
            qtui_MessageBox(tr("cannot load file: ")+QString(fname));
        }
        else
        {
            ncart->replace_menu(buffer,fs);
        }
    }
}

// #########################################################################

void neoMainWindow::on_actionBackup_sram_triggered()
{
    if(!ncart)
    {
        qtui_MessageBox(tr("cart is not plugged in"));
        return;
    }
    romIdVector fileIds=getSelectedFiles();
    int fc=fileIds.count();
    if(fc)
    {
        QStringList files=getFileNames(fileIds,save_files|save_sram);
        if(files.count())
        {
            membuf fname;
            for(int i=0;i<fc;i++)
            {
                fname.resize(files[i].length()+16);
                strcpy(fname,files[i].toLocal8Bit().data());
                ncart->backupsram(fileIds[i],fname);
            }
        }
    }
}

// #########################################################################

void neoMainWindow::on_actionLoad_sram_triggered()
{
    if(!ncart)
    {
        qtui_MessageBox(tr("cart is not plugged in"));
        return;
    }
    romIdVector fileIds=getSelectedFiles();
    if(fileIds.count()==1)
    {
        QStringList files=getFileNames(fileIds,open_files|single_filename);
        if(files.count())
        {
            DWORD fs;
            membuf buffer;
            membuf fname;
            fname.resize(files[0].length()+16);
            strcpy(fname,files[0].toLocal8Bit().data());
            LoadFile(fname,buffer,fs);
            if(fs==0)
            {
                qtui_MessageBox(tr("cannot load file: ")+QString(fname));
            }
            else
            {
                if(!ncart->linksram(fileIds[0],buffer,fs))
                {
                    qtui_MessageBox(tr("not enough sram"));
                }
            }
        }
    }
}

// #########################################################################

void neoMainWindow::on_actionInitialize_sram_triggered()
{
    if(!ncart)
    {
        qtui_MessageBox(tr("cart is not plugged in"));
        return;
    }
    romIdVector fileIds=getSelectedFiles();
    if(fileIds.count()==1)
    {
        if(qtui_MessageBox(tr("are you sure you want to initialize sram?"),1))
        {
            foreach(int romid,fileIds)
            {
                ncart->initsram(romid);
            }
        }
    }
}

// #########################################################################

void neoMainWindow::on_actionCartid_triggered()
{
    if(!ncart)
    {
        qtui_MessageBox(tr("cart is not plugged in"));
        return;
    }
    if(cartid[0x40])
    {
        QStringList files=getFileNames(romIdVector(),save_files);
        if(files.count())
        {
            membuf fname;
            fname.resize(files[0].length()+16);
            strcpy(fname,files[0].toLocal8Bit().data());
            SaveFile(fname,(char*)cartid,0x40);
        }
    }
}

// #########################################################################

void neoMainWindow::on_actionProperties_triggered()
{
}

// #########################################################################

void neoMainWindow::on_actionRomProperties_triggered()
{
    if(!ncart)
    {
        qtui_MessageBox(tr("cart is not plugged in"));
        return;
    }
    romIdVector fileIds=getSelectedFiles();
    //if(fileIds.count()==1)
    {
        foreach(int romid,fileIds)
        {
            selEntry*gSelection=ncart->getrominfo_byid(romid);
            if(gSelection)
            {
                if(gSelection->type==5)
                {
                    n64p->setWindowTitle(gSelection->name);
                    n64p->cic=gSelection->n64_cic;
                    n64p->saveType=gSelection->n64_saveType;
                    n64p->modeA=gSelection->n64_modeA;
                    if(n64p->runme())
                    {
                        gSelection->n64_cic=n64p->cic;
                        gSelection->n64_saveType=n64p->saveType;
                        gSelection->n64_modeA=n64p->modeA;
                    }
                }
            }
        }
        loadFlashItems(0);
    }
}
