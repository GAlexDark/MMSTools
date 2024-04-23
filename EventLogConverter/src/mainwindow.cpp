/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2023-2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion GUI Utility
*
*  Module name: mainwindow.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     Main file
*
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include "CElcGuiAppSettings.h"
#include "MMSTypes.h"
#include "CReportOptionsDialog.h"
#include "CLoadFilesOptionsDialog.h"
#include "CSVReader.h"
#include "CReportBuilder.h"
#include "elcUtils.h"
#include "CParserManager.h"
#include "CReportManager.h"

void
MainWindow::setStateText(const QString &state)
{
    m_state->setText(state);
}

void
MainWindow::setModeText(const QString &mode)
{
    m_mode->setText(mode);
}

void
MainWindow::setInfoText(const QString &info)
{
    ui->plainTextEdit->appendPlainText(info);
}

void
MainWindow::disableButtons()
{
    if (!m_isButtonsDisabled) {
        ui->pbOpenFile->setEnabled(false);
        ui->pbConvert->setEnabled(false);
        ui->pbClearDB->setEnabled(false);
        ui->pbGenerateReport->setEnabled(false);
        m_isButtonsDisabled = true;
    }
}

void
MainWindow::enableButtons()
{
    if (m_isButtonsDisabled) {
        ui->pbOpenFile->setEnabled(true);
        ui->pbConvert->setEnabled(true);
        ui->pbClearDB->setEnabled(true);
        ui->pbGenerateReport->setEnabled(true);
        m_isButtonsDisabled = false;
    }
}

bool
MainWindow::showReportOptionsDialog(const QStringList &logsList, quint16 &logID, QStringList &includeUsersList, QStringList &excludeUsersList)
{
    bool retVal = true;
    m_errorString.clear();
    try {
        CReportOptionsDialog *wnd = new CReportOptionsDialog(logID, logsList, this);
        wnd->exec();
        retVal = wnd->getOptions(logID, includeUsersList, excludeUsersList);
        delete wnd;
    } catch (const std::bad_alloc &e) {
        retVal = false;
        m_errorString = QObject::tr("Critical error: %1").arg(e.what());
        QMessageBox::critical(nullptr, QObject::tr("Error"), m_errorString, QMessageBox::Ok);
    }

    return retVal;
}

bool
MainWindow::showReadFilesOptionsDialog(const QStringList &logsList, quint16 &logID, bool &hasHeaders)
{
    bool retVal = true;
    m_errorString.clear();
    try {
        CLoadFilesOptionsDialog *wnd = new CLoadFilesOptionsDialog(logsList, this);
        wnd->exec();
        retVal = wnd->getOptions(logID, hasHeaders);
        delete wnd;
    } catch (const std::bad_alloc &e) {
        retVal = false;
        m_errorString = QObject::tr("Critical error: %1").arg(e.what());
        QMessageBox::critical(nullptr, QObject::tr("Error"), m_errorString, QMessageBox::Ok);
    }

    return retVal;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/data-transformation.png"));
    m_isButtonsDisabled = false;

    m_state = new QLabel(this);
    m_state->setMinimumWidth(250);
    ui->statusbar->addWidget(m_state);

    m_mode = new QLabel(this);
    m_mode->setMinimumWidth(250);
    ui->statusbar->addWidget(m_mode);

    bool retVal = connect(ui->actionAbout_program, SIGNAL(triggered(bool)), this, SLOT(onAboutProgram()));
    Q_ASSERT_X(retVal, "connect", "actionAbout_program connection is not established");
    retVal = connect(ui->actionAbout_Qt, SIGNAL(triggered(bool)), this, SLOT(onAboutQt()));
    Q_ASSERT_X(retVal, "connect", "actionAbout_Qt connection is not established");
    retVal = connect(ui->actionContact, SIGNAL(triggered(bool)), this, SLOT(onContact()));
    Q_ASSERT_X(retVal, "connect", "actionContact connection is not established");

    retVal = connect(ui->pbOpenFile, SIGNAL(clicked(bool)), this, SLOT(openFileClick()));
    Q_ASSERT_X(retVal, "connect", "pbOpenFile connection is not established");
    retVal = connect(ui->pbConvert, SIGNAL(clicked(bool)), this, SLOT(convertEventLogClick()));
    Q_ASSERT_X(retVal, "connect", "pbConvert connection is not established");
    retVal = connect(ui->pbClearDB, SIGNAL(clicked(bool)), this, SLOT(clearDBclick()));
    Q_ASSERT_X(retVal, "connect", "pbClearDB connection is not established");
    retVal = connect(ui->pbGenerateReport, SIGNAL(clicked(bool)), this, SLOT(generateReportClick()));
    Q_ASSERT_X(retVal, "connect", "pbGenerateReport connection is not established");
    
    const CElcGuiAppSettings &settings = CElcGuiAppSettings::instance();
    m_dbName =  QDir::fromNativeSeparators(settings.getMain(QLatin1String("SETTINGS/db_file_name")).toString().trimmed());
    elcUtils::expandEnvironmentStrings(m_dbName);
    m_lastDir = settings.getMain(QLatin1String("HISTORY/last_dir")).toString().trimmed();

    setStateText(tr("Ready"));

    m_fileList.clear();
    m_hasHeaders = true;
#ifdef Q_OS_WIN
    QString rds = settings.isRdsEnabled() ? QLatin1String("RDP mode") : QLatin1String("Single mode");
    QString rdp = QLatin1String("%SESSIONNAME%");
    elcUtils::expandEnvironmentStrings(rdp);
    if (rdp.startsWith('%')) {
        rdp = QLatin1String("WinAPI error");
    }
    setModeText(QLatin1String("%1 | %2").arg(rds, rdp));
#else
    setModeText(QLatin1String("Linux Console"));
#endif

    CReportManager::instance().init();
}

MainWindow::~MainWindow()
{
    if (!m_lastDir.isEmpty() && QDir(m_lastDir).exists()) {
        CElcGuiAppSettings &settings = CElcGuiAppSettings::instance();
        settings.setMain(QLatin1String("HISTORY"), QLatin1String("last_dir"), m_lastDir);
    }

    delete m_state;
    delete ui;
}

void
MainWindow::onAboutProgram()
{
    QMessageBox::about(this, tr("About MMS Event Log Conversion Utility"),
                       tr("MMS Event Log Conversion Utility\nVersion %1\nCopyright (C) 2023 Oleksii Gaienko\n\n \
This program uses QXlsx library:\n https://github.com/QtExcel/QXlsx.").arg(QCoreApplication::applicationVersion()));
}

void
MainWindow::onAboutQt()
{
    QMessageBox::aboutQt(this);
}

void
MainWindow::onContact()
{
    QString contacts = QStringLiteral("<p>&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"mailto:%1\">%1</a></p>").arg(CONTACT);
    QMessageBox dlg(QMessageBox::Information, tr("Contacts - MMS Event Log Conversion Utility"),
                    tr("<p>The MMS Event Log Conversion Utility developers can be reached at the mail:</p>"
                       "%1"
                       "<p>Please use %2 for bigger chunks of text.</p>")
                        .arg(contacts, "<a href=\"https://pastebin.com\">https://pastebin.com</a>"),
                    QMessageBox::Ok);
    dlg.exec();
}

void
MainWindow::openFileClick()
{
    disableButtons();

    m_fileList.clear();

    if (m_lastDir.isEmpty() || !QDir(m_lastDir).exists()) {
        m_lastDir.clear();
    }

    m_fileList = QFileDialog::getOpenFileNames(
                    this,
                    tr("Open MMS Event Log file"),
                    m_lastDir,
                    tr("MMS log files (*.csv)"));
    // The inferior stopped because it triggered an eaxception.
    // ref: https://forum.qt.io/topic/142647/the-inferior-stopped-because-it-triggered-an-eaxception/2

    if (!m_fileList.isEmpty()) {
        m_lastDir = QFileInfo(m_fileList.at(0)).absolutePath();
        setInfoText(tr("The selected file(s): "));
        int i = 0; QString buf;
        while (i < m_fileList.size()) {
            buf = m_fileList.at(i);
            if (QFileInfo(buf).size() == 0) {
                QString msg = tr("%1 - this file have zero size and will be removed from the list.").arg(buf);
                setInfoText(msg);
                m_fileList.removeAt(i);
            } else {
                setInfoText(buf);
                i++;
            }
        }

        setStateText(tr("The file(s) was selected"));
        QCoreApplication::processEvents();

        const CParserManager &parserManager = CParserManager::instance();
        QStringList logsList = parserManager.getVisibleLogsNames();
        if (showReadFilesOptionsDialog(logsList, m_logId, m_hasHeaders)) {
            if (m_hasHeaders) {
                setInfoText(tr("The data file(s) has headers"));
            } else {
                setInfoText(tr("The data file(s) has no headers"));
            }
            setInfoText(tr("Selected Log type: %1").arg(logsList.at(m_logId - 1)));
            if (!parserManager.checkID(m_logId)) {
                m_fileList.clear();
                setInfoText(tr("Unknown MMS Log type."));
                setStateText(tr("Error reading"));
            }
        } else {
            if (!m_errorString.isEmpty()) {
                setInfoText(tr("Error open options dialog."));
                setInfoText(m_errorString);
                setStateText(tr("Error"));
            } else {
                buf = tr("File selection canceled.");
                setInfoText(buf);
                setStateText(buf);
                m_fileList.clear();
            }
        }
    } else {
        QString buf = tr("No files selected");
        setInfoText(buf);
        setStateText(buf);
    }
    enableButtons();
}

void
MainWindow::convertEventLogClick()
{
    disableButtons();

    if (!m_fileList.isEmpty()) {
        CMmsLogsThreadReader logReader;
        bool retVal = QObject::connect(&logReader, SIGNAL(sendMessage(QString)), this, SLOT(setInfoText(QString)));
        Q_ASSERT_X(retVal, "connect", "connection is not established");
        
        const CElcGuiAppSettings &settings = CElcGuiAppSettings::instance();
        QString internalIpFirstOctet = settings.getMain(QLatin1String("SETTINGS/internal_ip_start_octet")).toString().trimmed();
        if (!internalIpFirstOctet.isEmpty() && elcUtils::sanitizeValue(QLatin1String("^([0-9.]+)$"), internalIpFirstOctet)) {
            mms::pragmaList_t pragmaList;
            QString value = settings.getMain(QLatin1String("DATABASE/synchronous")).toString().trimmed();
            pragmaList[QLatin1String("synchronous")] = elcUtils::sanitizeValue(value, elcUtils::plSynchronous, elcUtils::pvNormal);

            value = settings.getMain(QLatin1String("DATABASE/journal_mode")).toString().trimmed();
            pragmaList[QLatin1String("journal_mode")] = elcUtils::sanitizeValue(value, elcUtils::plJournalMode, elcUtils::pvMemory);

            value = settings.getMain(QLatin1String("DATABASE/temp_store")).toString().trimmed();
            pragmaList[QLatin1String("temp_store")] = elcUtils::sanitizeValue(value, elcUtils::plTempStore, elcUtils::pvMemory);

            value = settings.getMain(QLatin1String("DATABASE/locking_mode")).toString().trimmed();
            pragmaList[QLatin1String("locking_mode")] = elcUtils::sanitizeValue(value, elcUtils::plLockingMode, elcUtils::pvExclusive);

            setInfoText(tr("Start reading and converting file(s)..."));
            setStateText(tr("Read and converting"));
            QCoreApplication::processEvents();

            retVal = logReader.init(m_logId, m_dbName, m_hasHeaders, internalIpFirstOctet, &pragmaList);
            if (retVal) {
                logReader.setFileName(m_fileList);
                logReader.start();

                setInfoText(tr("wait..."));
                elcUtils::waitForEndThread(&logReader, 100);
                retVal = logReader.getStatus();
            }
            QCoreApplication::processEvents();
            QObject::disconnect(&logReader, SIGNAL(sendMessage(QString)), this, SLOT(setInfoText(QString)));

            if (retVal) {
                setInfoText(tr("Reading file(s) completed"));
                setStateText(tr("Reading complete"));
            } else {
                setInfoText(tr("Error reading file(s): %1").arg(logReader.errorString()));
                setStateText(tr("Error reading"));
            }
        } else {
            setInfoText(tr("Error in internal IP address mask. Please check it in the config file."));
        }
    } else {
        QString buf = tr("No files selected");
        setInfoText(buf);
        setStateText(buf);
    }

    enableButtons();
}

void
MainWindow::clearDBclick()
{
    disableButtons();

    setInfoText(tr("Starting cleaning database..."));
    QCoreApplication::processEvents();

    QString errorString;
    const CParserManager &parserManager = CParserManager::instance();
    qsizetype count = parserManager.getItemCount();
    QStringList tables = parserManager.getTablesList();
    QStringList creationStrings = parserManager.getCreateTableRequestList();
    if (elcUtils::trunvateDB(m_dbName, errorString, count, tables, creationStrings)) {
        setInfoText(tr("Database was cleared"));
        setStateText(tr("Ready"));
    } else {
        setInfoText(tr("Cannot clean database: %1").arg(errorString));
        setStateText(tr("Error"));
    }

    enableButtons();
}

void
MainWindow::generateReportClick()
{
    disableButtons();

    setInfoText(tr("Start generating the report..."));
    setStateText(tr("Start generating the report"));
    QCoreApplication::processEvents();

    QStringList includedUsers, excludedUsers;
    CReportManager &reportManager = CReportManager::instance();
    //QStringList logsList = {tr("Event Log"), tr("Audit Trail Log"), tr("(Experimental) Summary report") };
    QStringList reportsList = reportManager.getVisibleReportsNames();
    quint16 logId = reportManager.prettySelector(m_logId);
    if (showReportOptionsDialog(reportsList, logId, includedUsers, excludedUsers)) {
        setInfoText(tr("Additional report filtering settings:"));
        if (includedUsers.isEmpty()) {
            setInfoText(tr("\tThe included users list is empty."));
        } else {
            setInfoText(tr("\tThe included users: %1").arg(includedUsers.join(',')));
        }
        if (excludedUsers.isEmpty()) {
            setInfoText(tr("\tThe excluded users list is empty."));
        } else {
            setInfoText(tr("\tThe excluded users: %1").arg(excludedUsers.join(',')));
        }
        setInfoText(tr("Selected report type: %1").arg(reportsList.at(logId - 1)));
        QCoreApplication::processEvents();

        QString reportName = QFileDialog::getSaveFileName(this, tr("Save MMS Event Log report"),
                                                        m_lastDir,
                                                        tr("Excel (*.xlsx)"));
        if (!reportName.isEmpty()) {
            setInfoText(tr("The report will be created here: %1").arg(reportName));
            bool retVal = true;

            setInfoText(tr("Generating report..."));
            setStateText(tr("Generating report"));
            QCoreApplication::processEvents();

            CSVThreadReportBuilder report;
            if (report.init(logId, m_dbName, reportName, &excludedUsers, &includedUsers)) {
                report.start();

                setInfoText(tr("wait..."));
                elcUtils::waitForEndThread(&report, 100);
                retVal = report.getStatus();
            }
            QCoreApplication::processEvents();
            excludedUsers.clear();
            includedUsers.clear();
            if (retVal) {
                setInfoText(tr("Report generating finished.\nThe report was saved in the %1 file.").arg(reportName));
                setStateText(tr("Report created"));
            } else {
                setInfoText(tr("Error generate report: %1").arg(report.errorString()));
                setStateText(tr("Error"));
            }

        } else {
            setInfoText(tr("Report name is empty"));
            setStateText(tr("Ready"));
        }
    } else {
        if (!m_errorString.isEmpty()) {
            setInfoText(tr("Error open options dialog."));
            setInfoText(m_errorString);
            setStateText(tr("Error"));
        } else {
            setInfoText(tr("Report generation canceled."));
        }
    }

    enableButtons();
}
