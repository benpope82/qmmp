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

#include <QtGui>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/flacfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/tmap.h>

#include "cueparser.h"
#include "decoder_flac.h"
#include "flacmetadatamodel.h"
#include "decoderflacfactory.h"


// DecoderFLACFactory

bool DecoderFLACFactory::supports(const QString &source) const
{
    return (source.right(5).toLower() == ".flac");
}

bool DecoderFLACFactory::canDecode(QIODevice *input) const
{
    Q_UNUSED(input);
    return FALSE;
}

const DecoderProperties DecoderFLACFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("FLAC Plugin");
    properties.filter = "*.flac";
    properties.description = tr("FLAC Files");
    //properties.contentType = ;
    properties.shortName = "flac";
    properties.protocols = "flac";
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    return properties;
}

Decoder *DecoderFLACFactory::create(const QString &path, QIODevice *i)
{
    return new DecoderFLAC(path, i);
}

QList<FileInfo *> DecoderFLACFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    FileInfo *info = new FileInfo(fileName);

    TagLib::FLAC::File fileRef(fileName.toLocal8Bit ());
    TagLib::Tag *tag = useMetaData ? fileRef.tag() : 0;

    if (tag && !tag->isEmpty())
    {
        info->setMetaData(Qmmp::ALBUM,
                          QString::fromUtf8(tag->album().toCString(TRUE)).trimmed());
        info->setMetaData(Qmmp::ARTIST,
                          QString::fromUtf8(tag->artist().toCString(TRUE)).trimmed());
        info->setMetaData(Qmmp::COMMENT,
                          QString::fromUtf8(tag->comment().toCString(TRUE)).trimmed());
        info->setMetaData(Qmmp::GENRE,
                          QString::fromUtf8(tag->genre().toCString(TRUE)).trimmed());
        info->setMetaData(Qmmp::TITLE,
                          QString::fromUtf8(tag->title().toCString(TRUE)).trimmed());
        info->setMetaData(Qmmp::YEAR, tag->year());
        info->setMetaData(Qmmp::TRACK, tag->track());
    }

    if (fileRef.audioProperties())
        info->setLength(fileRef.audioProperties()->length());

    //looking for cuesheet comment
    TagLib::Ogg::XiphComment *xiph_comment = useMetaData ? fileRef.xiphComment() : 0;
    QList <FileInfo*> list;
    if(xiph_comment)
    {
        if (xiph_comment->fieldListMap().contains("CUESHEET"))
        {
            CUEParser parser(xiph_comment->fieldListMap()["CUESHEET"]
                             .toString().toCString(TRUE), fileName);
            list = parser.createPlayList();
            delete info;
            return list;
        }
        else
        {
            //additional metadata
            TagLib::StringList fld;
            if(!(fld = xiph_comment->fieldListMap()["COMPOSER"]).isEmpty())
                info->setMetaData(Qmmp::COMPOSER,
                                  QString::fromUtf8(fld.toString().toCString(TRUE)).trimmed());
            if(!(fld = xiph_comment->fieldListMap()["DISCNUMBER"]).isEmpty())
                info->setMetaData(Qmmp::DISCNUMBER,
                                  QString::fromUtf8(fld.toString().toCString(TRUE)).trimmed());
        }
    }
    list << info;
    return list;
}

MetaDataModel*DecoderFLACFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    if (!path.contains("://") || path.startsWith("flac://"))
        return new FLACMetaDataModel(path, parent);
    else
        return 0;
}

void DecoderFLACFactory::showSettings(QWidget *)
{}

void DecoderFLACFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About FLAC Audio Plugin"),
                        tr("Qmmp FLAC Audio Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderFLACFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/flac_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(flac,DecoderFLACFactory)
