/***************************************************************************
 *   Copyright (C) 2009-2013 by Ilya Kotov                                 *
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qmmp/qmmp.h>
#include "ui_mainwindow.h"

class QSlider;
class QLabel;
class QMenu;
class UiHelper;
class PlayListModel;
class MediaPlayer;
class SoundCore;
class PlayListManager;
class GeneralHandler;
class VisualMenu;
class PositionSlider;
class KeyboardManager;
class QSUiAnalyzer;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void addDir();
    void addFiles();
    void addUrl();
    void updatePosition(qint64 pos);
    void seek();
    void showState(Qmmp::State);
    void showBitrate(int);
    void updateTabs();
    void addPlaylist();
    void removePlaylist();
    void removePlaylistWithIndex(int);
    void addTab(int);
    void removeTab(int);
    void renameTab();
    void aboutUi();
    void about();
    void toggleVisibility();
    void showSettings();
    void updateVolumeIcon();
    void jumpTo();
    void playPause();
    void readSettings();
    void showTabMenu(QPoint pos);
    void savePlayList();
    void loadPlayList();
    void on_tabWidget_tabCloseRequested(int index);
    void showBuffering(int percent);
    void showEqualizer();
    void forward();
    void backward();
    void showMetaData();
    void setTitleBarsHidden(bool hidden);

private:
    void closeEvent(QCloseEvent *);
    void createActions();
    void writeSettings();
    QString m_lastDir;
    PlayListManager *m_pl_manager;
    Ui::MainWindow m_ui;
    MediaPlayer *m_player;
    PositionSlider *m_slider;
    QLabel *m_timeLabel;
    SoundCore *m_core;
    QLabel *m_statusLabel;
    VisualMenu *m_visMenu;
    UiHelper *m_uiHelper;
    QMenu *m_pl_menu;
    QMenu *m_tab_menu;
    QSlider *m_volumeSlider;
    QAction *m_volumeAction;
    KeyboardManager *m_key_manager;
    QSUiAnalyzer *m_analyzer;
    int m_balance;
    bool m_update;
    bool m_hideOnClose;

};

#endif