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

#include "CPkcs7.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include <openssl/pem.h>
#include <openssl/err.h>

namespace {
    const QRegularExpression rePem(QLatin1String("[^a-zA-Z0-9+/=\r\n]"));
    const QByteArray beginLine("-----BEGIN CERTIFICATE-----\n");
    const QByteArray endLine("\n-----END CERTIFICATE-----");
}

QString
CPkcs7::getOpenSslErrorMessage() const
{
    QString retVal;
    unsigned long errCode;
    while ((errCode = ERR_get_error()) != 0) {
        retVal += QStringLiteral("%1; ").arg(ERR_error_string(errCode, nullptr));
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

    m_pkcs7Store.reset(d2i_PKCS7_bio(store, nullptr)); // works for PKCS7 as DER
    if (m_pkcs7Store.isNull()) {
        m_pkcs7Store.reset(PEM_read_bio_PKCS7(store, nullptr, nullptr, nullptr));
        if (m_pkcs7Store.isNull()) {
            m_errorString = getOpenSslErrorMessage();
            return false;
        }
    }
    return true;
}

bool
CPkcs7::getCertsFromStore()
{
    ERR_clear_error();
    const int nid = OBJ_obj2nid(m_pkcs7Store->type);
    switch (nid) {
        case NID_pkcs7_signed:
            m_certs = m_pkcs7Store->d.sign->cert;
            break;
        case NID_pkcs7_signedAndEnveloped:
            m_certs = m_pkcs7Store->d.signed_and_enveloped->cert;
            break;
        default: // included NID_undef
            m_errorString = QStringLiteral("The store contains unknown objects (NID: %1): %2").arg(QString::number(nid), getOpenSslErrorMessage());
            return false;
    }
    if (!m_certs) {
        m_errorString = QStringLiteral("The store doesn't contain certificates");
        return false;
    }
    return true;
}

bool
CPkcs7::readCert(const QByteArray &buf)
{
    ERR_clear_error();
    QScopedPointer<BIO, bioFree> pBuf(BIO_new_mem_buf(buf.constData(), buf.length()));
    if (buf.contains("BEGIN") && buf.contains("END")) {
        m_cert.reset(PEM_read_bio_X509(pBuf.data(), nullptr, nullptr, nullptr));
    } else {
        m_cert.reset(d2i_X509_bio(pBuf.data(), nullptr));
    }
    if (m_cert.isNull()) {
        m_errorString = getOpenSslErrorMessage();
        return false;
    }
    return true;
}

bool
CPkcs7::readCertFromFile(const QString &certFileName)
{
    QFile file(certFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        m_errorString = QStringLiteral("Error loading certificate from file '%1': %2").arg(certFileName, file.errorString());
        return false;
    }
    QByteArray buf = file.readAll().trimmed();
    file.close();
    if (buf.isEmpty()) {
        m_errorString = QStringLiteral("Error loading certificate %1: buffer is empty").arg(certFileName);
        return false;
    }
    if (isBase64Data(buf)) { // The PEM certificate without the start and the end lines
        buf = beginLine + buf + endLine;
    }
    if (!readCert(buf)) {
        m_errorString = QStringLiteral("Error loading certificate %1: %2").arg(certFileName, m_errorString);
        return false;
    }
    return true;
}

bool
CPkcs7::isBase64Data(const QByteArray &data) const
{
    //ref: https://www.kuikie.com/snippet/55/cpp-how-to-check-if-a-qstring-is-base64-encoded
    return !rePem.match(data).hasMatch();
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
    if (buf.isNull()) {
        m_errorString = QStringLiteral("Error creating BIO: %1").arg(getOpenSslErrorMessage());
        return false;
    }
    int res = BIO_read_filename(buf.data(), fileName.toStdString().data());
    if (res <= 0) {
        m_errorString = QStringLiteral("Error reading file '%1': %2").arg(fileName, getOpenSslErrorMessage());
        return false;
    }
    if (!readToPkcs7Store(buf.data())) {
        m_errorString = QStringLiteral("Error decoding read data: %1").arg(m_errorString);
        return false;
    }
    return true;
}

bool
CPkcs7::appendCerts(const QStringList &filesList)
{
    if (m_pkcs7Store.isNull()) { // The p7b file not read and m_pkcs7Store is empty
        ERR_clear_error();
        m_pkcs7Store.reset(PKCS7_new());
        if (m_pkcs7Store.isNull()) {
            m_errorString = QStringLiteral("Error creating PKCS7 object: %1").arg(getOpenSslErrorMessage());
            return false;
        }
        if (PKCS7_set_type(m_pkcs7Store.data(), NID_pkcs7_signed) <= 0) {
            m_errorString = QStringLiteral("Error initializing ContentInfo type PKCS#7: %1").arg(getOpenSslErrorMessage());
            return false;
        } else if (PKCS7_content_new(m_pkcs7Store.data(), NID_pkcs7_data) <= 0) {
            m_errorString = QStringLiteral("Error setting nested contentInfo in PKCS#7 structure: %1").arg(getOpenSslErrorMessage());
            return false;
        }
    }

    for (const QString &fileName : std::as_const(filesList)) {
        if (!readCertFromFile(fileName) || PKCS7_add_certificate(m_pkcs7Store.data(), m_cert.data()) <= 0) {
            m_errorString = QStringLiteral("Error adding certificate from file '%1' to store: %2").arg(fileName, getOpenSslErrorMessage());
            return false;
        }
    }
    return true;
}

bool
CPkcs7::saveStore(const QString &fileName, OUTPUT_FORMAT outputFormat)
{
    if (m_pkcs7Store.isNull()) {
        m_errorString = QStringLiteral("Nothing to save.");
        return false;
    }
    QScopedPointer<BIO, bioFree> buf(BIO_new(BIO_s_mem()));
    ERR_clear_error();
    int res = (outputFormat == OUTPUT_FORMAT::FORMAT_ASN1) ? i2d_PKCS7_bio(buf.data(), m_pkcs7Store.data()) : PEM_write_bio_PKCS7(buf.data(), m_pkcs7Store.data());
    if (res <= 0) {
        m_errorString = QStringLiteral("Error decoding PKCS7 data: %1").arg(getOpenSslErrorMessage());
        return false;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        m_errorString = QStringLiteral("Error opening file '%1': %2").arg(fileName, file.errorString());
        return false;
    }
    const char *pBuf = nullptr;
    int len = BIO_get_mem_data(buf.data(), &pBuf);
    if (len <= 0) {
        file.close();
        m_errorString = QStringLiteral("Nothing to save.");
        return false;
    }
    if (file.write(pBuf, len) == -1) {
        file.close();
        m_errorString = QStringLiteral("Error writing to file: '%1': %2").arg(fileName, file.errorString());
        return false;
    }
    file.close();
    return true;
}

int
CPkcs7::size()
{
    return getCertsFromStore() ? sk_X509_num(m_certs) : -1;
}

QByteArray
CPkcs7::getHash(const QString &fileName)
{
    QCryptographicHash hash(QCryptographicHash::Sha1);
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        return QByteArray();
    }
    hash.addData(file.readAll());
    file.close();
    return hash.result();
}

bool
CPkcs7::createHashFile(const QString &fileName, QString &errorString)
{
    QByteArray hash = getHash(fileName);
    if (hash.isEmpty()) {
        errorString = QStringLiteral("Error calculating hash.");
        return false;
    }
    QString baseName = QFileInfo(fileName).completeBaseName(); // filename (wo ext)
    QString path = QFileInfo(fileName).path();
    QFile hashFile(QDir(path).filePath(baseName + ".sha"));
    if (!hashFile.open(QIODevice::WriteOnly)) {
        errorString = QStringLiteral("Error creating hash file: %1").arg(hashFile.errorString());
        return false;
    }
    QTextStream stream(&hashFile);
    stream << QStringLiteral("%1 *%2").arg(QString::fromUtf8(hash.toHex()), QFileInfo(fileName).fileName()); // filename.ext
    hashFile.close();
    return true;
}
