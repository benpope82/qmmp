/***************************************************************************
 *   Copyright (C) 2009-2014 by Ilya Kotov                                 *
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

#include "ffmpegmetadatamodel.h"

FFmpegMetaDataModel::FFmpegMetaDataModel(const QString &path, QObject *parent) : MetaDataModel(parent)
{
    m_in = 0;
    if (avformat_open_input(&m_in, path.toLocal8Bit().constData(), 0, 0) < 0)
        return;
    avformat_find_stream_info(m_in, 0);
    av_read_play(m_in);
}

FFmpegMetaDataModel::~FFmpegMetaDataModel()
{
    if(m_in)
        avformat_close_input(&m_in);
}

QHash<QString, QString> FFmpegMetaDataModel::audioProperties()
{
    QHash<QString, QString> ap;
    if(!m_in)
        return ap;
    QString text = QString("%1").arg(int(m_in->duration/AV_TIME_BASE)/60);
    text +=":"+QString("%1").arg(int(m_in->duration/AV_TIME_BASE)%60,2,10,QChar('0'));
    ap.insert(tr("Length"), text);
    ap.insert(tr("File size"),  QString("%1 ").arg(avio_size(m_in->pb)) + " " + tr("KB"));
    ap.insert(tr("Bitrate"), QString("%1 "+tr("kbps")).arg(m_in->bit_rate/1000));

    AVCodecContext *c = 0;
    uint wma_idx;
    for (wma_idx = 0; wma_idx < m_in->nb_streams; wma_idx++)
    {
        c = m_in->streams[wma_idx]->codec;
        if (c->codec_type == AVMEDIA_TYPE_AUDIO)
            break;
    }
    if (c)
    {
        ap.insert(tr("Sample rate"), QString("%1 " + tr("Hz")).arg(c->sample_rate));
        ap.insert(tr("Channels"), QString("%1").arg(c->channels));
    }
    return ap;
}
