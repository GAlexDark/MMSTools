/****************************************************************************
*
* The p7b file maker Console Utility Main file.
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

#include <QCoreApplication>
#include <QFileInfo>
#include <QDate>
#include <QDir>

#include "CPkcs7.h"
#include "CConsoleOutput.h"
#include "elcUtils.h"
#include "CP7bMakerCmdLineParser.h"


QString
getP7bResultFileName(const QString &fileName)
{
    QString baseName = QFileInfo(fileName).completeBaseName(); // filename (wo ext)
    QString path = QFileInfo(fileName).path();
    QDate now = QDate::currentDate();
    QString p7bFileName = QStringLiteral("%1%2.p7b").arg(baseName, now.toString(QLatin1String("yyyyMMdd")));
    return QDir(path).filePath(p7bFileName);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationVersion(QStringLiteral("%1 tag %2 %3").arg(BUILD_VER, BUILD_GIT, elcUtils::getFormattedDateTime( BUILD_DATE )));

    CConsoleOutput consoleOut;
    CP7bMakerCmdLineParser cmd;
    if (!cmd.parseCmdArgs(a)) {
        consoleOut.outToConsole(cmd.errorString());
        return 1;
    }
    if (argc == 1) {
        consoleOut.outToConsole(QLatin1String("The arguments are missing."));
        cmd.showHelpAndExit();
        return 0;
    }

    if (!cmd.isSilent()) {
        QString description(QStringLiteral("p7b file maker Utility Version %1\nCopyright (C) 2024 Oleksii Gaienko, %3\nThis program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.\n\n"));
        description.append(QStringLiteral("This program use the Qt version %2, OpenSSL version 3.3.1.\n"));
        consoleOut.outToConsole(description.arg(QCoreApplication::applicationVersion(), QT_VER, CONTACT));
        consoleOut.outToConsole(QLatin1String("p7b file maker Utility starting...\n"));
    }

    QStringList p7bFiles;
    if (!cmd.getP7bFileName(p7bFiles)) {
        consoleOut.outToConsole(cmd.errorString());
        return 1;
    }
    if (p7bFiles.size() != 1) {
        consoleOut.outToConsole(QStringLiteral("The file p7b not found or multiple p7b files found."));
        return 1;
    }
    const QString source = p7bFiles.at(0);
    CPkcs7 p7bStore;
    consoleOut.outToConsole(QStringLiteral("Start reading p7b file '%1'.").arg(source));
    bool retVal = p7bStore.readStore(source);
    if (retVal) {
        consoleOut.outToConsole(QStringLiteral("The p7b file '%1' was read successful.\n").arg(source));
    } else {
        consoleOut.outToConsole(QStringLiteral("Error read p7b file: %1.").arg(p7bStore.errorString()));
        return 1;
    }
    consoleOut.outToConsole(QStringLiteral("The number of certificates in the container before adding: %1.\n").arg(p7bStore.size()));

    QStringList certsList;
    if (!cmd.getCertsList(certsList)) {
        consoleOut.outToConsole(cmd.errorString());
        return 1;
    }
    if (certsList.isEmpty()) {
        consoleOut.outToConsole(QStringLiteral("No certificates found to add."));
        return 1;
    }
    consoleOut.outToConsole(QStringLiteral("The %1 certificates found.").arg(QString::number(certsList.size())));
    consoleOut.outToConsole(QStringLiteral("Start adding certificates."));
    retVal = p7bStore.appendCerts(certsList);
    if (retVal) {
        consoleOut.outToConsole("The certificates were added successful.\n");
    } else {
        consoleOut.outToConsole(QStringLiteral("Error adding certs to p7b store: %1.").arg(p7bStore.errorString()));
        return 1;
    }
    consoleOut.outToConsole(QStringLiteral("The number of certificates in the container after adding: %1.\n").arg(p7bStore.size()));

    QString dest = getP7bResultFileName(source);
    QString path = QFileInfo(dest).absolutePath();
    retVal = elcUtils::isFolderWritable(path);
    if (retVal) {
        QString outputFormat = cmd.getOutputFormat();
        consoleOut.outToConsole(QStringLiteral("Output format: %1").arg(outputFormat));
        OUTPUT_FORMAT of = outputFormat == "ASN1" ? OUTPUT_FORMAT::FORMAT_ASN1 : OUTPUT_FORMAT::FORMAT_PEM;
        retVal = p7bStore.saveStore(dest, of);
        if (retVal) {
            consoleOut.outToConsole(QStringLiteral("File '%1' saved successfully.\n").arg(dest));
        } else {
            consoleOut.outToConsole(QStringLiteral("Error save p7b file: %1.").arg(p7bStore.errorString()));
            return 1;
        }

        consoleOut.outToConsole("Calculating result p7b file hash:");
        QByteArray hash = CPkcs7::getHash(dest).toHex();
        if (!hash.isEmpty()) {
            consoleOut.outToConsole(QStringLiteral("SHA-1 hash: %1.\n").arg(hash));
        } else {
            consoleOut.outToConsole(QStringLiteral("Error calculate hash."));
            return 1;
        }

        consoleOut.outToConsole(QStringLiteral("Creating hash File."));
        QString errorString;
        retVal = CPkcs7::createHashFile(dest, errorString);
        if (retVal) {
            QString buf = QStringLiteral("The hash file '%1' saved successfully.\n").arg(dest);
            int lastIndex = buf.lastIndexOf("p7b");
            if (lastIndex != -1) {
                buf.replace(lastIndex, 3, "sha");
            }
            consoleOut.outToConsole(buf);
        } else {
            consoleOut.outToConsole(errorString);
        }
        consoleOut.outToConsole("DONE");
    } else {
        consoleOut.outToConsole(QStringLiteral("You don't have write permissions to this folder: %1").arg(path));
        return 1;
    }
    return retVal ? 0 : 1;
}
