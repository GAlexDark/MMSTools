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
