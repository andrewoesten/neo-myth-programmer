#include "n64romproperties.h"
#include "ui_n64romproperties.h"

#include <neoutil.h>

n64romProperties::n64romProperties(QWidget*parent):QDialog(parent),ui(new Ui::n64romProperties)
{
    ui->setupUi(this);
}

n64romProperties::~n64romProperties()
{
    delete ui;
}

void n64romProperties::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

int n64romProperties::runme()
{
    ui->cic_comboBox->setCurrentIndex(0);
    ui->saveType_comboBox->setCurrentIndex(0);
    ui->cic_lineEdit->setText(QString().sprintf("%i",cic));
    on_cic_lineEdit_textEdited(ui->cic_lineEdit->text());
    ui->saveType_lineEdit->setText(QString().sprintf("%i",saveType));
    on_saveType_lineEdit_textEdited(ui->saveType_lineEdit->text());
    ui->modeA_lineEdit->setText(QString().sprintf("%i",modeA));
    on_modeA_lineEdit_textEdited(ui->modeA_lineEdit->text());
    return exec();
}

void n64romProperties::on_cic_comboBox_currentIndexChanged(int index)
{
    if(index==0)ui->cic_lineEdit->setText("0");
    if(index==1)ui->cic_lineEdit->setText("1");
    if(index==2)ui->cic_lineEdit->setText("2");
    if(index==3)ui->cic_lineEdit->setText("3");
    if(index==4)ui->cic_lineEdit->setText("5");
    if(index==5)ui->cic_lineEdit->setText("6");
}

void n64romProperties::on_cic_lineEdit_textEdited(QString text)
{
    membuf str;
    str.resize(text.length()+16);
    strcpy(str,text.toLocal8Bit().data());
    int err;
    int val=atoiX(str,0,err);
    if(err==0)
    {
        int index=ui->cic_comboBox->currentIndex();
        if(val==0)index=0;
        if(val==1)index=1;
        if(val==2)index=2;
        if(val==3)index=3;
        if(val==5)index=4;
        if(val==6)index=5;
        if(index!=ui->cic_comboBox->currentIndex())
        {
            ui->cic_comboBox->setCurrentIndex(index);
        }
    }
}

void n64romProperties::on_saveType_comboBox_currentIndexChanged(int index)
{
    if(index==0)ui->saveType_lineEdit->setText("0");
    if(index==1)ui->saveType_lineEdit->setText("5");
    if(index==2)ui->saveType_lineEdit->setText("6");
    if(index==3)ui->saveType_lineEdit->setText("1");
    if(index==4)ui->saveType_lineEdit->setText("4");
}

void n64romProperties::on_saveType_lineEdit_textEdited(QString text)
{
    membuf str;
    str.resize(text.length()+16);
    strcpy(str,text.toLocal8Bit().data());
    int err;
    int val=atoiX(str,0,err);
    if(err==0)
    {
        int index=ui->saveType_comboBox->currentIndex();
        if(val==0)index=0;
        if(val==5)index=1;
        if(val==6)index=2;
        if(val==1)index=3;
        if(val==4)index=4;
        if(index!=ui->saveType_comboBox->currentIndex())
        {
            ui->saveType_comboBox->setCurrentIndex(index);
        }
    }
}

void n64romProperties::on_modeA_comboBox_currentIndexChanged(int index)
{
    ui->modeA_lineEdit->setText(QString().sprintf("%i",index));
}

void n64romProperties::on_modeA_lineEdit_textEdited(QString text)
{
    membuf str;
    str.resize(text.length()+16);
    strcpy(str,text.toLocal8Bit().data());
    int err;
    int index=atoiX(str,0,err);
    if(err==0)
    {
        if(index<16&&index!=ui->modeA_comboBox->currentIndex())
        {
            ui->modeA_comboBox->setCurrentIndex(index);
        }
    }
}

void n64romProperties::on_buttonBox_accepted()
{
    int err,val;
    membuf str;
    str.resize(ui->cic_lineEdit->text().length()+16);
    strcpy(str,ui->cic_lineEdit->text().toLocal8Bit().data());
    val=atoiX(str,0,err);
    if(err==0)
    {
        cic=val;
    }
    str.resize(ui->saveType_lineEdit->text().length()+16);
    strcpy(str,ui->saveType_lineEdit->text().toLocal8Bit().data());
    val=atoiX(str,0,err);
    if(err==0)
    {
        saveType=val;
    }
    str.resize(ui->modeA_lineEdit->text().length()+16);
    strcpy(str,ui->modeA_lineEdit->text().toLocal8Bit().data());
    val=atoiX(str,0,err);
    if(err==0)
    {
        modeA=val;
    }
}

void n64romProperties::on_buttonBox_rejected()
{

}
