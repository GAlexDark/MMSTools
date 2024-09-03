#include "CP7bMakerCmdLineParser.h"

#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QStringList>

#include "elcUtils.h"

bool
CP7bMakerCmdLineParser::addOption(const QCoreApplication &app)
{
    const QString storeDescription(QLatin1String("The folder path contains one *.p7b file and several *.cer, *.crt files to add."));

    QCommandLineOption storeOption(QStringList() << "l" << "localstore", storeDescription, "path");
    bool retVal = m_parser.addOption(storeOption);
    if (retVal) {
        m_parser.process(app);
        m_isStore = m_parser.isSet(storeOption);
    }
    if (!retVal) {
        setErrorString(QLatin1String("The fatal error has occurredd. The program will be closed."));
    }
    return retVal;
}

bool
CP7bMakerCmdLineParser::checkOption()
{
    if (!m_isStore) {
        setErrorString(QLatin1String("The <store> argument is missing."));
    }
    return m_isStore;
}

bool
CP7bMakerCmdLineParser::getP7bFileName(QStringList &p7bFileName)
{
    bool retVal = true;
    if (m_isStore) {
        m_searchFolder = m_parser.value("localstore");
        QFileInfo sf(m_searchFolder);
        QDir dir = sf.absoluteDir();
        if (dir.exists()) {
            m_isSearchFolderExists = true;
            m_searchFolder = dir.absolutePath(); //The QFileInfo class convert '\\', '//' into '/' in the filepath
            QString mask = QLatin1String("*.p7b");
            p7bFileName.append( elcUtils::getDataSourceList(m_searchFolder, QStringList() << mask) );
        } else {
            setErrorString(QStringLiteral("Cannot find the directory %1.").arg(m_searchFolder));
            retVal = false;
        }
    }

    if (retVal) {
        p7bFileName.removeDuplicates();
    }
    return retVal;
}

bool
CP7bMakerCmdLineParser::getCertsList(QStringList &certsList)
{
    bool retVal = true;
    if (m_isSearchFolderExists) {
        QStringList masks = { "*.cer", "*.crt" };
        for (const QString &maskItem : masks) {
            certsList.append( elcUtils::getDataSourceList(m_searchFolder, QStringList() << maskItem) );
        }
    } else {
        setErrorString(QStringLiteral("Cannot find the directory %1.").arg(m_searchFolder));
        retVal = false;
    }

    if (!retVal) {
        certsList.removeDuplicates();
    }
    return retVal;
}
