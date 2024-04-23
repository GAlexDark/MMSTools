/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Common module
*
*  Module name: CBasicManager.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The template class for the ParserManager and
*     the ReportManager classes.
*
****************************************************************************/

#ifndef CBASICMANAGER_H
#define CBASICMANAGER_H

#include <QMetaType>
#include <QMetaClassInfo>

template <class T>
class CBasicManager
{
public:
    ~CBasicManager()
    {
        destroyInstance();
    }

    T getInstance(const quint16 id)
    {
        destroyInstance();

        QString name;
        for (qsizetype i = 0; i < m_classList.size(); i++) {
            name = m_classList.at(i).trimmed();
            m_type = QMetaType::fromName(name.toUtf8());
            if (m_type.isValid()) {
                const QMetaObject* mObj = m_type.metaObject();
                for (int j = mObj->classInfoOffset(); j < mObj->classInfoCount(); j++) {
                    QMetaClassInfo classInfo = mObj->classInfo(j);
                    if ((QString(classInfo.name()) == QLatin1String("ID")) && (QString(classInfo.value()).toUInt() == id)) {
                        m_instancePtr = dynamic_cast<T>(mObj->newInstance());
                        Q_CHECK_PTR(m_instancePtr);
                        break;
                    }
                }
            }
        } //for

        return m_instancePtr;
    }

    QStringList getClassList() const { return m_classList; }
    qsizetype getItemCount() const { return m_classList.size(); }
    bool checkID(const quint16 id) { return m_ids.indexOf(id) != -1 ? true : false; }

protected:
    void destroyInstance()
    {
        if (m_type.isValid() && (m_instancePtr != nullptr)) {
            m_type.destroy(m_instancePtr);
            m_instancePtr = nullptr;
        }
    }

    QStringList         m_classList;
    QVector<quint16>    m_ids;

private:
    QMetaType           m_type;
    T                   m_instancePtr;

};

#endif // CBASICMANAGER_H
