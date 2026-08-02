#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <string>
#include <functional> // für std::hash
#include <cstdlib>
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
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
    vector<vector<string>> passwords;


private slots:
    void on_btnNewPassword_clicked();

private:
    Ui::MainWindow *ui;
    hash<string> hasher; // Hash-Objekt für Strings
    size_t mPassword = 5411735986536431176;

    void sleep(int seconds);
    void loadPasswords();
    string searchPassword(string searchTerm);
};
#endif // MAINWINDOW_H
