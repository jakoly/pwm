#ifndef ENCRYPT_H
#define ENCRYPT_H
#include <string>
#include <vector>
#include <cstdint>
#include <sodium.h>

// Kapselt Schluesselableitung (Argon2id) und authentifizierte Verschluesselung
// (XSalsa20-Poly1305 via libsodium) fuer den Passwortmanager.
//
// Verwendung:
//   1. Beim ersten Start: Encrypt::createNewVault(masterPassword) aufrufen.
//      Das erzeugt Salt-Datei und leitet den Schluessel ab.
//   2. Bei jedem weiteren Start: Encrypt::unlock(masterPassword) aufrufen.
//      Das laedt den vorhandenen Salt und leitet denselben Schluessel ab.
//   3. encryptText()/decryptText() fuer einzelne Passwort-Eintraege nutzen.
class Encrypt
{
public:
    // Initialisiert libsodium. Muss einmalig vor jeder anderen Nutzung
    // aufgerufen werden (z.B. in main()). Gibt false zurueck, falls die
    // Bibliothek nicht initialisiert werden konnte.
    static bool init();

    // Prueft, ob bereits ein Salt (also ein bestehender Tresor) existiert.
    static bool vaultExists(const std::string& saltPath = "salt.bin");

    // Erzeugt einen neuen, zufaelligen Salt, speichert ihn in saltPath
    // und leitet daraus den Schluessel aus masterPassword ab.
    // Nur beim allerersten Start verwenden.
    bool createNewVault(const std::string& masterPassword,
                         const std::string& saltPath = "salt.bin");

    // Laedt den bestehenden Salt aus saltPath und leitet den Schluessel
    // aus masterPassword ab. Schluessel wird erst beim Entschluesseln
    // eines Eintrags auf Korrektheit geprueft (falsches Passwort fuehrt
    // zu einem Fehler bei decryptText).
    bool unlock(const std::string& masterPassword,
                const std::string& saltPath = "salt.bin");

    // Verschluesselt Klartext mit dem aktuell abgeleiteten Schluessel.
    // Rueckgabe ist Base64-kodiert und enthaelt Nonce + Ciphertext,
    // damit sie sicher als eine Zeile in passwords.txt gespeichert werden kann.
    std::string encryptText(const std::string& plaintext) const;

    // Entschluesselt einen mit encryptText() erzeugten String.
    // Wirft std::runtime_error, wenn die Daten beschaedigt sind oder
    // das Master-Passwort falsch war (Authentifizierung schlaegt fehl).
    std::string decryptText(const std::string& encoded) const;

    // Ueberschreibt den Schluessel im Speicher (z.B. beim Schliessen der App).
    void clearKey();

    ~Encrypt();

private:
    std::vector<uint8_t> m_key; // crypto_secretbox_KEYBYTES (32) Bytes
    bool m_unlocked = false;

    bool deriveKey(const std::string& masterPassword,
                    const std::vector<uint8_t>& salt);
};

#endif // ENCRYPT_H
