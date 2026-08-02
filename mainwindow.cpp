#include "mainwindow.h"
#include "./ui_mainwindow.h"

vector<vector<string>> passwords;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    cout << "Passwordmanager" << endl << endl;
    bool mPasswordCorrect = false;
    while (!mPasswordCorrect) {
        cout << "Masterpassword: ";
        string mpasswordInput = "";
        cin >> mpasswordInput;
        size_t hashValueMP = hasher(mpasswordInput);
        if (hashValueMP == mPassword) {
            mPasswordCorrect = true;
            cout << "Access granted" << endl;
            cout << "Please wait until Passwords are loaded" << endl;
            sleep(1);
        } else {
            cout << "Incorrect Masterpassword. Please try again." << endl;
        }
    }

    system("cls");
    cout << "Passwordmanager" << endl << endl;
    cout << "Passwords: " << endl;

    string choice;
    while (true) {
        getline(cin, choice);
        if (choice == "N") {
            newPassword();
        }
        if (choice == "Q") {
            cout << "Exiting program..." << endl;
            break;
        }
        if (choice.length() >= 3) {
            string result = searchPassword(choice);
            system("cls");
            cout << "Search for '" << choice << "':" << endl;
            cout << "Search result: " << result << endl;
        }
        else {
            loadPasswords();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void sleep(int seconds) {
    this_thread::sleep_for(chrono::seconds(seconds));
}

void loadPasswords() {
    system("cls");
    cout << "Passwordmanager" << endl << endl;
    cout << "Passwords: " << endl;

    ifstream file("passwords.txt");
    string line;
    while (getline(file, line)) {
        size_t pos = line.find(',');
        if (pos != std::string::npos) {
            std::string name = line.substr(0, pos);
            std::string password = line.substr(pos + 1);
            passwords.push_back({ name, password });
            cout << "- " << name << ", " << password << endl;
        }
    }
}

int newPassword() {
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

string searchPassword(string searchTerm) {
    for (int i = 0; i < passwords.size(); i++) {
        if (passwords[i][0] == searchTerm) {
            cout << "Password found: " << passwords[i][1] << endl;
            return passwords[i][1];
        }
    }
    cout << "Password not found." << endl;
    return "";
}