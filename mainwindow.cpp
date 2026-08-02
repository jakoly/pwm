#include "mainwindow.h"
#include "addpassword.h"
#include "masterpassword.h"
#include "./ui_mainwindow.h"
#include <fstream>
#include <QMessageBox>
#include <QCloseEvent>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Passwoerter werden erst geladen, nachdem der Tresor entsperrt wurde
    // (siehe unlockVault(), das von main.cpp vor show() aufgerufen wird).
}

MainWindow::~MainWindow()
{
    m_encrypt.clearKey();
    delete ui;
}

bool MainWindow::unlockVault()
{
    // Bis zu 5 Versuche, falls das Master-Passwort falsch eingegeben wurde.
    for (int attempt = 0; attempt < 5; ++attempt) {
        MasterPasswordDialog dlg(&m_encrypt, this);
        if (dlg.exec() != QDialog::Accepted || !dlg.isUnlocked()) {
            return false; // Nutzer hat abgebrochen
        }

        // Testen, ob das Passwort tatsaechlich stimmt: falls bereits
        // Eintraege existieren, den ersten probeweise entschluesseln.
        ifstream file("passwords.txt");
        if (file.is_open()) {
            string line;
            if (getline(file, line) && !line.empty()) {
                size_t pos = line.find(',');
                if (pos != string::npos) {
                    string encryptedPassword = line.substr(pos + 1);
                    try {
                        m_encrypt.decryptText(encryptedPassword);
                    } catch (const std::exception&) {
                        m_encrypt.clearKey();
                        QMessageBox::warning(this, "Falsches Passwort",
                            "Das Master-Passwort ist falsch. Bitte erneut versuchen.");
                        continue;
                    }
                }
            }
        }

        loadPasswords();
        return true;
    }

    return false;
}

void MainWindow::loadPasswords() {
    passwords.clear();
    ui->table->setRowCount(0);
    ui->table->setColumnCount(2);
    ui->table->setHorizontalHeaderLabels({"Name", "Password"});

    ifstream file("passwords.txt");
    if (!file.is_open()) {
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue; // leere Zeilen ueberspringen

        size_t pos = line.find(',');
        if (pos != std::string::npos) {
            std::string name = line.substr(0, pos);
            std::string encryptedPassword = line.substr(pos + 1);

            string decrypted;
            try {
                decrypted = m_encrypt.decryptText(encryptedPassword);
            } catch (const std::exception&) {
                // Eintrag konnte nicht entschluesselt werden (z.B. beschaedigt).
                // Ueberspringen, aber App nicht abstuerzen lassen.
                continue;
            }

            passwords.push_back({ name, decrypted });

            int row = ui->table->rowCount();
            ui->table->insertRow(row);
            ui->table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(name)));
            ui->table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(decrypted)));
        }
    }
}

string MainWindow::searchPassword(string searchTerm) {
    for (size_t i = 0; i < passwords.size(); i++) {
        if (passwords[i][0] == searchTerm) {
            cout << "Password found." << endl;
            return passwords[i][1];
        }
    }
    cout << "Password not found." << endl;
    return "";
}

void MainWindow::on_btnNewPassword_clicked()
{
    addPassword newWindow(&passwords, &m_encrypt, this);
    newWindow.exec();
    loadPasswords();
}


void MainWindow::on_pushButton_clicked()
{
    loadPasswords();
}
