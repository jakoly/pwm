#ifndef ADDPASSWORD_H
#define ADDPASSWORD_H

#include <QDialog>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

namespace Ui {
class addPassword;
}

class addPassword : public QDialog
{
    Q_OBJECT

public:
    explicit addPassword(vector<vector<string>> *passwords, QWidget *parent = nullptr);
    ~addPassword();

private slots:
    void on_btnCancel_clicked();

    void on_btnAdd_clicked();

private:
    Ui::addPassword *ui;
    vector<vector<string>> *m_passwords;
};

#endif // ADDPASSWORD_H
