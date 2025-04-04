/****************************************************************************
*
* The Event Log Conversion GUI Utility Main file.
* Copyright (C) 2023-2024  Oleksii Gaienko
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QScopedPointer>
#include <QFileInfo>
#include <QObject>

#include "CElcGuiAppSettings.h"
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
    m_errorString.clear();
    QScopedPointer<CReportOptionsDialog> wnd(new CReportOptionsDialog(logID, logsList, this));
    if (wnd) {
        wnd->exec();
        return wnd->getOptions(logID, includeUsersList, excludeUsersList);
    } else {
        m_errorString = tr("Unable to open advanced report filtering settings window.");
        QMessageBox::critical(nullptr, tr("Error"), m_errorString, QMessageBox::Ok);
        return false;
    }
}

bool
MainWindow::showReadFilesOptionsDialog(const QStringList &logsList, quint16 &logID, bool &hasHeaders)
{
    m_errorString.clear();
    QScopedPointer<CLoadFilesOptionsDialog> wnd(new CLoadFilesOptionsDialog(logsList, this));
    if (wnd) {
        wnd->exec();
        return wnd->getOptions(logID, hasHeaders);
    } else {
        m_errorString = tr("Unable to open the advanced file reading settings window.");
        QMessageBox::critical(nullptr, tr("Error"), m_errorString, QMessageBox::Ok);
        return false;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    const CElcGuiAppSettings &settings = CElcGuiAppSettings::instance();
#ifdef Q_OS_WIN
    QList<QScreen *> monitorList = QGuiApplication::screens();
    qsizetype monitorNumber = settings.getDefaultMonitor();
    if (monitorNumber > monitorList.size()) {
        monitorNumber = 0;
    }
    if (monitorNumber > 0) {
        move(monitorList[monitorNumber - 1]->geometry().center() - frameGeometry().center());
    }
#endif
    setWindowIcon(QIcon(":/img/data-transformation.png"));

    m_state.reset(new QLabel(this));
    m_state->setMinimumWidth(250);
    ui->statusbar->addWidget(m_state.data());

    m_mode.reset(new QLabel(this));
    m_mode->setMinimumWidth(250);
    ui->statusbar->addWidget(m_mode.data());

    bool retVal = connect(ui->actionAbout_program, &QAction::triggered, this, &MainWindow::onAboutProgram);
    Q_ASSERT_X(retVal, "connect", "actionAbout_program connection is not established");
    retVal = connect(ui->actionAbout_Qt, &QAction::triggered, this, &MainWindow::onAboutQt);
    Q_ASSERT_X(retVal, "connect", "actionAbout_Qt connection is not established");
    retVal = connect(ui->actionContact, &QAction::triggered, this, &MainWindow::onContact);
    Q_ASSERT_X(retVal, "connect", "actionContact connection is not established");

    retVal = connect(ui->pbOpenFile, &QPushButton::clicked, this, &MainWindow::openFileClick);
    Q_ASSERT_X(retVal, "connect", "pbOpenFile connection is not established");
    retVal = connect(ui->pbConvert, &QPushButton::clicked, this, &MainWindow::convertEventLogClick);
    Q_ASSERT_X(retVal, "connect", "pbConvert connection is not established");
    retVal = connect(ui->pbClearDB, &QPushButton::clicked, this, &MainWindow::clearDBclick);
    Q_ASSERT_X(retVal, "connect", "pbClearDB connection is not established");
    retVal = connect(ui->pbGenerateReport, &QPushButton::clicked, this, &MainWindow::generateReportClick);
    Q_ASSERT_X(retVal, "connect", "pbGenerateReport connection is not established");

    m_dbName = settings.getDbFileName();
    elcUtils::expandEnvironmentStrings(m_dbName);
    m_lastDir = settings.getLastDir();

    setStateText(tr("Ready"));

    m_fileList.clear();
#ifdef Q_OS_WIN
    QString rds = settings.isRdsEnabled() ? QLatin1String("RDP mode") : QLatin1String("Single mode");
    QString rdp = QLatin1String("%SESSIONNAME%");
    elcUtils::expandEnvironmentStrings(rdp);
    if (rdp.startsWith('%')) {
        rdp = QLatin1String("WinAPI error");
    }
    setModeText(QStringLiteral("%1 | %2").arg(rds, rdp));
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

    delete ui;
}

void
MainWindow::onAboutProgram()
{
    QMessageBox::about(this, tr("About MMS Event Log Conversion Utility"),
                       tr("MMS Event Log Conversion Utility\nVersion %1\nCopyright (C) 2023 Oleksii Gaienko\nThis program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.\n\nThis program use QXlsx library:\n https://github.com/QtExcel/QXlsx.").arg(QCoreApplication::applicationVersion()));
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

    if (m_fileList.isEmpty()) {
        QString buf = tr("No files selected");
        setInfoText(buf);
        setStateText(buf);
        enableButtons();
        return;
    }

    m_lastDir = QFileInfo(m_fileList.at(0)).absolutePath();
    setInfoText(tr("The selected file(s): "));
    int i = 0;
    QString buf;
    while (i < m_fileList.size()) {
        buf = m_fileList.at(i);
        if (QFileInfo(buf).size() == 0) {
            setInfoText(tr("%1 - this file have zero size and will be removed from the list.").arg(buf));
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
    } else if (!m_errorString.isEmpty()) {
        setInfoText(tr("Error opening options dialog."));
        setInfoText(m_errorString);
        setStateText(tr("Error"));
    } else {
        buf = tr("File selection canceled.");
        setInfoText(buf);
        setStateText(buf);
        m_fileList.clear();
    }

    enableButtons();
}

void
MainWindow::convertEventLogClick()
{
    disableButtons();

    if (m_fileList.isEmpty()) {
        QString buf = tr("No files selected");
        setInfoText(buf);
        setStateText(buf);
        enableButtons();
        return;
    }

    CMmsLogsThreadReader logReader;
    bool retVal = QObject::connect(&logReader, &CMmsLogsThreadReader::sendMessage, this, &MainWindow::setInfoText);
    Q_ASSERT_X(retVal, "connect", "connection is not established");

    const CElcGuiAppSettings &settings = CElcGuiAppSettings::instance();
    QString internalIpFirstOctet = settings.getInternalIpStartOctet();
    if (!internalIpFirstOctet.isEmpty()) {
        QMap<QString, QString> pragmaList;
        pragmaList[QLatin1String("synchronous")] = settings.getSynchronousType();
        pragmaList[QLatin1String("journal_mode")] = settings.getJournalModeType();
        pragmaList[QLatin1String("temp_store")] = settings.getTempStore();
        pragmaList[QLatin1String("locking_mode")] = settings.getLockingMode();

        setInfoText(tr("Start reading and converting file(s)..."));
        setStateText(tr("Reading and converting"));
        QCoreApplication::processEvents();

        retVal = logReader.init(m_logId, m_dbName, m_hasHeaders, internalIpFirstOctet, &pragmaList);
        if (retVal) {
            logReader.setFileNames(m_fileList);
            logReader.start();

            setInfoText(tr("wait..."));
            elcUtils::waitForEndThread(&logReader, 100);
            retVal = logReader.getStatus();
        }
        QCoreApplication::processEvents();
        QObject::disconnect(&logReader, &CMmsLogsThreadReader::sendMessage, this, &MainWindow::setInfoText);

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
    QStringList tables = parserManager.getTablesList();
    QStringList updateData = parserManager.getCreateTableRequestList();
    if (CSqliteDatabase::truncateDB(m_dbName, errorString, tables, updateData)) {
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

    QStringList includedUsers;
    QStringList excludedUsers;
    const CReportManager &reportManager = CReportManager::instance();
    QStringList reportsList = reportManager.getVisibleReportsNames();
    if (!reportManager.checkID(m_logId)) {
        m_logId = 0;
    }
    if (!showReportOptionsDialog(reportsList, m_logId, includedUsers, excludedUsers)) {
        if (!m_errorString.isEmpty()) {
            setInfoText(tr("Error opening options dialog."));
            setInfoText(m_errorString);
            setStateText(tr("Error"));
        } else {
            setInfoText(tr("Report generation canceled."));
        }
        enableButtons();
        return;
    }

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
    setInfoText(tr("Selected report type: %1").arg(reportsList.at(m_logId - 1)));
    QCoreApplication::processEvents();

    QString reportName = QFileDialog::getSaveFileName(this, tr("Save MMS Event Log report"),
                                                      m_lastDir,
                                                      tr("Excel (*.xlsx)"));
    if (reportName.isEmpty()) {
        setInfoText(tr("Report name is empty"));
        setStateText(tr("Ready"));
        enableButtons();
        return;
    }

    QString path = QFileInfo(reportName).absolutePath();
    bool retVal = elcUtils::isFolderWritable(path);
    if (retVal) {
        setInfoText(tr("The report will be created here: %1").arg(reportName));

        setInfoText(tr("Generating report..."));
        setStateText(tr("Generating report"));
        QCoreApplication::processEvents();

        const CElcGuiAppSettings &settings = CElcGuiAppSettings::instance();
        bool showMilliseconds = settings.getShowMilliseconds();
        CSVThreadReportBuilder report;
        retVal = report.init(m_logId, m_dbName, reportName, &excludedUsers, &includedUsers, showMilliseconds);
        if (retVal) {
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
            setInfoText(tr("Error generating report: %1").arg(report.errorString()));
            setStateText(tr("Error"));
        }
    } else {
        setInfoText(tr("You don't have write permissions to this folder: %1").arg(path));
        setStateText(tr("Warning"));
    }

    enableButtons();
}
