#include <QtGui/QApplication>
#include "neomainwindow.h"
#include <plugins.h>

int main(int argc, char *argv[])
{
    init_plugins();
    QApplication a(argc, argv);
    neoMainWindow w;
    w.show();
    return a.exec();
}
