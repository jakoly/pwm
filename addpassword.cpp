#include "addpassword.h"
#include "ui_addpassword.h"
#include "encrypt.h"
#include <vector>
#include <QTimer>
using namespace std;

addPassword::addPassword(vector<vector<string>> *passwords, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addPassword)
    , m_passwords(passwords)
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

}


void addPassword::on_btnAdd_clicked()
{
    string newName = ui->editName->text().toStdString();
    string newPassword = ui->editPassword->text().toStdString();
    string password = encryptText(newPassword);
    m_passwords->push_back({newName, password});

    ofstream file("passwords.txt", ios::app);
    if (file.is_open()) {
        file << newName << "," << password << endl;
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
