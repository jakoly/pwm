#include "mainwindow.h"
#include "encrypt.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!Encrypt::init()) {
        QMessageBox::critical(nullptr, "Fehler",
            "Verschluesselungsbibliothek konnte nicht initialisiert werden.");
        return 1;
    }

    MainWindow w;
    if (!w.unlockVault()) {
        return 0; // Nutzer hat den Master-Passwort-Dialog abgebrochen
    }

    w.show();
    return QCoreApplication::exec();
}
