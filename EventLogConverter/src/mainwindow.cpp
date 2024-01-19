
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>

#include "CELCWSettings.h"
//#include "Debug.h"
#include "OptionsDialog.h"
#include "CSVLoader.h"
#include "CReportBuilder.h"
#include "elcUtils.h"

using pragmaList_t = QMap<QString, QString>;

void
MainWindow::setStateText(const QString &state)
{
    m_state->setText(state);
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
MainWindow::showOptionsDialog( QStringList &includeUsersList, QStringList &excludeUsersList)
{
    bool retVal = true;
    OptionsDialog *wnd;
    try {
        wnd = new OptionsDialog(this);
        wnd->exec();

        includeUsersList = wnd->getIncludeUsersList();
        excludeUsersList = wnd->getExcludeUsersList();

    } catch (const std::bad_alloc &e) {
        retVal = false;
        QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("Critical error: %1").arg(e.what()), QMessageBox::Ok);
    }
    delete wnd;
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

    const ELCWSettings &settings = ELCWSettings::instance();
    m_dbName =  QDir::fromNativeSeparators(settings.getMain("SETTINGS/db_file_name").toString().trimmed());
    m_lastDir = settings.getMain("HISTORY/last_dir").toString().trimmed();

    setStateText(tr("Ready"));

    m_fileList.clear();
    m_hasHeaders = true;
}

MainWindow::~MainWindow()
{
    if (!m_lastDir.isEmpty() && QDir(m_lastDir).exists()) {
        ELCWSettings &settings = ELCWSettings::instance();
        settings.setMain("HISTORY", "last_dir", m_lastDir);
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
    QMessageBox dlg(QMessageBox::Information, tr("Contacts - MMS Event Log Conversion Utility"),
                    tr("<p>The MMS Event Log Conversion Utility developers can be reached at the mail:</p>"
                           "%1"
                           "<p>Please use %2 for bigger chunks of text.</p>")
                        .arg(QStringLiteral("<p>&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"mailto:%1\">%1</a></p>").arg(CONTACT))
                        .arg("<a href=\"https://pastebin.com\">"
                             "https://pastebin.com"
                             "</a>"),
                    QMessageBox::Ok);
    dlg.exec();
}

void
MainWindow::openFileClick()
{
    m_fileList.clear();

    if (m_lastDir.isEmpty() || !QDir(m_lastDir).exists()) {
        m_lastDir.clear();
    }

    m_fileList = QFileDialog::getOpenFileNames(this, tr("Open MMS Event Log file"), m_lastDir, tr("MMS Eventlog (*.csv)"));
    // The inferior stopped because it triggered an eaxception.
    // ref: https://forum.qt.io/topic/142647/the-inferior-stopped-because-it-triggered-an-eaxception/2

    if (!m_fileList.isEmpty()) {
        m_lastDir = QFileInfo(m_fileList.at(0)).absolutePath();
        setInfoText(tr("The selected file(s): "));
        for (int i = 0; i < m_fileList.size(); ++i) {
            setInfoText(m_fileList.at(i));
        }
        setStateText(tr("The file(s) was selected"));
        QCoreApplication::processEvents();

        // ref: https://github.com/mu-editor/mu/issues/832
        QMessageBox messageBox = QMessageBox(this);
        messageBox.setText(tr("Do the data file(s) have headers?"));
        messageBox.setWindowTitle(tr("MMS Event Log Conversion Utility"));
        messageBox.setInformativeText(tr("Yes - The data file(s) has headers,\nNo - Otherwise."));
        messageBox.setIcon(QMessageBox::Question);
        messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        messageBox.setDefaultButton(QMessageBox::Yes);
        messageBox.button(QMessageBox::Yes)->setText(tr("Yes"));
        messageBox.button(QMessageBox::No)->setText(tr("No"));
        m_hasHeaders = (messageBox.exec() == 0x4000);
        /*
        Yes - 16348 - 0x04000 - 0000 0100 0000 0000 0000
         No - 65536 - 0x10000 - 0001 0000 0000 0000 0000
        */
        if (m_hasHeaders) {
            setInfoText(tr("The data file(s) has headers"));
        } else {
            setInfoText(tr("The data file(s) has no headers"));
        }
    } else {
        QString buf = tr("No files selected");
        setInfoText(buf);
        setStateText(buf);
    }
}

void
MainWindow::convertEventLogClick()
{
    disableButtons();

    if (!m_fileList.isEmpty()) {
        CSVThreadLoader loader;
        bool retVal = QObject::connect(&loader, SIGNAL(sendMessage(QString)), this, SLOT(setInfoText(QString)));
        Q_ASSERT_X(retVal, "connect", "connection is not established");

        const ELCWSettings &settings = ELCWSettings::instance();
        QString internalipFirstOctet = settings.getMain("SETTINGS/internal_ip_start_octet").toString().trimmed();

        pragmaList_t pragmaList;
        QString value = settings.getMain("DATABASE/synchronous").toString().trimmed();
        if (!value.isEmpty()) {
            pragmaList["synchronous"] = value;
        }
        value = settings.getMain("DATABASE/journal_mode").toString().trimmed();
        if (!value.isEmpty()) {
            pragmaList["journal_mode"] = value;
        }
        value = settings.getMain("DATABASE/temp_store").toString().trimmed();
        if (!value.isEmpty()) {
            pragmaList["temp_store"] = value;
        }
        value = settings.getMain("DATABASE/locking_mode").toString().trimmed();
        if (!value.isEmpty()) {
            pragmaList["locking_mode"] = value;
        }

        setInfoText(tr("Start reading and converting file(s)..."));
        setStateText(tr("Read and converting"));
        QCoreApplication::processEvents();

        retVal = loader.init(m_dbName, m_hasHeaders, internalipFirstOctet, pragmaList, "\r\n");
        if (retVal) {
            loader.setFileName(m_fileList);
            loader.start();

            setInfoText(tr("wait..."));
            elcUtils::waitForEndThread(&loader, 100);
            retVal = loader.getStatus();
        }
        QCoreApplication::processEvents();
        QObject::disconnect(&loader, SIGNAL(sendMessage(QString)), this, SLOT(setInfoText(QString)));

        if (retVal) {
            setInfoText(tr("Reading file(s) completed"));
            setStateText(tr("Reading complete"));
        } else {
            setInfoText(tr("Error reading file(s): %1").arg(loader.errorString()));
            setStateText(tr("Error reading"));
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
    if (elcUtils::trunvateDB(m_dbName, errorString)) {
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
    if (showOptionsDialog(includedUsers, excludedUsers)) {
        setInfoText(tr("Additional report filtering settings:"));
        if (includedUsers.isEmpty()) {
            setInfoText(tr("\tThe included users list is empty."));
        } else {
            setInfoText("\t" + includedUsers.join(','));
        }
        if (excludedUsers.isEmpty()) {
            setInfoText(tr("\tThe excluded users list is empty."));
        } else {
            setInfoText("\t" + excludedUsers.join(','));
        }
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
            if (report.init(m_dbName, reportName, excludedUsers, includedUsers)) {
                report.start();

                setInfoText(tr("wait..."));
                elcUtils::waitForEndThread(&report, 100);
                retVal = report.getStatus();
            }
            QCoreApplication::processEvents();
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
    }

    enableButtons();
}
