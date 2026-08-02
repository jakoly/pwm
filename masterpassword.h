#ifndef MASTERPASSWORD_H
#define MASTERPASSWORD_H

#include <QDialog>
#include <string>
#include "encrypt.h"

namespace Ui {
class MasterPasswordDialog;
}

// Dialog, der beim Programmstart nach dem Master-Passwort fragt.
// - Existiert noch kein Tresor (kein salt.bin): Passwort + Bestaetigung,
//   erzeugt einen neuen Tresor.
// - Existiert bereits ein Tresor: nur ein Feld, versucht zu entsperren.
//   Bei falschem Passwort wird eine Fehlermeldung angezeigt und der
//   Nutzer kann es erneut versuchen.
class MasterPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MasterPasswordDialog(Encrypt* encrypt, QWidget* parent = nullptr);
    ~MasterPasswordDialog();

    // true, sobald der Tresor erfolgreich (neu erstellt oder entsperrt) wurde.
    bool isUnlocked() const { return m_unlocked; }

private slots:
    void onOkClicked();
    void updateOkEnabled();

private:
    Ui::MasterPasswordDialog* ui;
    Encrypt* m_encrypt;
    bool m_isNewVault;
    bool m_unlocked = false;

    static const std::string SALT_PATH;
};

#endif // MASTERPASSWORD_H
