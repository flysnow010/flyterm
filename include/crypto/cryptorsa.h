#ifndef CRYPTORSA_H
#define CRYPTORSA_H

#include "config.h"
#include <QString>

class CRYPTO_EXPORT CryptoRSA
{
public:
    CryptoRSA();

    static void GenerateKeys(QString & privateKey, QString & publicKey, int keyLength = 1024);
    static QString EncryptString(QString const& publicKey, QString const& text);
    static QString DecryptString(QString const& privateKey, QString const& cryptText);
};

#endif // CRYPTORSA_H
