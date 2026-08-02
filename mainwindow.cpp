#include "mainwindow.h"
#include "addpassword.h"
#include "./ui_mainwindow.h"

vector<vector<string>> passwords;

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
    string line;
    int row = 0;
    while (getline(file, line)) {
        size_t pos = line.find(',');
        if (pos != std::string::npos) {
            std::string name = line.substr(0, pos);
            std::string password = line.substr(pos + 1);
            passwords.push_back({ name, password });
            ui->table->insertRow(row);
            ui->table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(name)));
            ui->table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(password)));
            cout << "- " << name << ", " << password << endl;
        }
        row++;
    }
}

int MainWindow::newPassword() {
    system("cls");
    cout << "Passwordmanager" << endl << endl;
    cout << "Name: ";
    string newName = "";
    getline(cin, newName);
    cout << "New Password: ";
    string newPassword = "";
    getline(cin, newPassword);
    passwords.push_back({newName, newPassword});

    ofstream file("passwords.txt", ios::app);
    if (file.is_open()) {
        file << "\n" << newName << "," << newPassword << endl;
        file.close();
    }
    else {
        cout << "Unable to open file for writing." << endl;
        return 1;
    }

    cout << "Password added successfully!" << endl;
    return 0;
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
    addPassword newWindow(this);
    newWindow.exec();
}

