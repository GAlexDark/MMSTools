/****************************************************************************
*
* The template class for the ParserManager and the ReportManager Classes.
* Copyright (C) 2024  Oleksii Gaienko
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

#ifndef CBASICMANAGER_H
#define CBASICMANAGER_H

#include <QMetaType>
#include <QMetaClassInfo>

const quint16 invalidId = 0xFFFF;

template <class T>
class CBasicManager
{
public:
    CBasicManager() = default;
    CBasicManager(CBasicManager const&) = delete;
    CBasicManager& operator=(CBasicManager const&) = delete;
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
    quint16 getIdByIndex(const quint16 index) const
    {
        return index > 0 && index <= m_ids.size() ?  m_ids.at(index - 1) : invalidId;
    }

protected:
    void addClassListItem(const QString &value)
    {
        m_classList.append(value);
    }
    void addId(quint16 value)
    {
        m_ids.append(value);
    }
    void sortIds()
    {
        std::sort(m_ids.begin(), m_ids.end());
    }
    const QVector<quint16>& getIds() const
    {
        return m_ids;
    }

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
    QStringList m_classList;
    QVector<quint16> m_ids;
};

#endif // CBASICMANAGER_H
