/****************************************************************************
*
* The p7b file maker Console Utility input arguments parsing class.
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

#ifndef CP7BMAKERCMDLINEPARSER_H
#define CP7BMAKERCMDLINEPARSER_H

#include <QString>

#include "CBasicCmdLineParser.h"

class CP7bMakerCmdLineParser : public CBasicCmdLineParser
{
public:
    bool getP7bFileName(QStringList &p7bFileName);
    bool getCertsList(QStringList &certsList);
    bool isSilent() const { return m_isSilent; };
    QString getOutputFormat() const;

private:
    bool addOption(const QCoreApplication &app) override;
    bool checkOption() override;

    bool m_isStore = false;
    bool m_isSearchFolderExists = false;
    bool m_isSilent = false;
    bool m_isOutputFormat = false;
    QString m_searchFolder;
};

#endif // CP7BMAKERCMDLINEPARSER_H
