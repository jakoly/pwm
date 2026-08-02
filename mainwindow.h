#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <string>
#include <vector>
#include "encrypt.h"
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    vector<vector<string>> passwords; // {name, entschluesseltes Passwort}

    // Fuehrt den Master-Passwort-Dialog aus. Bei falschem Passwort kann der
    // Nutzer es erneut versuchen. Gibt false zurueck, wenn abgebrochen wurde
    // (die App sollte sich dann beenden).
    bool unlockVault();

private slots:
    void on_btnNewPassword_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    Encrypt m_encrypt;

    void loadPasswords();
    string searchPassword(string searchTerm);
};
#endif // MAINWINDOW_H
