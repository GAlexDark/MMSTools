/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  MMS XLSX to JSON Conversion Console Utility
*  MMS XLSX to JSON Conversion Console Utility
*
*  Module name: main.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     Main file
*
****************************************************************************/

#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QLocale>

#include "CConsoleOutput.h"
#include "elcUtils.h"
#include "CX2jConvCmdLineParser.h"
#include "MMSTypes.h"

const QString errorMsg = QStringLiteral("Error at line %1: The '%2' value '%3' is wrong!\n");
const QString DataSend = QLatin1String("dataSend");
const QString DebtCredit = QLatin1String("debtCredit");
const QString DebtDebit = QLatin1String("debtDebit");
const QString PaymentSumCredit = QLatin1String("paymentSumCredit");
const QString PaymentSumDebit = QLatin1String("paymentSumDebit");
const QString ReportNumber = QLatin1String("reportNumber");


QString
toString(const QVariant &value, bool &isOk)
{
    QString retVal;
    isOk = !value.isNull();
    if (isOk) {
        retVal = value.toString().trimmed();
        isOk = !retVal.isEmpty();
    }
    return retVal;
}

QString
toDateTimeAsString(const QVariant &value, bool &isOk)
{
    QString retVal = toString(value, isOk);
    if (isOk) {
        QDateTime val = QDateTime::fromString(retVal, Qt::ISODate);
        isOk = val.isValid();
    }
    return retVal;
}

double
toDouble(const QVariant &value, bool &isOk)
{
    double retVal = 0.0;
    QString buf = toString(value, isOk); //for correct conversion of values ​​like 10,000.00
    if (isOk) {
        QLocale c(QLocale::C);
        if ((buf.indexOf('.') == -1) && buf.indexOf(',') != -1 ) {
            buf.replace(',', '.');
        }
        retVal = c.toDouble(buf, &isOk);
        isOk = isOk && !std::isnan(retVal);
    }
    return retVal;
}

QString
toReportNumber(const QVariant &value, bool &isOk)
{
    QString retVal = toString(value, isOk);
    if (isOk) {
        if (retVal.length() > 13) {
            isOk = false;
        } else {
            if (retVal.length() < 13) {
                retVal = retVal.rightJustified(13, '0');
            }
        }
    }
    return retVal;
}

bool
checkHeader(const QXlsx::Document &value)
{
    bool retVal = false;
    QVariant cell;
    QString buf;
    int row = 1;
    QStringList headers = { DataSend, DebtCredit, DebtDebit, PaymentSumCredit, PaymentSumDebit, ReportNumber };
    for (int i = 0; i < headers.size() - 1; ++i) {
        cell = value.read(row, i + 1);
        buf = toString(cell, retVal);
        retVal = retVal && QString::compare(buf, headers.at(i), Qt::CaseInsensitive) == 0;
        if (!retVal) {
            break;
        }
    }
    return retVal;
}

class jsonConvError : public mms::MmsCommonException
{
public:
    explicit jsonConvError(const QString &text) noexcept
        : mms::MmsCommonException(text) {}
};

QJsonArray
read(const QXlsx::Document &dataSource, int &row)
{
    int numberOfRow = dataSource.dimension().lastRow();
    QVariant cell;
    QString dataSend;
    double debtCredit;
    double debtDebit;
    double paymentSumCredit;
    double paymentSumDebit;
    QString reportNumber;
    bool res = true;
    QJsonObject recordObject;
    QJsonArray retVal;

    while (row <= numberOfRow) {
        cell = dataSource.read(row, 1);
        dataSend = toDateTimeAsString(cell, res);
        if (!res) {
            throw jsonConvError(errorMsg.arg(row).arg(DataSend, dataSend));
        }

        cell = dataSource.read(row, 2);
        debtCredit = toDouble(cell, res);
        if (!res) {
            throw jsonConvError(errorMsg.arg(row).arg(DebtCredit).arg(debtCredit));
        }

        cell = dataSource.read(row, 3);
        debtDebit = toDouble(cell, res);
        if (!res) {
            throw jsonConvError(errorMsg.arg(row).arg(DebtDebit).arg(debtDebit));
        }

        cell = dataSource.read(row, 4);
        paymentSumCredit = toDouble(cell, res);
        if (!res) {
            throw jsonConvError(errorMsg.arg(row).arg(PaymentSumCredit).arg(paymentSumCredit));
        }

        cell = dataSource.read(row, 5);
        paymentSumDebit = toDouble(cell, res);
        if (!res) {
            throw jsonConvError(errorMsg.arg(row).arg(PaymentSumDebit).arg(paymentSumDebit));
        }

        cell = dataSource.read(row, 6);
        reportNumber = toReportNumber(cell, res);
        if (!res) {
            throw jsonConvError(errorMsg.arg(row).arg(ReportNumber, reportNumber));
        }

        recordObject = QJsonObject();
        recordObject.insert(DataSend, QJsonValue::fromVariant(dataSend));
        recordObject.insert(DebtCredit, QJsonValue::fromVariant(debtCredit));
        recordObject.insert(DebtDebit, QJsonValue::fromVariant(debtDebit));
        recordObject.insert(PaymentSumCredit, QJsonValue::fromVariant(paymentSumCredit));
        recordObject.insert(PaymentSumDebit, QJsonValue::fromVariant(paymentSumDebit));
        recordObject.insert(ReportNumber, QJsonValue::fromVariant(reportNumber));
        retVal.push_back(recordObject);

        ++row;
    } // while
    return retVal;
}

bool
convertSheet(const QXlsx::Document &dataSource, const QString &fileName, OutputMode mode, QString &msgString)
{
    bool retVal = checkHeader(dataSource);
    if (retVal) {
        int row = 2;
        int numberOfRow = dataSource.dimension().lastRow();
        if (numberOfRow >= row) {
            QJsonArray recordsArray;
            try {
                recordsArray = read(dataSource, row);
            } catch (const jsonConvError &ex) {
                msgString = ex.what();
                retVal = false;
            }
            if (retVal) {
                if (!recordsArray.isEmpty()) {
                    QFile jsonFile(fileName);
                    retVal = jsonFile.open(QIODevice::WriteOnly);
                    if (retVal) {
                        QJsonDocument jsonResult(recordsArray);
                        QJsonDocument::JsonFormat outputJsonFormat = mode == OutputMode::OUTPUTMODE_INDENTED ? QJsonDocument::Indented : QJsonDocument::Compact;
                        retVal = jsonFile.write(jsonResult.toJson(outputJsonFormat)) != -1;
                        jsonFile.close();
                        if (retVal) {
                            msgString = QLatin1String("\nTotal rows converted: %1.\nThe JSON was saved in the file: %2\n").arg(QString::number(row - 2)).arg(fileName);
                        } else {
                            msgString = QLatin1String("Error save result to the file: %1").arg(fileName);
                        }
                    } else {
                        msgString = QLatin1String("Error create file '%1': %2").arg(fileName, jsonFile.errorString());
                    }
                } else {
                    msgString = QLatin1String("Nothing to save.");
                    retVal = true;
                }
            }
        } else {
            msgString = QLatin1String("The file %1 is empty.").arg(fileName);
        }
    } else {
        msgString = QLatin1String("The file %1 header is wrong.").arg(fileName);
    }
    return retVal;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationVersion(QStringLiteral("%1 tag %2 %3").arg(BUILD_VER, BUILD_GIT, elcUtils::getFormattedDateTime( BUILD_DATE )));

    CConsoleOutput consoleOut;
    QString description(QStringLiteral("MMS XLSX to JSON Conversion Utility Version %1\nCopyright (C) 2024 Oleksii Gaienko, %3\n\n"));
    description.append(QStringLiteral("This program use Qt version %2 and QXlsx library: https://github.com/QtExcel/QXlsx.\n"));
    consoleOut.outToConsole(description.arg(QCoreApplication::applicationVersion(), QT_VER, CONTACT));

    CX2jConvCmdLineParser cmd;
    if (!cmd.parseCmdArgs(a)) {
        consoleOut.outToConsole(cmd.errorString());
        return 1;
    }
    if (argc == 1) {
        consoleOut.outToConsole(QLatin1String("The arguments are missing."));
        cmd.showHelpAndExit();
        return 0;
    }

    consoleOut.outToConsole(QLatin1String("MMS XLSX to JSON Conversion Utility starting...\n"));

    QString fileName;
    if (!cmd.getDataFile(fileName)) {
        consoleOut.outToConsole(cmd.errorString());
        return 1;
    }

    QXlsx::Document dataSource(fileName);
    bool retVal = dataSource.load();
    if (retVal) {
        fileName = cmd.getReportName();
        OutputMode mode = cmd.getOutputMode();
        QString msgString;
        qsizetype sheetCount = dataSource.sheetNames().size();
        if (sheetCount > 1) {
            QString currentSheetName;
            QString fileNameWithSheet;
            for (int sheetIndexNumber = 0; sheetIndexNumber < sheetCount; ++sheetIndexNumber) {
                currentSheetName = dataSource.sheetNames().at(sheetIndexNumber);
                fileNameWithSheet = fileName;
                fileNameWithSheet.replace(QLatin1String(".json"), QStringLiteral(".%1.json").arg(currentSheetName));
                retVal = dataSource.selectSheet(sheetIndexNumber);
                if (retVal) {
                    retVal = convertSheet(dataSource, fileNameWithSheet, mode, msgString);
                    consoleOut.outToConsole(QStringLiteral("Worksheet '%1': %2").arg(currentSheetName, msgString));
                } else {
                    consoleOut.outToConsole(QStringLiteral("Worksheet select error."));
                    break;
                }
            } //for
        } else {
            retVal = convertSheet(dataSource, fileName, mode, msgString);
            consoleOut.outToConsole(msgString);
        }
    } else {
        consoleOut.outToConsole(QLatin1String("Error load xlsx file %1").arg(fileName));
    }

    return retVal ? 0 : 1;
}
