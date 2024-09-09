/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  p7b file maker Console Utility
*  p7b file maker Console Utility
*
*  Module name: CP7bMakerCmdLineParser.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The console utility input arguments parsing Class.
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

private:
    bool addOption(const QCoreApplication &app) override;
    bool checkOption() override;

    bool m_isStore = false;
    bool m_isSearchFolderExists = false;
    QString m_searchFolder;
};

#endif // CP7BMAKERCMDLINEPARSER_H
