
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QChar>
#include <QUuid>

#include "ELCSettings.h"

#include "xlsxdocument.h"
//#include "xlsxchartsheet.h"
//#include "xlsxcellrange.h"
//#include "xlsxchart.h"
//#include "xlsxrichstring.h"
//#include "xlsxworkbook.h"
#include "xlsxformat.h"
using namespace QXlsx;

const QString createEventLogTable = QStringLiteral("CREATE TABLE IF NOT EXISTS [eventlog] (id INTEGER PRIMARY KEY ASC, username TEXT, timestampISO8601 TEXT, requestid TEXT, \
                                                   type TEXT, details TEXT, username1 TEXT, authtype TEXT, externalip TEXT, internalip TEXT, timestamp DATETIME);");

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

    connect(ui->actionAbout_program, SIGNAL(triggered(bool)), this, SLOT(onAboutProgram()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered(bool)), this, SLOT(onAboutQt()));

    connect(ui->pbOpenFile, SIGNAL(clicked(bool)), this, SLOT(openFileClick()));
    connect(ui->pbConvert, SIGNAL(clicked(bool)), this, SLOT(convertEventLogClick()));
    connect(ui->pbClearDB, SIGNAL(clicked(bool)), this, SLOT(clearDBclick()));
    connect(ui->pbGenerateReport, SIGNAL(clicked(bool)), this, SLOT(generateReportClick()));

    setStateText(tr("Ready"));

    //init DB
    m_ConnectionName = QUuid::createUuid().toString();
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_ConnectionName);
    if (!m_db.isValid()) {
        QSqlError error = m_db.lastError();
        m_dbErrorText = error.text();

        setInfoText(getDBErrorText());
        setStateText(tr("DB Error!"));
        return;
    }

    ELCSettings &settings = ELCSettings::instance();
    QString db_file_name = settings.getMain("SETTINGS/db_file_name").toString().trimmed();
    if (!openDB(db_file_name)) {
        setInfoText(getDBErrorText());
        setStateText(tr("DB Error!"));
        return;
    }
    QString mode = settings.getMain("SETTINGS/mode").toString().trimmed();
    if (mode.indexOf("simple_report") != -1) {
        m_mode = simpleReport;
    }
    if (mode.indexOf("multi_report") != -1) {
        m_mode = multiReport;
    }
    if (mode.indexOf("auto_report") != -1) {
        m_mode = autoReport;
    }
    switch (m_mode) {
    case multiReport:
    case autoReport:
        ui->pbGenerateReport->setVisible(true);
        ui->pbClearDB->setVisible(true);
        break;
    default:
        ui->pbGenerateReport->setVisible(false);
        ui->pbClearDB->setVisible(false);
        break;
    }

    m_dbreq = QSqlQuery(m_db);
    if (!_exec(createEventLogTable)) {
        setInfoText(getDBErrorText());
    }
    _exec("PRAGMA journal_mode = MEMORY;");
    _exec("PRAGMA page_size = 32768;");
    _exec("PRAGMA synchronous = OFF;");
    _exec("PRAGMA cache_size = 100000;"); // from test
    _exec("PRAGMA temp_store = MEMORY;"); // from test
    m_isHasError = false;
}

MainWindow::~MainWindow()
{
    doClearDB();
    closeDB();
    QString qs;
    qs.append(QSqlDatabase::database().connectionName());
    QSqlDatabase::removeDatabase(qs);

    delete m_state;
    delete ui;
}

void
MainWindow::onAboutProgram()
{
    QMessageBox::about(this, tr("About MMS EventLog Converter"),
                             tr("MMS EventLog Converter\nVersion 1.0\n(c) Oleksii Gaienko"));
}

void
MainWindow::onAboutQt()
{
    QMessageBox::aboutQt(this);
}

bool
MainWindow::_exec(const QString &query) {
    qDebug() << query;
    m_dbreq.clear();
    bool retVal = m_dbreq.exec(query);
    if (!retVal) {
        QSqlError error = m_db.lastError();
        m_dbErrorText = error.text();
    }
    return retVal;
}

bool
MainWindow::_exec() {
    bool retVal = m_dbreq.exec();
    if (!retVal) {
        QSqlError error = m_db.lastError();
        m_dbErrorText = error.text();
    }
    return retVal;
}

bool
MainWindow::openDB(const QString &dbName)
{
    m_db.setDatabaseName(dbName);
    bool retVal = m_db.open();
    if (!retVal){
        QSqlError error = m_db.lastError();
        m_dbErrorText = error.text();
    }
    return retVal;
}

void
MainWindow::closeDB()
{
    m_dbreq.finish();
    m_db.close();
}

bool
MainWindow::prepareRequest(const QString &query)
{
    m_dbreq.clear();
    bool retVal = m_dbreq.prepare(query);
    if (!retVal){
        QSqlError error = m_db.lastError();
        m_dbErrorText = error.text();
    }
    return retVal;
}

bool
MainWindow::beginTransaction()
{
    bool retVal = m_db.transaction();
    if (!retVal) {
        QSqlError error = m_db.lastError();
        m_dbErrorText = error.text();
    }
    return retVal;
}

bool
MainWindow::commitTransaction()
{
    bool retVal = m_db.commit();
    if (!retVal) {
        QSqlError error = m_db.lastError();
        m_dbErrorText = error.text();
    }
    return retVal;
}

void
removeQuote(QString &data, QChar quoteChar) {
    if (data.isNull() || data.isEmpty())
        return;

    if (data.at(0) == quoteChar) {
        data.remove(0, 1);
    }
    int lastChar = data.length() - 1;
    if (data.at(lastChar) == quoteChar) {
        data.resize(lastChar);
    }
}

void
parseHeaderString(const QString &buffer, QStringList &fields, QChar delimChar, QChar quoteChar) {

    fields.append(buffer.split(delimChar));
    int fieldsCount = fields.size();
    QString buf;
    for (int i = 0; i < fieldsCount; ++i) {
        buf = fields.at(i).trimmed();
        removeQuote(buf, quoteChar);
        fields[i] = buf.trimmed();
    }
}

void
analizeIPAdresses(const QString &ipaddresses, QString &externalIP, QString &internalIP)
{
    int pos = ipaddresses.indexOf(',');
    if (pos != -1) {
        QString firstip = ipaddresses.mid(0, pos).trimmed();
        QString secondip = ipaddresses.mid(pos + 1).trimmed();
        bool isPrivateFirstIP = (firstip.indexOf("10.") == 0)? true : false;
        bool isPrivateSecondIP = (secondip.indexOf("10.") == 0)? true : false;

        if (isPrivateFirstIP && isPrivateSecondIP) {
            externalIP = QString();
            internalIP = ipaddresses;
            return;
        }

        if (isPrivateFirstIP) {
            externalIP = secondip;
            internalIP = firstip;
        } else {
            externalIP = firstip;
            internalIP = secondip;
        }
        return;
    } // pos()

    if (ipaddresses.indexOf("10.") == 0) {
        externalIP = QString();
        internalIP = ipaddresses;
    } else {
        externalIP = ipaddresses;
        internalIP = QString();
    }
}

bool
parseUserSuccessLogonDetails(const QString &buffer, QString &username, QString &authType, QString &externalip, QString &internalip)
{
    bool retVal = false;
    username.clear(), authType.clear(), externalip.clear(), internalip.clear();

    QRegularExpression re("^(.*?)@N@(.*?)@N@(.*?)$");
    QRegularExpressionMatch match = re.match(buffer);
    if (match.hasMatch()) {
        username = match.captured(1).trimmed();
        username.remove("username: ");
        username.resize(username.length() - 1);

        authType = match.captured(2).trimmed();
        authType.remove("type: ");
        authType.resize(authType.length() - 1);

        QString ipaddresses = match.captured(3).trimmed();
        ipaddresses.remove("ip address: ");
        analizeIPAdresses(ipaddresses, externalip, internalip);

        retVal = true;
    }
    return retVal;
}

bool
parseUserFailedLogonDetails(const QString &buffer, QString &username, QString &authType, QString &externalip, QString &internalip)
{
    bool retVal = false;
    username.clear(), authType.clear(), externalip.clear(), internalip.clear();

    QRegularExpression re("^(.*?)@N@(.*?)$");
    QRegularExpressionMatch match = re.match(buffer);

    if (match.hasMatch()) {
        authType = match.captured(1).trimmed();
        authType.remove("type: ");

        QString ipaddresses = match.captured(2).trimmed();
        ipaddresses.remove("ip address: ");
        analizeIPAdresses(ipaddresses,externalip, internalip);

        retVal = true;
    }
    return retVal;
}

bool
parseUserLogonDetails(const QString &buffer, QString &username, QString &authType, QString &externalip, QString &internalip)
{
    bool retVal = parseUserSuccessLogonDetails(buffer, username, authType, externalip, internalip);
    if (!retVal) {
        retVal = parseUserFailedLogonDetails(buffer, username, authType, externalip, internalip);
    }
    return retVal;
}

bool
MainWindow::openEventLogFile()
{
    m_file.setFileName(m_filename);
    bool retVal = m_file.open(QIODevice::ReadOnly);
    if (!retVal) {
        m_fileErrorText = m_file.errorString();
    }
/*
 * QIODevice::Text - dont using
 * About QIODevice::Text in openMode the official document says.
 *      When reading, the end-of-line terminators are translated to '\n'.
 *      When writing, the end-of-line terminators are translated to the local encoding, for example '\r\n' for Win32.
 * It says Win32,while working on Win64,Qt5.8 I found it works differently. With QIODevice::Text in openMode, QIODevice::readAll() remove all '\r','\t'.
 * And talk about \n,they are replaced by \r whatever openMode is using. May be removed if using QIODevice::Text mode.
 */
    return retVal;
}

bool
MainWindow::readEventLogFile()
{
    bool retVal = true;
    QTextStream textStream(&m_file);
    try {
        m_buffer = textStream.readAll();
        if (m_buffer.isEmpty() || m_buffer.isNull()) {
            retVal = false;
        }
    } catch (...) {
        retVal = false;
        closeEventLogFile();
        m_buffer.clear();
    }
    return retVal;
}

void
MainWindow::openFileClick()
{
    QString selected_filter;
    //QString dir = QApplication::applicationDirPath();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open MMS EventLog"), QDir::currentPath(), tr("MMS Eventlog (*.csv);; MMS Eventlog (*.xls)"), &selected_filter);
    // The inferior stopped because it triggered an eaxception.
    // ref: https://forum.qt.io/topic/142647/the-inferior-stopped-because-it-triggered-an-eaxception/2
    if (fileName.isEmpty() || fileName.isNull()) {
        return;
    }
    m_filename = fileName;

    qDebug() << selected_filter;
    qDebug() << m_filename;
    qDebug() << QFileInfo(m_filename).absoluteDir().absolutePath(); //path
    qDebug() << QFileInfo(m_filename).fileName(); //name.ext
    qDebug() << QFileInfo(m_filename).baseName();
    m_logType = -1;
    if (selected_filter.indexOf("*.csv") != -1) {
        qDebug() << "*.csv";
        m_logType = 0;
    }
    if (selected_filter.indexOf("*.xls") != -1) {
        qDebug() << "*.xls";
        m_logType = 1;
    }

    setStateText(tr("File %1 was selected").arg(QFileInfo(m_filename).fileName()));
}

void
MainWindow::removeCRLF()
{
    m_buffer.replace("\r\n", "@RN@", Qt::CaseInsensitive );
    m_buffer.replace("\n", "@N@", Qt::CaseInsensitive);
    m_buffer.replace("@RN@", "\n", Qt::CaseInsensitive );
}

bool
MainWindow::splitIntoLines(QChar eolChar)
{
    bool retVal = true;
    try {
        m_stringList.append(m_buffer.split(eolChar));
    } catch (...) {
        retVal = false;
        m_stringList.clear();
    }
    return retVal;
}

bool
MainWindow::doParseEventLogFile()
{
    QStringList headerItems;
    parseHeaderString(m_stringList.at(0), headerItems, ',', '"');
    setInfoText(headerItems.join(" , ") + " -- Details");
    QApplication::processEvents();

    const QString insertOriginalData = QStringLiteral("INSERT INTO [eventlog] (username, timestampISO8601, requestid, type, details, timestamp, username1, authtype, externalip, internalip) \
                                        VALUES (:username, :timestampISO8601, :requestid, :type, :details, :timestamp, :username1, :authtype, :externalip, :internalip)");
    if (!prepareRequest(insertOriginalData)) {
        setInfoText(getDBErrorText());
        setStateText(tr("DB Error!"));
        return false;
    }

    QRegularExpression re("^(\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\")"); //get header
    QString header, details, timestampISO8601, format;
    const QString format24 = "yyyy-MM-ddTHH:mm:ss.zzzZ", // lenght 24
        format22 = "yyyy-MM-ddTHH:mm:ss.zZ", // lenght 22
        format20 = "yyyy-MM-ddTHH:mm:ssZ"; //lenght 20
    QDateTime timestamp, timestamptz;

    QString username,  authType, ipaddresses, externalip, internalip;

    if (!beginTransaction()) {
        setInfoText(getDBErrorText());
        setStateText(tr("DB Error!"));
        return false;
    }

    m_isHasError = false;
    for (int i = 1; i < m_stringList.size(); ++i) {
        details = m_stringList.at(i);
        QRegularExpressionMatch match = re.match(details);
        if (match.hasMatch()) {
            header = match.captured(0);

            details.remove(header, Qt::CaseInsensitive);
            details.remove(0, 1); // remove first ',' char before quote char in details field
            removeQuote(details, '"');

            headerItems.clear();
            parseHeaderString(header, headerItems, ',', '"');
            header = headerItems.join(" , ");

            m_dbreq.bindValue(":username", headerItems.at(0));
            timestampISO8601 = headerItems.at(1);
            m_dbreq.bindValue(":timestampISO8601", timestampISO8601);
            switch (timestampISO8601.length()) {
            case 20:
                format = format20;
                break;
            case 22:
            case 23:
                format = format22;
                break;
            default:
                format = format24;
                break;
            }
            timestamp = QDateTime::fromString(timestampISO8601, format);
            timestamp.setTimeSpec(Qt::UTC);
            timestamptz = timestamp.toLocalTime();
            m_dbreq.bindValue(":timestamp", timestamptz);
            m_dbreq.bindValue(":requestid", headerItems.at(2));
            m_dbreq.bindValue(":type", headerItems.at(3));
            m_dbreq.bindValue(":details", details);

            m_dbreq.bindValue(":username1", QString());
            m_dbreq.bindValue(":authtype", QString());
            m_dbreq.bindValue(":externalip", QString());
            m_dbreq.bindValue(":internalip", QString());

            if ((details.indexOf("ip address:") > 0) && parseUserLogonDetails(details, username, authType, externalip, internalip)) {
                m_dbreq.bindValue(":username1", username);
                m_dbreq.bindValue(":authtype", authType);
                m_dbreq.bindValue(":externalip", externalip);
                m_dbreq.bindValue(":internalip", internalip);
            }
            if (!_exec()) {
                m_isHasError = true;
                setInfoText(getDBErrorText());
                setStateText(tr("DB Error!"));
                break;
            }
            setInfoText(tr("Added: %1 -- %2").arg(header, details));
            QApplication::processEvents();
        }
    } // for
    if (m_isHasError) {
        return false;
    }

    if (!commitTransaction()) {
        setInfoText(getDBErrorText());
        setStateText(tr("DB Error!"));
        return false;
    }

    setStateText(tr("File %1 was parsed").arg(QFileInfo(m_filename).fileName()));
    QApplication::processEvents();
    return true;
}

bool
MainWindow::doGenerateReport(const QString &fileName)
{
    setStateText(tr("Generating report"));
    QApplication::processEvents();
    // [1]  Writing excel file(*.xlsx)
    int rowHeader = 1, row = 2;
    int colTimestampISO8601 = 1, colTimestamp = 2, colExternalIP = 3, colUsername = 4, colType = 5, colDetails = 6,
        colAuthType = 7, colInternalIP = 8, colRequestid = 9;
    QString buf;
    const QString getAllRecords = QStringLiteral("select timestampISO8601, timestamp, externalip, username, type, details, \
                                    authtype, internalip, requestid from eventlog order by timestamp DESC;");

    QXlsx::Document xlsxW;
    // Add header
    QVariant writeValue = QString("Відмітка часу (часовий пояс - UTC)");
    xlsxW.write(rowHeader, colTimestampISO8601, writeValue);
    writeValue = QString("Відмітка часу (за Київським часом)");
    xlsxW.write(rowHeader, colTimestamp, writeValue);
    writeValue = QString("Зовнішній IP");
    xlsxW.write(rowHeader, colExternalIP, writeValue);
    writeValue = QString("Ім'я користувача");
    xlsxW.write(rowHeader, colUsername, writeValue);
    writeValue = QString("Тип");
    xlsxW.write(rowHeader, colType, writeValue);
    writeValue = QString("Деталі");
    xlsxW.write(rowHeader, colDetails, writeValue);
    writeValue = QString("Тип авторизації");
    xlsxW.write(rowHeader, colAuthType, writeValue);
    writeValue = QString("Внутрішній IP");
    xlsxW.write(rowHeader, colInternalIP, writeValue);
    writeValue = QString("ID запиту");
    xlsxW.write(rowHeader, colRequestid, writeValue);

    // Add data
    Format dateFormat;
    dateFormat.setHorizontalAlignment(Format::AlignRight);
    dateFormat.setNumberFormat("dd.mm.yyyy hh:mm:ss");

    if (!_exec(getAllRecords)) {
        setInfoText(getDBErrorText());
        return false;
    }

    while (m_dbreq.next()) {
        writeValue = m_dbreq.value(0).toString();
        xlsxW.write(row, colTimestampISO8601, writeValue);

        writeValue = m_dbreq.value(1).toDateTime();
        xlsxW.write(row, colTimestamp, writeValue, dateFormat);

        writeValue = m_dbreq.value(2).toString();
        xlsxW.write(row, colExternalIP, writeValue);

        writeValue = m_dbreq.value(3).toString();
        xlsxW.write(row, colUsername, writeValue);

        writeValue = m_dbreq.value(4).toString();
        xlsxW.write(row, colType, writeValue);

        buf = m_dbreq.value(5).toString();
        buf.replace("@N@", "\n", Qt::CaseInsensitive);
        writeValue = buf;
        xlsxW.write(row, colDetails, writeValue);

        writeValue = m_dbreq.value(6).toString();
        xlsxW.write(row, colAuthType, writeValue);

        writeValue = m_dbreq.value(7).toString();
        xlsxW.write(row, colInternalIP, writeValue);

        writeValue = m_dbreq.value(8).toString();
        xlsxW.write(row, colRequestid, writeValue);

        ++row;
        QApplication::processEvents();
    }

    QString saveFileName;
    if (m_mode == simpleReport) {
        saveFileName = QFileInfo(m_filename).absoluteDir().absolutePath() + '/' + QFileInfo(m_filename).baseName() + "_report.xlsx";
    } else {
        saveFileName = fileName;
    }
    qDebug() << saveFileName;
    if (!xlsxW.saveAs(saveFileName)) { // save the document as 'Test.xlsx'
        setStateText(tr("Error save xlsx report!"));
        return false;
    }
    setStateText(tr("Report file %1 was saved").arg(QFileInfo(saveFileName).fileName()));
    return true;
}

void
MainWindow::doParseDone()
{
//    if (m_mode == simpleReport) {
//        closeDB();
//    }
    enableButtons();
}

void
MainWindow::doClearDB()
{
    if (!_exec("drop table if exists [eventlog];")) {
        setInfoText(getDBErrorText());
        setStateText(tr("DB Error!"));
        return;
    }
    if (!_exec("VACUUM;")) {
        setInfoText(getDBErrorText());
        setStateText(tr("DB Error!"));
        return;
    }
    if (!_exec(createEventLogTable)) {
        setInfoText(getDBErrorText());
        setStateText(tr("DB Error!"));
        return;
    }
}

void
MainWindow::convertEventLogClick()
{
    disableButtons();

    m_buffer.clear();
    m_stringList.clear();
    if (m_mode == simpleReport) {
        doClearDB();
    }

    setStateText(tr("Opening file %1").arg(QFileInfo(m_filename).fileName()));
    QApplication::processEvents();

    if (!openEventLogFile()) {
        enableButtons();
        setStateText(tr("File %1 opening error").arg(QFileInfo(m_filename).fileName()));
        setInfoText(getFileErrorText());
        return;
    }

    setStateText(tr("Reading file %1").arg(QFileInfo(m_filename).fileName()));
    QApplication::processEvents();

    if (!readEventLogFile()) {
        enableButtons();
        closeEventLogFile();
        setStateText(tr("File %1 read error").arg(QFileInfo(m_filename).fileName()));
        setInfoText(tr("Error. File %1 too large").arg(QFileInfo(m_filename).fileName()));
        return;
    }
    closeEventLogFile();

    setStateText(tr("Preparation for file %1 parsing").arg(QFileInfo(m_filename).fileName()));
    QApplication::processEvents();

    removeCRLF();
    if (!splitIntoLines('\n')) {
        enableButtons();
        setStateText(tr("Error. File %1 too large").arg(QFileInfo(m_filename).fileName()));
        return;
    }

    setStateText(tr("File %1 parsing start").arg(QFileInfo(m_filename).fileName()));
    QApplication::processEvents();

    if (doParseEventLogFile() && m_mode == simpleReport)
        doGenerateReport();
    doParseDone();
}

void
MainWindow::clearDBclick()
{
    m_buffer.clear();
    m_stringList.clear();

    doClearDB();

    setInfoText(tr("Database was cleared"));
    setStateText(tr("Ready"));
}

void
MainWindow::generateReportClick()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save MMS EventLog report"),
                                                      QDir::currentPath(),
                                                      tr("Excel (*.xlsx)"));
    if (fileName.isEmpty() || fileName.isNull()) {
        return;
    }
    doGenerateReport(fileName);
    doParseDone();
}
