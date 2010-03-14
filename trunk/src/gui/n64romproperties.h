#ifndef N64ROMPROPERTIES_H
#define N64ROMPROPERTIES_H

#include <QDialog>

namespace Ui {
    class n64romProperties;
}

class n64romProperties : public QDialog {
    Q_OBJECT
public:
    n64romProperties(QWidget *parent = 0);
    ~n64romProperties();
    int cic;
    int saveType;
    int modeA;
    int runme();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::n64romProperties *ui;

private slots:
    void on_modeA_lineEdit_textEdited(QString );
    void on_modeA_comboBox_currentIndexChanged(int index);
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_saveType_lineEdit_textEdited(QString );
    void on_cic_lineEdit_textEdited(QString );
    void on_saveType_comboBox_currentIndexChanged(int index);
    void on_cic_comboBox_currentIndexChanged(int index);
};

#endif // N64ROMPROPERTIES_H
