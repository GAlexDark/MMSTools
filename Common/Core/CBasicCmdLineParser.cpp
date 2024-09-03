/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Common module
*  Common module
*
*  Module name: CBasicCmdLineParser.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     Basic Class for parsing input arguments of console utilities.
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
