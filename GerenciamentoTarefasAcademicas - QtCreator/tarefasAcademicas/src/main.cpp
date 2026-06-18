#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // inicia a aplicacao grafica do qt
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
