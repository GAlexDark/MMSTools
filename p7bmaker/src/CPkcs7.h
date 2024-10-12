/****************************************************************************
*
* The class for work with a p7b files.
* Copyright (C) 2024  Oleksii Gaienko
* Contact: galexsoftware@gmail.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

#ifndef CPKCS7_H
#define CPKCS7_H

#include <QString>
#include <QScopedPointer>
#include <QStringList>
#include <QByteArray>

#include <openssl/pkcs7.h>
#include <openssl/x509.h>
#include <openssl/bio.h>


class CPkcs7
{
public:
    CPkcs7();
    ~CPkcs7();
    bool readStore(const QString &fileName);
    bool appendCerts(const QStringList &filesList);
    bool saveStore(const QString &fileName);
    int size(); //Returns the number of items in the pkcs7 store
    const QString& errorString() const { return m_errorString; }

    static QByteArray getHash(const QString &fileName);
    static bool createHashFile(const QString &fileName, QString &errorString);

private:
    QString getOpenSslErrorMessage() const;
    bool readToPkcs7Store(BIO *store);
    bool getCertsFromStore();
    bool readCert(const QByteArray &buf);
    bool readCertFromFile(const QString &certFileName);
    bool isBase64Data(const QByteArray &data) const;

    struct bioFree
    {
        static inline void cleanup(BIO *pointer)
        {
            BIO_free(pointer);
        }
    };
    struct pkcs7Free
    {
        static inline void cleanup(PKCS7 *pointer)
        {
            PKCS7_free(pointer);
        }
    };
    struct x509Free
    {
        static inline void cleanup(X509 *pointer)
        {
            X509_free(pointer);
        }
    };
    QScopedPointer<PKCS7, pkcs7Free> m_pkcs7Store;
    QScopedPointer<X509, x509Free> m_cert;

    QString m_errorString;
    STACK_OF(X509) *m_certs = nullptr;
};

#endif // CPKCS7_H
