/****************************************************************************
*
* The basic class for the input arguments parsing of the console utilities.
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

#ifndef CBASICCMDLINEPARSER_H
#define CBASICCMDLINEPARSER_H

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QString>

class CBasicCmdLineParser
{
public:
    explicit CBasicCmdLineParser();
    bool parseCmdArgs(const QCoreApplication &app);
    [[noreturn]] void showHelpAndExit();
    const QString& errorString() const { return m_errorString; }

private:
    virtual bool addOption(const QCoreApplication &app) = 0;
    virtual bool checkOption() = 0;

    QString m_errorString;

protected:
    void setErrorString(const QString &errorString) { m_errorString = errorString; }

    QCommandLineParser  m_parser;
};

#endif // CBASICCMDLINEPARSER_H
