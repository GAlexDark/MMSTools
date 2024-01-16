#include <QtTest>
#include <QDebug>

#include "elcUtils.h"
// add necessary includes here

class UtilsTest : public QObject
{
    Q_OBJECT

public:
    UtilsTest();
    ~UtilsTest();

private slots:
    void test_parseValuesList1();
    void test_parseValuesList2();
    void test_sanitizeValue();
//    void test_expand_environment_variables();
};

UtilsTest::UtilsTest() {}

UtilsTest::~UtilsTest() {}

void
UtilsTest::test_parseValuesList1()
{
    qDebug() << "elcUtils::parseValuesList(QString data)";

    QString data;
    data.clear();
    QStringList retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 0);

    data = "user1;user2;user3;user4";
    retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 4);

    retVal.clear();
    data = data + ';';
    retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 4);

    retVal.clear();
    data = data + ";user3";
    retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 4);

    retVal.clear();
    data = "user1;user2;user3,user4";
    retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 4);

    retVal.clear();
    data = "user1";
    retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 1);

    retVal.clear();
    data = data + ';';
    retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 1);
}

void
UtilsTest::test_parseValuesList2()
{
    qDebug() << "elcUtils::parseValuesList(QStringList data)";

    QStringList data;
    data.clear();
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 0);

    data.clear();
    data << "user1" << "user2" << "user3" << "user4";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1" << "user2" << "user3" << "" <<"user4" << "";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1" << "user2" << "user3" << "" <<"user4" << "" << "user2";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 1);
    QCOMPARE(data.at(0), "user1");

    data.clear();
    data << "user1;";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 1);
    QCOMPARE(data.at(0), "user1");

    data.clear();
    data << "user1;user2;user3;user4";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1;user2;user3;user4;";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1;user2;user3;user4;;user3";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1;user2,user3;user4;";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 1);
}

void
UtilsTest::test_sanitizeValue()
{
    bool retVal = elcUtils::sanitizeValue("Mr_data01");
    QCOMPARE(retVal, true);

    retVal = elcUtils::sanitizeValue("Mr-data01");
    QCOMPARE(retVal, false);
}
/*
void
UtilsTest::test_expand_environment_variables()
{
    QString data = QStringLiteral("$TMP\\dir1\\$OS\\.myconfigfile");
    elcUtils::expand_environment_variables(data);
    qDebug() << data;
    QVERIFY(true);
}
*/
QTEST_APPLESS_MAIN(UtilsTest)

#include "tst_utilstest.moc"
