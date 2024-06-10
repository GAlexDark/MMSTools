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
    CBasicManager()
    {
        m_classList.clear();
        m_ids.clear();
    }

    ~CBasicManager()
    {
        destroyInstance();
    }

    T getInstance(const quint16 id)
    {
        destroyInstance();

        for (const QString &name : m_classList) {
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
        }
        return m_instancePtr;
    }

    const QStringList& getClassList() const { return m_classList; }
    qsizetype getItemCount() const { return m_classList.size(); }
    bool checkID(const quint16 id) const { return m_ids.indexOf(id) != -1 ? true : false; }

protected:
    QStringList m_classList;
    QVector<quint16> m_ids;

private:
    void destroyInstance()
    {
        if (m_type.isValid() && (m_instancePtr != nullptr)) {
            m_type.destroy(m_instancePtr);
            m_instancePtr = nullptr;
        }
    }

    QMetaType m_type;
    T m_instancePtr = nullptr;
};

#endif // CBASICMANAGER_H
