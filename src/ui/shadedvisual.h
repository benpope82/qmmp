/***************************************************************************
 *   Copyright (C) 2007-2010 by Ilya Kotov                                 *
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
#ifndef SHADEDVISUAL_H
#define SHADEDVISUAL_H

#include <QPainter>
#include <qmmp/visual.h>
#include <qmmp/buffer.h>

class QTimer;
class QPixmap;
class Skin;
//class VisualNode;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class ShadedVisual : public Visual
{
    Q_OBJECT
public:
    ShadedVisual(QWidget *parent = 0);

    ~ShadedVisual();

    void add(unsigned char *data, qint64 size, int chan);
    void clear();

    void paintEvent (QPaintEvent *);
    void hideEvent (QHideEvent *);
    void showEvent (QShowEvent *);

public slots:
    void timeout();

private slots:
    void updateSkin();

private:
    void process (short *l, short *r);
    void draw (QPainter *);
    Skin *m_skin;
    QTimer *m_timer;
    QPixmap m_pixmap;
    short *m_left_buffer;
    short *m_right_buffer;
    int m_buffer_at;
    double m_l, m_r;
    int m_ratio;

};

#endif
