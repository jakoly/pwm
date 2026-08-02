#include "encrypt.h"
#include <fstream>
#include <stdexcept>
#include <sodium.h>

using namespace std;

bool Encrypt::init()
{
    // sodium_init() gibt 0 bei Erstinitialisierung, 1 wenn schon initialisiert,
    // -1 bei Fehler zurueck.
    return sodium_init() >= 0;
}

bool Encrypt::vaultExists(const string& saltPath)
{
    ifstream f(saltPath, ios::binary);
    return f.good();
}

bool Encrypt::deriveKey(const string& masterPassword, const vector<uint8_t>& salt)
{
    if (salt.size() != crypto_pwhash_SALTBYTES) {
        return false;
    }

    m_key.resize(crypto_secretbox_KEYBYTES);

    // Argon2id (crypto_pwhash nutzt standardmaessig Argon2id) mit
    // moderaten Kosten - guter Kompromiss aus Sicherheit und Wartezeit
    // fuer eine Desktop-Anwendung.
    int rc = crypto_pwhash(
        m_key.data(), m_key.size(),
        masterPassword.c_str(), masterPassword.size(),
        salt.data(),
        crypto_pwhash_OPSLIMIT_MODERATE,
        crypto_pwhash_MEMLIMIT_MODERATE,
        crypto_pwhash_ALG_ARGON2ID13
    );

    if (rc != 0) {
        // Vermutlich zu wenig RAM verfuegbar fuer die gewaehlten Kosten.
        m_key.clear();
        return false;
    }

    m_unlocked = true;
    return true;
}

bool Encrypt::createNewVault(const string& masterPassword, const string& saltPath)
{
    vector<uint8_t> salt(crypto_pwhash_SALTBYTES);
    randombytes_buf(salt.data(), salt.size());

    ofstream out(saltPath, ios::binary | ios::trunc);
    if (!out.is_open()) {
        return false;
    }
    out.write(reinterpret_cast<const char*>(salt.data()), salt.size());
    out.close();

    return deriveKey(masterPassword, salt);
}

bool Encrypt::unlock(const string& masterPassword, const string& saltPath)
{
    ifstream in(saltPath, ios::binary);
    if (!in.is_open()) {
        return false;
    }

    vector<uint8_t> salt(crypto_pwhash_SALTBYTES);
    in.read(reinterpret_cast<char*>(salt.data()), salt.size());
    if (!in) {
        return false; // Datei zu kurz / beschaedigt
    }

    return deriveKey(masterPassword, salt);
}

string Encrypt::encryptText(const string& plaintext) const
{
    if (!m_unlocked) {
        throw runtime_error("Encrypt: Schluessel nicht initialisiert (unlock() zuerst aufrufen).");
    }

    vector<uint8_t> nonce(crypto_secretbox_NONCEBYTES);
    randombytes_buf(nonce.data(), nonce.size());

    vector<uint8_t> ciphertext(plaintext.size() + crypto_secretbox_MACBYTES);

    crypto_secretbox_easy(
        ciphertext.data(),
        reinterpret_cast<const uint8_t*>(plaintext.data()), plaintext.size(),
        nonce.data(),
        m_key.data()
    );

    // Nonce + Ciphertext zusammen speichern, damit decryptText() beides hat.
    vector<uint8_t> combined;
    combined.reserve(nonce.size() + ciphertext.size());
    combined.insert(combined.end(), nonce.begin(), nonce.end());
    combined.insert(combined.end(), ciphertext.begin(), ciphertext.end());

    size_t b64_len = sodium_base64_encoded_len(combined.size(), sodium_base64_VARIANT_ORIGINAL);
    vector<char> b64(b64_len);
    sodium_bin2base64(
        b64.data(), b64.size(),
        combined.data(), combined.size(),
        sodium_base64_VARIANT_ORIGINAL
    );

    return string(b64.data());
}

string Encrypt::decryptText(const string& encoded) const
{
    if (!m_unlocked) {
        throw runtime_error("Encrypt: Schluessel nicht initialisiert (unlock() zuerst aufrufen).");
    }

    vector<uint8_t> combined(encoded.size()); // obere Schranke, base64 ist nie kleiner als Binaerdaten
    size_t bin_len = 0;

    int rc = sodium_base642bin(
        combined.data(), combined.size(),
        encoded.c_str(), encoded.size(),
        nullptr, &bin_len,
        nullptr,
        sodium_base64_VARIANT_ORIGINAL
    );
    if (rc != 0) {
        throw runtime_error("Encrypt: Ungueltige Base64-Daten.");
    }
    combined.resize(bin_len);

    if (combined.size() < crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES) {
        throw runtime_error("Encrypt: Daten zu kurz / beschaedigt.");
    }

    vector<uint8_t> nonce(combined.begin(), combined.begin() + crypto_secretbox_NONCEBYTES);
    vector<uint8_t> ciphertext(combined.begin() + crypto_secretbox_NONCEBYTES, combined.end());

    vector<uint8_t> plaintext(ciphertext.size() - crypto_secretbox_MACBYTES);

    rc = crypto_secretbox_open_easy(
        plaintext.data(),
        ciphertext.data(), ciphertext.size(),
        nonce.data(),
        m_key.data()
    );

    if (rc != 0) {
        // Authentifizierung fehlgeschlagen: falsches Master-Passwort oder
        // manipulierte/beschaedigte Daten.
        throw runtime_error("Entschluesselung fehlgeschlagen: falsches Master-Passwort oder beschaedigte Daten.");
    }

    return string(plaintext.begin(), plaintext.end());
}

void Encrypt::clearKey()
{
    if (!m_key.empty()) {
        sodium_memzero(m_key.data(), m_key.size());
    }
    m_key.clear();
    m_unlocked = false;
}

Encrypt::~Encrypt()
{
    clearKey();
}
