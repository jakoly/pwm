#include "mainwindow.h"
#include "addpassword.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loadPasswords();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sleep(int seconds) {
    this_thread::sleep_for(chrono::seconds(seconds));
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
        if (line.empty()) continue; // leere Zeilen überspringen

        size_t pos = line.find(',');
        if (pos != std::string::npos) {
            std::string name = line.substr(0, pos);
            std::string password = line.substr(pos + 1);
            passwords.push_back({ name, password });

            int row = ui->table->rowCount();
            ui->table->insertRow(row);
            ui->table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(name)));
            ui->table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(password)));
        }
    }
}

string MainWindow::searchPassword(string searchTerm) {
    for (int i = 0; i < passwords.size(); i++) {
        if (passwords[i][0] == searchTerm) {
            cout << "Password found: " << passwords[i][1] << endl;
            return passwords[i][1];
        }
    }
    cout << "Password not found." << endl;
    return "";
}

void MainWindow::on_btnNewPassword_clicked()
{
    addPassword newWindow(&passwords, this);
    newWindow.exec();
    loadPasswords();
}


void MainWindow::on_pushButton_clicked()
{
    loadPasswords();
}

