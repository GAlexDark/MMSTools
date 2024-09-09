/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  p7b file maker Console Utility
*  Common module
*
*  Module name: CPkcs7.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The Class for working with the p7b files
*     This class use the BioByteArray Class from the XCA Project
*     ToDo: X509 will be placed in an individual class
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
