#include <QtTest>
#include <QStringList>
#include <QDebug>

#include "CPkcs7.h"

class CPkcs7Test : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

private:
    CPkcs7 m_store;

private slots:
    void test_readPemCert();
    void test_readBadPemCert_wCRLF();
    void test_readBadPemCert_wLF();
    void test_readBadPemCert_woRN();
    void test_readDerCert();
    void test_getCertsCount();
    void test_readP7b();
};

void
CPkcs7Test::test_readPemCert()
{
    qDebug() << "Read the PEM certificate";
    bool retVal = m_store.appendCerts(QStringList() << SRCDIR"data/testcert1_pem.cer" );
    if (!retVal) {
        qDebug() << QStringLiteral("Error: %1").arg(m_store.errorString());
    }
    QVERIFY(retVal);
}

void
CPkcs7Test::test_readBadPemCert_wCRLF()
{
    qDebug() << "Read the PEM certificate without the BEGIN and END lines and the CRLF";
    bool retVal = m_store.appendCerts(QStringList() << SRCDIR"data/testcert2_badpem_wCRLF.cer" );
    if (!retVal) {
        qDebug() << QStringLiteral("Error: %1").arg(m_store.errorString());
    }
    QVERIFY(retVal);
}

void
CPkcs7Test::test_readBadPemCert_wLF()
{
    qDebug() << "Read the PEM certificate without the BEGIN and END lines and the LF";
    bool retVal = m_store.appendCerts(QStringList() << SRCDIR"data/testcert2_badpem_wLF.cer" );
    if (!retVal) {
        qDebug() << QStringLiteral("Error: %1").arg(m_store.errorString());
    }
    QVERIFY(retVal);
}

void
CPkcs7Test::test_readBadPemCert_woRN()
{
    qDebug() << "Read the PEM certificate without the BEGIN and END lines and without RN";
    bool retVal = m_store.appendCerts(QStringList() << SRCDIR"data/testcert2_badpem_woRN.cer" );
    if (!retVal) {
        qDebug() << QStringLiteral("Error: %1").arg(m_store.errorString());
    }
    QVERIFY(retVal);
}

void
CPkcs7Test::test_readDerCert()
{
    qDebug() << "Read the DER certificate";
    bool retVal = m_store.appendCerts(QStringList() << SRCDIR"data/testcert3_der.cer" );
    if (!retVal) {
        qDebug() << QStringLiteral("Error: %1").arg(m_store.errorString());
    }
    QVERIFY(retVal);
}

void
CPkcs7Test::test_getCertsCount()
{
    qDebug() << "Get the certificates count";
    int certsCount = m_store.size();
    QCOMPARE(certsCount, 5);
}

void
CPkcs7Test::test_readP7b()
{
    qDebug() << "Read the P7B container";
    bool retVal = m_store.readStore(SRCDIR"data/testp7b.p7b");
    if (!retVal) {
        qDebug() << QStringLiteral("Error read p7b file: %1.").arg(m_store.errorString());
    }
    QVERIFY(retVal);

    int certsCount = m_store.size();
    QCOMPARE(certsCount, 3);
}

QTEST_APPLESS_MAIN(CPkcs7Test)

#include "tst_cpkcs7test.moc"
