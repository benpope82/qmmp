/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "decoder.h"
#include "decoderfactory.h"
#include "abstractengine.h"
#include "inputsource.h"
#include "metadatamanager.h"

MetaDataManager* MetaDataManager::m_instance = 0;

MetaDataManager::MetaDataManager()
{
    if(m_instance)
        qFatal("MetaDataManager is already created");
    m_instance = this;
    m_decoderFactories = Decoder::factories();
    m_engineFactories = AbstractEngine::factories();
    m_inputSourceFactories = InputSource::factories();
}

MetaDataManager::~MetaDataManager()
{
    m_instance = 0;
}

QList <FileInfo *> MetaDataManager::createPlayList(const QString &fileName, bool useMetaData) const
{
    QList <FileInfo *> list;
    DecoderFactory *fact = 0;
    EngineFactory *efact = 0;

    if (!fileName.contains("://")) //local file
    {
        if((fact = Decoder::findByPath(fileName)))
            return fact->createPlayList(fileName, useMetaData);
        else if((efact = AbstractEngine::findByPath(fileName)))
            return efact->createPlayList(fileName, useMetaData);
        return list;
    }
    else if(protocols().contains(fileName.section("://",0,0)))
        list << new FileInfo(fileName);
    return list;
}

MetaDataModel* MetaDataManager::createMetaDataModel(const QString &path, QObject *parent) const
{
    DecoderFactory *fact = 0;
    EngineFactory *efact = 0;
    if (!path.contains("://")) //local file
    {
        if((fact = Decoder::findByPath(path)))
            return fact->createMetaDataModel(path, parent);
        else if((efact = AbstractEngine::findByPath(path)))
            return efact->createMetaDataModel(path, parent);
        return 0;
    }
    else
    {
        QString scheme = path.section("://",0,0);
        MetaDataModel *model = 0;
        foreach(fact, *Decoder::factories())
        {
            if(fact->properties().protocols.split(" ").contains(scheme))
                model = fact->createMetaDataModel(path, parent);
            if(model)
                return model;
        }
        foreach(efact, *AbstractEngine::factories())
        {
            if(efact->properties().protocols.split(" ").contains(scheme))
                model = efact->createMetaDataModel(path, parent);
            if(model)
                return model;
        }
    }
    return 0;
}

QStringList MetaDataManager::filters() const
{
    QStringList filters;
    foreach(DecoderFactory *fact, *m_decoderFactories)
    {
        if (Decoder::isEnabled(fact) && !fact->properties().filter.isEmpty())
            filters << fact->properties().description + " (" + fact->properties().filter + ")";
    }
    foreach(EngineFactory *fact, *m_engineFactories)
    {
        if (AbstractEngine::isEnabled(fact) && !fact->properties().filter.isEmpty())
            filters << fact->properties().description + " (" + fact->properties().filter + ")";
    }
    return filters;
}

QStringList MetaDataManager::nameFilters() const
{
    QStringList filters;
    foreach(DecoderFactory *fact, *m_decoderFactories)
    {
        if (Decoder::isEnabled(fact))
            filters << fact->properties().filter.split(" ", QString::SkipEmptyParts);
    }
    foreach(EngineFactory *fact, *m_engineFactories)
    {
        if (AbstractEngine::isEnabled(fact))
            filters << fact->properties().filter.split(" ", QString::SkipEmptyParts);
    }
    return filters;
}

QStringList MetaDataManager::protocols() const
{
    QStringList p;
    foreach(InputSourceFactory *f, *m_inputSourceFactories)
    {
        p << f->properties().protocols.split(" ", QString::SkipEmptyParts);
    }
    foreach(DecoderFactory *f, *m_decoderFactories)
    {
        if (Decoder::isEnabled(f))
            p << f->properties().protocols.split(" ", QString::SkipEmptyParts);
    }
    return p;
}

bool MetaDataManager::supports(const QString &fileName) const
{
    DecoderFactory *fact = 0;
    EngineFactory *efact = 0;
    if (!fileName.contains("://")) //local file
    {
        if (!QFile::exists(fileName))
            return FALSE;
        if((fact = Decoder::findByPath(fileName)))
            return TRUE;
        else if((efact = AbstractEngine::findByPath(fileName)))
            return TRUE;
        return FALSE;
    }
    return FALSE;
}

QPixmap MetaDataManager::getCover(const QString &url) const
{
    if(!url.contains("://"))
    {
        QString p = getCoverPath(url);
        if(!p.isEmpty())
            return QPixmap(p);
    }
    MetaDataModel *model = createMetaDataModel(url);
    if(model)
    {
        QPixmap pix = model->cover();
        model->deleteLater();
        return pix;
    }
    return QPixmap();
}

QString MetaDataManager::getCoverPath(const QString &url) const
{
    if(url.contains("://")) //url
    {
        MetaDataModel *model = createMetaDataModel(url);
        if(model)
        {
            QString coverPath = model->coverPath();
            model->deleteLater();
            return coverPath;
        }
    }
    else //local file
    {
        QString p = QFileInfo(url).absolutePath();
        QDir dir(p);
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        dir.setSorting(QDir::Name);
        QStringList filters;
        filters << "*.jpg" << "*.png";
        QFileInfoList file_list = dir.entryInfoList(filters);
        foreach(QFileInfo i, file_list)
        {
            if(!i.absoluteFilePath().contains("back", Qt::CaseInsensitive))
                return i.absoluteFilePath();
        }
    }
    return QString();
}

MetaDataManager *MetaDataManager::instance()
{
    if(!m_instance)
        new MetaDataManager();
    return m_instance;
}

void MetaDataManager::destroy()
{
    if(m_instance)
        delete m_instance;
}

