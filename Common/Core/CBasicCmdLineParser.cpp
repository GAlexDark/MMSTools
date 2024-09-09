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

#include "CBasicCmdLineParser.h"

CBasicCmdLineParser::CBasicCmdLineParser()
{
    m_errorString.clear();
}

bool
CBasicCmdLineParser::parseCmdArgs(const QCoreApplication &app)
{
    m_parser.addHelpOption();

    bool retVal = addOption(app);
    if (retVal) {
        retVal = checkOption();
    }

    return retVal;
}

[[noreturn]] void
CBasicCmdLineParser::showHelpAndExit()
{
    m_parser.showHelp(0);
}
