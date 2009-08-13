/***************************************************************************
 *   Copyright (C) 2006-2009 by Ilya Kotov                                 *
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
#include <QTextCodec>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/apetag.h>
#include <taglib/tfile.h>
#include <taglib/mpegfile.h>
#include <taglib/mpegheader.h>
#include <taglib/mpegproperties.h>
#include <taglib/textidentificationframe.h>

#include <qmmp/decoder.h>
#include <qmmp/qmmp.h>

#include "detailsdialog.h"

DetailsDialog::DetailsDialog(QWidget *parent, const QString &path)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    m_path = path;
    setWindowTitle (path.section('/',-1));
    ui.pathLineEdit->setText(m_path);

    if (!QFile::exists(m_path))
        return;

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("MAD");
    m_codec_v1 =
        QTextCodec::codecForName(settings.value("ID3v1_encoding", "ISO-8859-1")
                                 .toByteArray ());
    m_codec_v2 =
        QTextCodec::codecForName(settings.value("ID3v2_encoding","UTF-8")
                                 .toByteArray ());
    if (!m_codec_v1)
        m_codec_v1 = QTextCodec::codecForName ("ISO-8859-1");
    if (!m_codec_v2)
        m_codec_v2 = QTextCodec::codecForName ("UTF-8");

    QString tag_name = settings.value("current_tag","ID3v2").toString ();

    if (tag_name == "ID3v1")
        ui.id3v1RadioButton->setChecked(TRUE);
    else if (tag_name == "ID3v2")
        ui.id3v2RadioButton->setChecked(TRUE);
    else if (tag_name == "APE")
        ui.apeRadioButton->setChecked(TRUE);
    else
        ui.id3v2RadioButton->setChecked(TRUE);

    settings.endGroup();

    loadMPEGInfo();
    QFileInfo info(m_path);
    m_rw = info.isWritable();
    loadTag();
    connect(ui.saveButton, SIGNAL(clicked()), SLOT(save()));
    connect(ui.createButton, SIGNAL(clicked()), SLOT(create()));
    connect(ui.deleteButton, SIGNAL(clicked()), SLOT(deleteTag()));
    connect(ui.id3v1RadioButton, SIGNAL(clicked()), SLOT(loadTag()));
    connect(ui.id3v2RadioButton, SIGNAL(clicked()), SLOT(loadTag()));
    connect(ui.apeRadioButton, SIGNAL(clicked()), SLOT(loadTag()));
    QPixmap pix = Decoder::findCover(m_path);
    if(pix.isNull())
    {
        ui.coverWidget->hide();
        qobject_cast <QBoxLayout *> (ui.groupBox->layout ())->setDirection(QBoxLayout::LeftToRight);
    }
    else
        ui.coverWidget->setPixmap(pix);
}


DetailsDialog::~DetailsDialog()
{}

void DetailsDialog::loadMPEGInfo()
{
    TagLib::MPEG::File f (m_path.toLocal8Bit().constData());
    QString text;
    QString v;
    switch((int)f.audioProperties()->version())
    {
        case TagLib::MPEG::Header::Version1:
        v = "1";
        break;
        case TagLib::MPEG::Header::Version2:
        v = "2";
        break;
        case TagLib::MPEG::Header::Version2_5:
        v = "2.5";
    }
    QMap <QString, QString> ap;
    text = QString("MPEG-%1 layer %2").arg(v).arg(f.audioProperties()->layer());
    ap.insert(tr("Format"), text);
    text = QString("%1").arg(f.audioProperties()->bitrate());
    ap.insert(tr("Bitrate"), text+" "+tr("kbps"));
    text = QString("%1").arg(f.audioProperties()->sampleRate());
    ap.insert(tr("Samplerate"), text+" "+tr("Hz"));
    switch (f.audioProperties()->channelMode())
    {
    case TagLib::MPEG::Header::Stereo:
        ap.insert(tr("Mode"), "Stereo");
        break;
    case TagLib::MPEG::Header::JointStereo:
        ap.insert(tr("Mode"), "Joint stereo");
        break;
    case TagLib::MPEG::Header::DualChannel:
        ap.insert(tr("Mode"), "Dual channel");
        break;
    case TagLib::MPEG::Header::SingleChannel:
        ap.insert(tr("Mode"), "Single channel");
        break;
    }
    text = QString("%1 "+tr("KB")).arg(f.length()/1024);
    ap.insert(tr("File size"), text);
    if (f.audioProperties()->protectionEnabled())
        ap.insert(tr("Protection"), tr("Yes"));
    else
        ap.insert(tr("Protection"), tr("No"));
    if (f.audioProperties()->isCopyrighted())
        ap.insert(tr("Copyright"), tr("Yes"));
    else
        ap.insert(tr("Copyright"), tr("No"));
    if (f.audioProperties()->isOriginal())
        ap.insert(tr("Original"), tr("Yes"));
    else
        ap.insert(tr("Original"), tr("No"));
    showAudioProperties(ap);
}

void DetailsDialog::loadTag()
{
    TagLib::MPEG::File f (m_path.toLocal8Bit().constData());
    QTextCodec *codec = QTextCodec::codecForName ("UTF-8");
    TagLib::Tag *tag = 0;
    TagLib::ID3v2::FrameListMap flm;

    if (selectedTag() == TagLib::MPEG::File::ID3v1)
    {
        tag = f.ID3v1Tag();
        codec = m_codec_v1;
        ui.tagGroupBox->setTitle(tr("ID3v1"));
    }
    else if (selectedTag() == TagLib::MPEG::File::ID3v2)
    {
        tag = f.ID3v2Tag();
        codec = m_codec_v2;
        ui.tagGroupBox->setTitle(tr("ID3v2"));
        if(tag)
            flm = f.ID3v2Tag()->frameListMap();
    }
    else if (selectedTag() == TagLib::MPEG::File::APE)
    {
        ui.tagGroupBox->setTitle(tr("APE"));
        tag = f.APETag();
    }
    ui.saveButton->setEnabled(tag && m_rw);
    ui.createButton->setEnabled(!tag && m_rw);
    ui.deleteButton->setEnabled(tag && m_rw);
    ui.tagGroupBox->setEnabled(tag);
    //clear old values
    ui.titleLineEdit->clear();
    ui.artistLineEdit->clear();
    ui.albumLineEdit->clear();
    ui.commentBrowser->clear();
    ui.yearSpinBox->clear();
    ui.trackSpinBox->setValue(0);
    ui.genreLineEdit->clear();
    ui.composerLineEdit->clear();
    ui.discSpinBox->setValue(0);

    if (tag)
    {
        bool utf = codec->name().contains("UTF");
        if (utf)
            codec = QTextCodec::codecForName ("UTF-8");
        TagLib::String title = tag->title();
        TagLib::String artist = tag->artist();
        TagLib::String album = tag->album();
        TagLib::String comment = tag->comment();
        TagLib::String genre = tag->genre();
        QString string = codec->toUnicode(title.toCString(utf)).trimmed();
        ui.titleLineEdit->setText(string);
        string = codec->toUnicode(artist.toCString(utf)).trimmed();
        ui.artistLineEdit->setText(string);
        string = codec->toUnicode(album.toCString(utf)).trimmed();
        ui.albumLineEdit->setText(string);
        string = codec->toUnicode(comment.toCString(utf)).trimmed();
        ui.commentBrowser->setText(string);
        ui.yearSpinBox->setValue(tag->year());
        ui.trackSpinBox->setValue(tag->track());
        string = codec->toUnicode(genre.toCString(utf)).trimmed();
        ui.genreLineEdit->setText(string);
        if(selectedTag() == TagLib::MPEG::File::ID3v2)
        {
            if(!flm["TCOM"].isEmpty())
            {
                string = codec->toUnicode(flm["TCOM"].front()->toString().toCString(utf)).trimmed();
                ui.composerLineEdit->setText(string);
            }
            if(!flm["TPOS"].isEmpty())
            {
                string = QString(flm["TPOS"].front()->toString().toCString(utf)).trimmed();
                ui.discSpinBox->setValue(string.toInt());
            }
        }
    }
}

void DetailsDialog::save()
{
    TagLib::MPEG::File* f = new  TagLib::MPEG::File(m_path.toLocal8Bit().constData());
    TagLib::String::Type type = TagLib::String::Latin1;

    QTextCodec *codec = 0;
    TagLib::Tag *tag = 0;

    if (selectedTag() == TagLib::MPEG::File::ID3v1)
    {
        codec = m_codec_v1;
        tag = f->ID3v1Tag(TRUE);
        if (codec->name().contains("UTF"))
        {
            delete f;
            loadTag();
        }
    }
    if (selectedTag() == TagLib::MPEG::File::ID3v2)
    {
        codec = m_codec_v2;
        tag = f->ID3v2Tag(TRUE);

        if (codec->name().contains("UTF"))
        {
            type = TagLib::String::UTF8;
            if (codec->name().contains("UTF-16"))
                type = TagLib::String::UTF16;
            else if (codec->name().contains("UTF-16LE"))
                type = TagLib::String::UTF16LE;
            else if (codec->name().contains("UTF-16BE"))
                type = TagLib::String::UTF16BE;

            codec = QTextCodec::codecForName ("UTF-8");
            TagLib::ID3v2::FrameFactory *factory = TagLib::ID3v2::FrameFactory::instance();
            factory->setDefaultTextEncoding(type);
            f->setID3v2FrameFactory(factory);
            type = TagLib::String::UTF8;
        }
        //set composer tag
        TagLib::String composer =
                TagLib::String(codec->fromUnicode(ui.composerLineEdit->text()).constData(), type);
        TagLib::ID3v2::Tag *id3v2_tag = dynamic_cast<TagLib::ID3v2::Tag *>(tag);
        if(ui.composerLineEdit->text().isEmpty())
            id3v2_tag->removeFrames("TCOM");
        else if(!id3v2_tag->frameListMap()["TCOM"].isEmpty())
            id3v2_tag->frameListMap()["TCOM"].front()->setText(composer);
        else
        {
            TagLib::ID3v2::TextIdentificationFrame *frame;
            frame = new TagLib::ID3v2::TextIdentificationFrame("TCOM", TagLib::String::Latin1);
            frame->setText(composer);
            id3v2_tag->addFrame(frame);
        }
        //set disc number
        TagLib::String discnumber =
                TagLib::String(QString("%1").arg(ui.discSpinBox->value()).toAscii().constData());
        if(ui.discSpinBox->value() == 0)
            id3v2_tag->removeFrames("TPOS");
        else if(!id3v2_tag->frameListMap()["TPOS"].isEmpty())
            id3v2_tag->frameListMap()["TPOS"].front()->setText(discnumber);
        else
        {
            TagLib::ID3v2::TextIdentificationFrame *frame;
            frame = new TagLib::ID3v2::TextIdentificationFrame("TPOS", TagLib::String::Latin1);
            frame->setText(discnumber);
            id3v2_tag->addFrame(frame);
        }
    }
    if (selectedTag() == TagLib::MPEG::File::APE)
    {
        codec = QTextCodec::codecForName ("UTF-8");
        tag = f->APETag(TRUE);
        type = TagLib::String::UTF8;
    }

    tag->setTitle(TagLib::String(codec->fromUnicode(ui.titleLineEdit->text()).constData(), type));
    tag->setArtist(TagLib::String(codec->fromUnicode(ui.artistLineEdit->text()).constData(), type));
    tag->setAlbum(TagLib::String(codec->fromUnicode(ui.albumLineEdit->text()).constData(), type));
    tag->setComment(TagLib::String(codec->fromUnicode(ui.commentBrowser->toPlainText ())
                                                                     .constData(), type));
    tag->setGenre(TagLib::String(codec->fromUnicode(ui.genreLineEdit->text()).constData(), type));
    tag->setYear(ui.yearSpinBox->value());
    tag->setTrack(ui.trackSpinBox->value());

    f->save(selectedTag(), FALSE);
    delete f;
    loadTag();
}

void DetailsDialog::create()
{
    TagLib::MPEG::File *f = new TagLib::MPEG::File (m_path.toLocal8Bit().constData());
    TagLib::Tag *tag = 0;
    if (selectedTag() == TagLib::MPEG::File::ID3v1)
        tag = f->ID3v1Tag(TRUE);
    else if (selectedTag() == TagLib::MPEG::File::ID3v2)
        tag = f->ID3v2Tag(TRUE);
    else if (selectedTag() == TagLib::MPEG::File::APE)
        tag = f->APETag(TRUE);

    f->save(selectedTag(), FALSE);
    delete f;
    loadTag();
    ui.tagGroupBox->setEnabled(TRUE);
    ui.saveButton->setEnabled(m_rw);
}

void DetailsDialog::deleteTag()
{
    TagLib::MPEG::File *f = new TagLib::MPEG::File (m_path.toLocal8Bit().constData());
    f->strip(selectedTag());
    delete f;
    loadTag();
}

uint DetailsDialog::selectedTag()
{
    if (ui.id3v1RadioButton->isChecked())
        return TagLib::MPEG::File::ID3v1;
    else if (ui.id3v2RadioButton->isChecked())
        return TagLib::MPEG::File::ID3v2;
    else if (ui.apeRadioButton->isChecked())
        return TagLib::MPEG::File::APE;
    return TagLib::MPEG::File::ID3v2;
}

void DetailsDialog::closeEvent (QCloseEvent *)
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("MAD");
    if (ui.id3v1RadioButton->isChecked())
        settings.setValue("current_tag","ID3v1");
    else if (ui.id3v2RadioButton->isChecked())
        settings.setValue("current_tag","ID3v2");
    else if (ui.apeRadioButton->isChecked())
        settings.setValue("current_tag","APE");
    settings.endGroup();
}

void DetailsDialog::showAudioProperties(QMap <QString, QString> p)
{
    QString formattedText;
    formattedText.append("<TABLE>");
    foreach(QString key, p.keys())
    {
        formattedText.append("<tr>");
        formattedText.append("<td>" + key + ":</td> <td style=\"padding-left: 5px; \"><b>"
                             + p.value(key) + "</b></td>");
        formattedText.append("</tr>");
    }
    formattedText.append("</TABLE>");
    ui.propertiesLabel->setText(formattedText);
}
