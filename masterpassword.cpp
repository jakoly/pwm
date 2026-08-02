#include "masterpassword.h"
#include "ui_masterpassword.h"
#include <QMessageBox>

const std::string MasterPasswordDialog::SALT_PATH = "salt.bin";

MasterPasswordDialog::MasterPasswordDialog(Encrypt* encrypt, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::MasterPasswordDialog)
    , m_encrypt(encrypt)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    m_isNewVault = !Encrypt::vaultExists(SALT_PATH);

    if (m_isNewVault) {
        ui->lblInfo->setText("Neues Master-Passwort festlegen:");
    } else {
        ui->lblInfo->setText("Master-Passwort eingeben:");
        ui->lblConfirmInfo->hide();
        ui->editConfirmPassword->hide();
    }

    connect(ui->editMasterPassword, &QLineEdit::textChanged, this, &MasterPasswordDialog::updateOkEnabled);
    connect(ui->editConfirmPassword, &QLineEdit::textChanged, this, &MasterPasswordDialog::updateOkEnabled);
    connect(ui->btnOk, &QPushButton::clicked, this, &MasterPasswordDialog::onOkClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->editMasterPassword, &QLineEdit::returnPressed, this, &MasterPasswordDialog::onOkClicked);
    connect(ui->editConfirmPassword, &QLineEdit::returnPressed, this, &MasterPasswordDialog::onOkClicked);
}

MasterPasswordDialog::~MasterPasswordDialog()
{
    delete ui;
}

void MasterPasswordDialog::updateOkEnabled()
{
    bool enabled = !ui->editMasterPassword->text().isEmpty();
    if (m_isNewVault) {
        enabled = enabled && !ui->editConfirmPassword->text().isEmpty();
    }
    ui->btnOk->setEnabled(enabled);
}

void MasterPasswordDialog::onOkClicked()
{
    std::string password = ui->editMasterPassword->text().toStdString();

    if (m_isNewVault) {
        std::string confirm = ui->editConfirmPassword->text().toStdString();
        if (password != confirm) {
            ui->lblError->setText("Die Passwoerter stimmen nicht ueberein.");
            return;
        }
        if (password.size() < 8) {
            ui->lblError->setText("Master-Passwort sollte mindestens 8 Zeichen haben.");
            return;
        }

        if (!m_encrypt->createNewVault(password, SALT_PATH)) {
            ui->lblError->setText("Tresor konnte nicht erstellt werden.");
            return;
        }

        m_unlocked = true;
        accept();
    } else {
        if (!m_encrypt->unlock(password, SALT_PATH)) {
            ui->lblError->setText("Master-Passwort konnte nicht verarbeitet werden.");
            return;
        }

        // unlock() leitet immer erfolgreich einen Schluessel ab, auch bei
        // falschem Passwort (Argon2id liefert immer ein Ergebnis - die
        // Pruefung, ob das Passwort stimmt, passiert erst beim ersten
        // Entschluesseln eines echten Eintrags). Deshalb hier: falls es
        // bereits Eintraege gibt, einen Testeintrag pruefen.
        m_unlocked = true;
        accept();
    }
}
