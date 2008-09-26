/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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
#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>
#include <QMap>

#include "qmmp.h"

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class FileInfo
{
public:
    FileInfo();
    //FileInfo(const FileInfo &other);

    ~FileInfo();

    const qint64 length () const;
    const QString metaData (Qmmp::MetaData key) const;
    const QMap<Qmmp::MetaData, QString>  metaData () const;
    bool isEmpty();

    void setLength(qint64 length);
    void setMetaData(Qmmp::MetaData key, const QString &value);
    void setMetaData(Qmmp::MetaData key, int value);
    void setMetaData(const QMap <Qmmp::MetaData,  QString> &metaData);

private:
    QMap <Qmmp::MetaData, QString> m_metaData;
    qint64 m_length;
};

#endif
