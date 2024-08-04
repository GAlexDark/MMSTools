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
#include <QTextStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QLocale>

#include "CConsoleOutput.h"
#include "elcUtils.h"
#include "QCommandLineParserHelper.h"

const int defaultNumberOfColumns = 6;

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
    QStringList headers = { "dataSend", "debtCredit", "debtDebit", "paymentSumCredit", "paymentSumDebit", "reportNumber" };
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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationVersion(QStringLiteral("%1 tag %2 %3").arg(BUILD_VER, BUILD_GIT, elcUtils::getFormattedDateTime( BUILD_DATE )));

    CConsoleOutput consoleOut;
    QString description(QStringLiteral("MMS XLSX to JSON Conversion Utility Version %1\nCopyright (C) 2024 Oleksii Gaienko, %3\n\n"));
    description.append(QStringLiteral("This program use Qt version %2 and QXlsx library: https://github.com/QtExcel/QXlsx.\n"));
    consoleOut.outToConsole(description.arg(QCoreApplication::applicationVersion(), QT_VER, CONTACT));

    xlsxc::QCommandLineParserHelper cmd;
    if (!cmd.parseCmdArgs(a)) {
        consoleOut.outToConsole(cmd.errorString());
        return 1;
    }
    if (argc == 1) {
        consoleOut.outToConsole(QLatin1String("The arguments are missing."));
        cmd.showHelpAndExit();
        return 0;
    }

    consoleOut.outToConsole(QLatin1String("MMS XLSX to JSON Conversion Utility starting..."));

    QString fileName;
    if (!cmd.getDataFile(fileName)) {
        consoleOut.outToConsole(cmd.errorString());
        return 1;
    }

    QXlsx::Document dataSource(fileName);
    bool retVal = dataSource.load();
    if (retVal) {
        int numberOfColumn = dataSource.dimension().lastColumn();
        if (numberOfColumn == defaultNumberOfColumns) {
            retVal = checkHeader(dataSource);
            if (retVal) {
                int row = 2;
                int numberOfRow = dataSource.dimension().lastRow();
                if (numberOfRow >= row) {
                    QVariant cell;
                    QString dataSend;
                    double debtCredit;
                    double debtDebit;
                    double paymentSumCredit;
                    double paymentSumDebit;
                    QString reportNumber;
                    QJsonObject recordObject;
                    QJsonArray recordsArray;

                    while (row <= numberOfRow) {
                        cell = dataSource.read(row, 1);
                        dataSend = toDateTimeAsString(cell, retVal);
                        if (!retVal) {
                            consoleOut.outToConsole(QStringLiteral("Error at line %1: The 'dataSend' value '%2' is wrong!").arg(row).arg(dataSend));
                            break;
                        }

                        cell = dataSource.read(row, 2);
                        debtCredit = toDouble(cell, retVal);
                        if (!retVal) {
                            consoleOut.outToConsole(QStringLiteral("Error at line %1: The 'debtCredit' value '%2' is wrong!").arg(row).arg(debtCredit));
                            break;
                        }

                        cell = dataSource.read(row, 3);
                        debtDebit = toDouble(cell, retVal);
                        if (!retVal) {
                            consoleOut.outToConsole(QStringLiteral("Error at line %1: The 'debtDebit' value '%2' is wrong!").arg(row).arg(debtDebit));
                            break;
                        }

                        cell = dataSource.read(row, 4);
                        paymentSumCredit = toDouble(cell, retVal);
                        if (!retVal) {
                            consoleOut.outToConsole(QStringLiteral("Error at line %1: The 'paymentSumCredit' value '%2' is wrong!").arg(row).arg(paymentSumCredit));
                            break;
                        }

                        cell = dataSource.read(row, 5);
                        paymentSumDebit = toDouble(cell, retVal);
                        if (!retVal) {
                            consoleOut.outToConsole(QStringLiteral("Error at line %1: The 'paymentSumDebit' value '%2' is wrong!").arg(row).arg(paymentSumDebit));
                            break;
                        }

                        cell = dataSource.read(row, 6);
                        reportNumber = toReportNumber(cell, retVal);
                        if (!retVal) {
                            consoleOut.outToConsole(QStringLiteral("Error at line %1: The 'reportNumber' value '%2' is wrong!").arg(row).arg(reportNumber));
                            break;
                        }

                        recordObject = QJsonObject();
                        recordObject.insert("dataSend", QJsonValue::fromVariant(dataSend));
                        recordObject.insert("debtCredit", QJsonValue::fromVariant(debtCredit));
                        recordObject.insert("debtDebit", QJsonValue::fromVariant(debtDebit));
                        recordObject.insert("paymentSumCredit", QJsonValue::fromVariant(paymentSumCredit));
                        recordObject.insert("paymentSumDebit", QJsonValue::fromVariant(paymentSumDebit));
                        recordObject.insert("reportNumber", QJsonValue::fromVariant(reportNumber));

                        recordsArray.push_back(recordObject);

                        ++row;
                    } // while

                    if (retVal) {
                        QJsonDocument jsonResult(recordsArray);
                        fileName = cmd.getReportName();
                        QFile jsonFile(fileName);
                        if (jsonFile.open(QIODevice::WriteOnly)) {
                            OutputMode mode = cmd.getOutputMode();
                            if (mode == OutputMode::OUTPUTMODE_INDENTED) {
                                retVal = jsonFile.write(jsonResult.toJson(QJsonDocument::Indented));
                            } else {
                                retVal = jsonFile.write(jsonResult.toJson(QJsonDocument::Compact));
                            }
                            jsonFile.close();
                        }
                        if (retVal) {
                            consoleOut.outToConsole(QStringLiteral("\nTotal rows converted: %1.").arg(row - 1));
                            consoleOut.outToConsole(QStringLiteral("The JSON was saved in the %1 file.").arg(fileName));
                        }
                    }
                }
            } else {
                consoleOut.outToConsole(QLatin1String("The file %1 header is wrong.").arg(fileName));
            }
        } else {
            consoleOut.outToConsole(QLatin1String("Wrong xlsx file %1.").arg(fileName));
            retVal = false;
        }
    } else {
        consoleOut.outToConsole(QLatin1String("Error load xlsx file %1.").arg(fileName));
    }

    return retVal ? 0 : 1;
}
