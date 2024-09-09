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

#include "CPkcs7.h"
#include "CConsoleOutput.h"
#include "elcUtils.h"
#include "CP7bMakerCmdLineParser.h"


QString
getP7bResultFileName(const QString &fileName)
{
    QString baseName = QFileInfo(fileName).baseName(); // filename (wo ext)
    QString path = QFileInfo(fileName).path();
    QDate now = QDate::currentDate();
    QString retVal = QStringLiteral("%1/%2%3.p7b").arg(path, baseName, now.toString(QLatin1String("yyyyMMdd")));
    return retVal.replace("//", "/");
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationVersion(QStringLiteral("%1 tag %2 %3").arg(BUILD_VER, BUILD_GIT, elcUtils::getFormattedDateTime( BUILD_DATE )));

    CConsoleOutput consoleOut;
    QString description(QStringLiteral("p7b file maker Utility Version %1\nCopyright (C) 2024 Oleksii Gaienko, %3\n\n"));
    description.append(QStringLiteral("This program use the Qt version %2, OpenSSL version 3.3.1 and use the BioByteArray Class from the XCA Project: https://github.com/chris2511/xca.\n"));
    consoleOut.outToConsole(description.arg(QCoreApplication::applicationVersion(), QT_VER, CONTACT));

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

    consoleOut.outToConsole(QLatin1String("p7b file maker Utility starting...\n"));

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
    retVal = p7bStore.saveStore(dest);
    if (retVal) {
        consoleOut.outToConsole(QStringLiteral("File '%1' saved successfully.\n").arg(dest));
    } else {
        consoleOut.outToConsole(QStringLiteral("Error save p7b file: %1.").arg(p7bStore.errorString()));
        return 1;
    }

    consoleOut.outToConsole("Calculate result p7b file hash:");
    QByteArray hash = CPkcs7::getHash(dest).toHex();
    if (!hash.isEmpty()) {
        consoleOut.outToConsole(QStringLiteral("SHA-1 hash: %1.\n").arg(hash));
    } else {
        consoleOut.outToConsole(QStringLiteral("Error calculate hash."));
        return 1;
    }

    consoleOut.outToConsole(QStringLiteral("Create hash File."));
    QString errorString;
    retVal = CPkcs7::createHashFile(dest, errorString);
    if (retVal) {
        consoleOut.outToConsole(QStringLiteral("The hash file '%1' saved successfully.\n").arg(dest));
    } else {
        consoleOut.outToConsole(errorString);
    }

    consoleOut.outToConsole("Done");

    return retVal ? 0 : 1;
}
