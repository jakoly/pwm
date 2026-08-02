#ifndef ADDPASSWORD_H
#define ADDPASSWORD_H

#include <QDialog>
#include <fstream>
#include <iostream>
#include <vector>
#include "encrypt.h"
using namespace std;

namespace Ui {
class addPassword;
}

class addPassword : public QDialog
{
    Q_OBJECT

public:
    explicit addPassword(vector<vector<string>> *passwords, Encrypt *encrypt, QWidget *parent = nullptr);
    ~addPassword();

private slots:
    void on_btnCancel_clicked();

    void on_btnAdd_clicked();

private:
    Ui::addPassword *ui;
    vector<vector<string>> *m_passwords;
    Encrypt *m_encrypt;
};

#endif // ADDPASSWORD_H
