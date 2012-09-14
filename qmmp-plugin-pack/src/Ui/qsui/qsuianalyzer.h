/***************************************************************************
 *   Copyright (C) 2007-2012 by Ilya Kotov                                 *
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
#ifndef QSUIANALYZER_H
#define QSUIANALYZER_H

#include <QWidget>
#include <QResizeEvent>
#include <qmmp/visual.h>

class QTimer;
class QMenu;
class QActionGroup;


class QSUiAnalyzer : public Visual
{
    Q_OBJECT

public:
    QSUiAnalyzer( QWidget *parent = 0);
    virtual ~QSUiAnalyzer();

    void add(unsigned char *data, qint64 size, int chan);
    void clear();
    void paintEvent( QPaintEvent * );

protected:
    virtual void hideEvent (QHideEvent *);
    virtual void showEvent (QShowEvent *);

public slots:
    void timeout();

private:
    void process(short *l, short *r);
    void draw(QPainter *p);
    QPixmap m_bg;
    QTimer *m_timer;
    int m_fps;
    double *m_intern_vis_data;
    double *m_peaks;
    int *m_x_scale;
    double m_peaks_falloff;
    double m_analyzer_falloff;
    bool m_show_peaks;
    short *m_left_buffer;
    short *m_right_buffer;
    int m_buffer_at;
    int m_cols, m_rows;
    //colors
    QColor m_color1;
    QColor m_color2;
    QColor m_color3;
    QColor m_bgColor;
    QColor m_peakColor;
    QSize m_cell_size;
};


#endif //QSUIANALYZER_H
