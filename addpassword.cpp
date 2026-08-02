#include "addpassword.h"
#include "ui_addpassword.h"
#include "encrypt.h"
#include <vector>
#include <QTimer>
#include <QMessageBox>
using namespace std;

addPassword::addPassword(vector<vector<string>> *passwords, Encrypt *encrypt, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addPassword)
    , m_passwords(passwords)
    , m_encrypt(encrypt)
{
    ui->setupUi(this);
    ui->btnAdd->setEnabled(false);

    auto updateButtonState = [this]() {
        ui->btnAdd->setEnabled(!ui->editName->text().isEmpty() &&
                               !ui->editPassword->text().isEmpty());
    };
    connect(ui->editName, &QLineEdit::textChanged, this, updateButtonState);
    connect(ui->editPassword, &QLineEdit::textChanged, this, updateButtonState);
}

addPassword::~addPassword()
{
    delete ui;
}

void addPassword::on_btnCancel_clicked()
{
    reject();
}


void addPassword::on_btnAdd_clicked()
{
    string newName = ui->editName->text().toStdString();
    string newPassword = ui->editPassword->text().toStdString();

    string encryptedPassword;
    try {
        encryptedPassword = m_encrypt->encryptText(newPassword);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Fehler", "Verschluesselung fehlgeschlagen.");
        return;
    }

    m_passwords->push_back({newName, newPassword}); // Klartext im Speicher fuer die Anzeige

    ofstream file("passwords.txt", ios::app);
    if (file.is_open()) {
        file << newName << "," << encryptedPassword << endl;
        file.close();
    }

    ui->btnAdd->setText("Done!");
    ui->btnAdd->setEnabled(false);

    QTimer::singleShot(2000, this, [this]() {
        ui->editName->clear();
        ui->editPassword->clear();
        ui->btnAdd->setText("Add");
    });
}
