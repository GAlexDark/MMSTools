/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  p7b file maker Console Utility
*  Common module
*
*  Module name: CPkcs7.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The Class for working with the p7b files
*     This class use the BioByteArray Class from the XCA Project
*
****************************************************************************/

#include "CPkcs7.h"

#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>

#include <openssl/pem.h>
#include <openssl/err.h>
#include "BioByteArray.h"


QString
CPkcs7::getOpenSslErrorMessage() const
{
    QString retVal;
    while (int i = ERR_get_error() ) {
        retVal += QStringLiteral("%1; ").arg(ERR_error_string(i, nullptr));
    }
    if (retVal.isEmpty()) {
        retVal = QStringLiteral("No errors in the queue.");
    }
    return retVal;
}

bool
CPkcs7::readToPkcs7Store(BIO *store)
{
    ERR_clear_error();
    m_errorString.clear();
    bool retVal = true;
    m_pkcs7Store.reset(d2i_PKCS7_bio(store, nullptr)); // works for PKCS7 as DER
    if (m_pkcs7Store.isNull()) {
        m_pkcs7Store.reset(PEM_read_bio_PKCS7(store, nullptr, nullptr, nullptr));
        if (m_pkcs7Store.isNull()) {
            m_errorString = getOpenSslErrorMessage();
            retVal = false;
        }
    }
    return retVal;
}

bool
CPkcs7::getCertsFromStore()
{
    bool retVal = true;
    ERR_clear_error();
    int nid = OBJ_obj2nid(m_pkcs7Store->type);
    switch (nid) {
    case NID_pkcs7_signed:
        m_certs = m_pkcs7Store->d.sign->cert;
        break;
    case NID_pkcs7_signedAndEnveloped:
        m_certs = m_pkcs7Store->d.signed_and_enveloped->cert;
        break;
    default: // included NID_undef
        m_errorString = QStringLiteral("The store contains unknown objects (NID: %1): %2").arg(QString::number(nid), getOpenSslErrorMessage());
        retVal = false;
        break;
    }
    if (retVal && m_certs == nullptr) {
        m_errorString = QStringLiteral("The store doesnt contains certificates");
        retVal = false;
    }
    return retVal;
}

bool
CPkcs7::readCert(const QByteArray &buf)
{
    bool retVal = true;
    ERR_clear_error();
    m_cert.reset(PEM_read_bio_X509(BioByteArray(buf).ro(), nullptr, nullptr, nullptr));
    if (m_cert.isNull()) {
        m_cert.reset(d2i_X509_bio(BioByteArray(buf).ro(), nullptr));
        if (m_cert.isNull()) {
            m_errorString = getOpenSslErrorMessage();
            retVal = false;
        }
    }
    return retVal;
}

bool
CPkcs7::readCertFromFile(const QString &certFileName)
{
    QFile file(certFileName);
    bool retVal = file.open(QIODevice::ReadOnly);
    if (retVal) {
        QByteArray buf(file.readAll());
        retVal = !buf.isEmpty();
        file.close();
        if (retVal) {
            retVal = readCert(buf);
            if (!retVal) {
                m_errorString = QStringLiteral("Error load certificate: %1").arg(m_errorString);
            }
        }
    } else {
        m_errorString = QStringLiteral("Error load certificate from file '%1': %2").arg(certFileName, file.errorString()) ;
    }
    return retVal;
}

CPkcs7::CPkcs7()
{
    ERR_load_crypto_strings();
    ERR_clear_error();
    OPENSSL_malloc_init();
    OpenSSL_add_all_algorithms();
}

CPkcs7::~CPkcs7()
{
    ERR_free_strings();
}

bool
CPkcs7::readStore(const QString &fileName)
{
    ERR_clear_error();
    QScopedPointer<BIO, bioFree> buf(BIO_new(BIO_s_file()));
    bool retVal = !buf.isNull();
    if (retVal) {
        ERR_clear_error();
        int res = BIO_read_filename(buf.data(), fileName.toStdString().data());
        if (res > 0) {
            retVal = readToPkcs7Store(buf.data());
            if (!retVal) {
                m_errorString = QStringLiteral("Error decoding read data: %1").arg(m_errorString);
            }
        } else {
            m_errorString = QStringLiteral("Error read file '%1': %2").arg(fileName, getOpenSslErrorMessage());
            retVal = false;
        }
    } else {
        m_errorString = QStringLiteral("Error create BIO: %1").arg(getOpenSslErrorMessage());
    }
    return retVal;
}

bool
CPkcs7::appendCerts(const QStringList &filesList)
{
    bool retVal = true;
    if (m_pkcs7Store.isNull()) { // the p7b file not read and m_pkcs7Store is empty
        ERR_clear_error();
        m_pkcs7Store.reset(PKCS7_new());
        if (!m_pkcs7Store.isNull()) {
            ERR_clear_error();
            int res = PKCS7_set_type(m_pkcs7Store.data(), NID_pkcs7_signed); // init type of PKCS#7 contentInfo
            if (res > 0) {
                ERR_clear_error();
                res = PKCS7_content_new(m_pkcs7Store.data(), NID_pkcs7_data);
                if (res <= 0) {
                    m_errorString = QStringLiteral("Error setting nested contentInfo in PKCS#7 structure: %1").arg(getOpenSslErrorMessage());
                    retVal = false;
                }
            } else {
                m_errorString = QStringLiteral("Error initializing ContentInfo type PKCS#7: %1").arg(getOpenSslErrorMessage());
                retVal = false;
            }
        } else {
            m_errorString = QStringLiteral("Error create PKCS7 object: %1").arg(getOpenSslErrorMessage());
            retVal = false;
        }
    }
    if (retVal) {
        for (const QString &fileName : filesList) {
            retVal = readCertFromFile(fileName);
            if (retVal) {
                ERR_clear_error();
                int res = PKCS7_add_certificate(m_pkcs7Store.data(), m_cert.data());
                if (res <= 0) {
                    m_errorString = QStringLiteral("Error add certificate from file '%1' to store: %2").arg(fileName, getOpenSslErrorMessage());
                    retVal = false;
                    break;
                }
            }
        }
    }
    return retVal;
}

bool
CPkcs7::saveStore(const QString &fileName)
{
    bool retVal = !m_pkcs7Store.isNull();
    if (retVal) {
        BioByteArray buf;
        ERR_clear_error();
        int res = i2d_PKCS7_bio(buf, m_pkcs7Store.data());
        if (res > 0) {
            QFile file(fileName);
            retVal = file.open(QIODevice::WriteOnly);
            if (retVal) {
                res = file.write(buf);
                if (res == -1) {
                    m_errorString = QStringLiteral("Error write to file: '%1': %2").arg(fileName, file.errorString());
                    retVal = false;
                }
                file.close();
            } else {
                m_errorString = QStringLiteral("Error opening file: '%1': %2").arg(fileName, file.errorString());
            }
        } else {
            m_errorString = QStringLiteral("Error decoding write data: %1").arg(getOpenSslErrorMessage());
            retVal = false;
        }
    } else {
        m_errorString = QStringLiteral("Nothing to save.");
    }

    return retVal;
}

int
CPkcs7::size()
{
    bool retVal = getCertsFromStore();
    return retVal ? sk_X509_num(m_certs) : -1;
}

QByteArray
CPkcs7::getHash(const QString &fileName)
{
    QCryptographicHash hash(QCryptographicHash::Sha1);
    QFile file(fileName);
    bool retVal = file.open(QFile::ReadOnly);
    if (retVal) {
        hash.addData(file.readAll());
        file.close();
    }
    return retVal ? hash.result() : QByteArray();
}

bool
CPkcs7::createHashFile(const QString &fileName, QString &errorString)
{
    QByteArray hash = CPkcs7::getHash(fileName);
    bool retVal = !hash.isEmpty();
    if (retVal) {
        QString baseName = QFileInfo(fileName).baseName(); // filename (wo ext)
        QString path = QFileInfo(fileName).path();
        QFile hashFile(QStringLiteral("%1/%2.sha").arg(path, baseName));
        retVal = hashFile.open(QIODevice::WriteOnly);
        if (retVal) {
            QTextStream stream(&hashFile);
            QString file = QFileInfo(fileName).fileName(); // filename.ext
            stream << QStringLiteral("%1 *%2").arg(hash.toHex(), file);
            hashFile.close();
        } else {
            errorString = QStringLiteral("Error! Could not create hash file: %1").arg(hashFile.errorString());
        }
    } else {
        errorString = QStringLiteral("Error calculate hash.");
    }

    return retVal;
}
