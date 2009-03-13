/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
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
#ifndef SOUNDCORE_H
#define SOUNDCORE_H


#include <QObject>
#include <QString>

#include "decoder.h"
#include "output.h"
#include "visual.h"
#include "qmmp.h"

class QIODevice;
class VolumeControl;

/*!
 *  @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class SoundCore : public QObject
{
    Q_OBJECT
public:
    SoundCore(QObject *parent = 0);

    ~SoundCore();

    /*!
     * Returns length in seconds
     */
    qint64 totalTime() const;

    /*!
     * Sets equalizer settings. Each item of \p bands[] and \p reamp should be
     *  \b -20.0..20.0
     */
    void setEQ(double bands[10], double preamp);

    /*!
     * Enables equalizer if \p on is \b TRUE or disables it if \p on is \b FALSE
     */
    void setEQEnabled(bool on);

    int leftVolume();
    int rightVolume();
    bool softwareVolume();

    qint64 elapsed();
    int bitrate();
    int frequency();
    int precision();
    int channels();

    /*!
     * Returns the current state.
     *
     * \return the state of the object.
     */
    Qmmp::State state() const;

    QMap <Qmmp::MetaData, QString> metaData();

    QString metaData(Qmmp::MetaData key);

    /*!
     * Returns a pointer to the SoundCore instance.
     */
    static SoundCore* instance();


public slots:

    void setSoftwareVolume(bool);

    /*!
     * Sets volume.
     * \p left - volume of the left channel.
     * \p right - volume of the right channel.
     * \b left and \b right should be \b 0..100.
     */
    void setVolume(int left, int right);

    /*!
     *  This function plays file with the given path \p source.
     *
     *  \return \b TRUE if playback started successful or source is not a local file.
     *  \return \b FALSE otherwise.
     */
    bool play(const QString &source);

    /*!
     *  Stops playback
     */
    void stop();

    /*!
     *  Pauses/resumes playback
     */
    void pause();

    /*!
    *  This function sets the current play position to \p pos.
    */
    void seek(qint64 pos);

    /*!
    *  This function returns file path or stream url.
    */
    const QString url();

signals:

    /*!
     * This signal is emited when the stream reader fills it's buffer.
     * The argument \b progress indicates the current percentage of buffering completed
     */

    void bufferingProgress(int progress);
    void elapsedChanged(qint64 time);
    void bitrateChanged(int bitrate);
    void frequencyChanged(int frequency);
    void precisionChanged(int precision);
    void channelsChanged(int channels);
    void metaDataChanged ();
    void stateChanged (Qmmp::State newState);
    void finished();
    void volumeChanged(int left, int right);

private slots:
    bool decode();

private:
    Decoder* m_decoder;
    DecoderFactory* m_factory;
    //QUrl m_url;
    QString m_source;
    Output* m_output;
    QIODevice* m_input;
    uint m_error;
    bool m_paused;
    bool m_useEQ;
    bool m_update;
    bool m_block;
    double m_preamp;
    double m_bands[10];
    Visual *m_vis;
    QList <Visual*> m_visuals;
    QWidget *m_parentWidget;
    static SoundCore* m_instance;
    StateHandler *m_handler;
    VolumeControl *m_volumeControl;
};

#endif
