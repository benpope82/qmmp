/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QStringList>
#include <QDateTime>
#include <QFileInfo>
#include <QPluginLoader>
#include <QApplication>
#include <QTranslator>
#include "generalfactory.h"
#include "qmmpuiplugincache_p.h"

QmmpUiPluginCache::QmmpUiPluginCache(const QString &file, QSettings *settings)
{
    m_error = false;
    m_instance = 0;
    m_generalFactory = 0;
    m_priority = 0;
    bool update = false;
    QFileInfo info(file);
    m_path = info.QFileInfo::canonicalFilePath();

    settings->beginGroup("PluginCache");
    QString key = m_path;
#ifndef Q_OS_WIN
    key.remove(0,1);
#endif
    if(settings->allKeys().contains(key))

    {
        QStringList values = settings->value(m_path).toStringList();
        if(values.count() != 3)
            update = true;
        else
        {
            m_shortName = values.at(0);
            m_priority = values.at(1).toInt();
            update = (info.lastModified().toString(Qt::ISODate) != values.at(2));
        }
    }
    else
        update = true;


    if(update)
    {
        if(GeneralFactory *factory = generalFactory())
        {
            m_shortName = factory->properties().shortName;
            m_priority = 0;
        }
        else
        {
            qWarning("QmmpUiPluginCache: unknown plugin type: %s", qPrintable(m_path));
            m_error = true;
        }

        if (!m_error)
        {
            QStringList values;
            values << m_shortName;
            values << QString::number(m_priority);
            values << info.lastModified().toString(Qt::ISODate);
            settings->setValue(m_path, values);
            qDebug("QmmpUiPluginCache: added cache item \"%s=%s\"",
                   qPrintable(info.fileName()), qPrintable(values.join(",")));
        }
    }
    settings->endGroup();
}

const QString QmmpUiPluginCache::shortName() const
{
    return m_shortName;
}

const QString QmmpUiPluginCache::file() const
{
    return m_path;
}

int QmmpUiPluginCache::priority() const
{
    return m_priority;
}

bool QmmpUiPluginCache::hasError() const
{
    return m_error;
}

GeneralFactory *QmmpUiPluginCache::generalFactory()
{
    if(!m_generalFactory)
    {
        m_generalFactory = qobject_cast<GeneralFactory *> (instance());
        if(m_generalFactory)
            qApp->installTranslator(m_generalFactory->createTranslator(qApp));
    }
    return m_generalFactory;
}

QObject *QmmpUiPluginCache::instance()
{
    if(m_error)
        return 0;
    if(m_instance)
        return m_instance;
    QPluginLoader loader(m_path);
    m_instance = loader.instance();
    if (loader.isLoaded())
        qDebug("QmmpUiPluginCache: loaded plugin %s", qPrintable(QFileInfo(m_path).fileName()));
    else
    {
        m_error = true;
        qWarning("QmmpUiPluginCache: error: %s", qPrintable(loader.errorString ()));
    }
    return m_instance;
}

void QmmpUiPluginCache::cleanup(QSettings *settings)
{
    settings->beginGroup("PluginCache");

    foreach (QString key, settings->allKeys())
    {
#ifdef Q_OS_WIN
        if(!QFile::exists(key))
#else
        if(!QFile::exists("/" + key))
#endif
        {
            settings->remove(key);
            qDebug("QmmpUiPluginCache: removed key %s", qPrintable(key));
        }
    }
    settings->endGroup();
}
