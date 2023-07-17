
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QFile>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

const int simpleReport = 0;
const int multiReport = 1;
const int autoReport = 2;

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getDBErrorText() const { return m_dbErrorText; }
    QString getFileErrorText() const { return m_fileErrorText; }

private:
    Ui::MainWindow *ui;
    QLabel         *m_state;

    QString         m_filename,
                    m_fileErrorText;
    QFile           m_file;
    QString         m_path;

    QString         m_buffer;
    QStringList     m_stringList;

    int             m_logType;
    bool            m_isButtonsDisabled;

    QSqlDatabase    m_db,
                    m_excelDB;
    QSqlQuery       m_dbreq;
    QString         m_ConnectionName,
                    m_dbErrorText;
    bool            m_isHasError;
    int             m_mode;

    void disableButtons();
    void enableButtons();

    bool _exec(const QString &query);
    bool _exec();
    bool openDB(const QString &dbName);
    void closeDB();
    bool prepareRequest(const QString &query);
    bool beginTransaction();
    bool commitTransaction();

    bool openEventLogFile();
    void closeEventLogFile() { m_file.close(); };
    bool readEventLogFile();
    void removeCRLF();
    bool splitIntoLines(QChar eolChar);

    bool doParseEventLogFile();
    bool doGenerateReport(const QString &fileName = QString());
    void doParseDone();
    void doClearDB();
    void setStateText(const QString &state);
    void setInfoText(const QString &info);

private slots:
    //window menu
    void onAboutProgram();
    void onAboutQt();

    //
    void openFileClick();
    void convertEventLogClick();
    void clearDBclick();
    void generateReportClick();
};

#endif // MAINWINDOW_H
