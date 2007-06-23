/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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
#ifndef PLAYLISTTITLEBAR_H
#define PLAYLISTTITLEBAR_H

#include "playlist.h"
#include "pixmapwidget.h"
#include "mainwindow.h"

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/
class Skin;
class MainWindow;

class PlayListTitleBar : public PixmapWidget
{
Q_OBJECT
public:
    PlayListTitleBar(QWidget *parent = 0);

    ~PlayListTitleBar();

    void setActive(bool);

private slots:
    void updateSkin();

private:
    void drawPixmap(int);
    Skin *m_skin;
    QPoint pos;
    bool m_active;
    PlayList* m_pl;
    MainWindow* m_mw;

protected:
    void resizeEvent(QResizeEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
};

#endif
