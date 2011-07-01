/***************************************************************************
 *   Copyright (C) 2008-2009 by Ilya Kotov                                 *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Ui Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Ui Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Ui Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef UiFACTORY_H
#define UiFACTORY_H

class QObject;
class QTranslator;
class QDialog;
class QString;
class QWidget;

/*! @brief Helper class to store ui plugin properies.
 */
class UiProperties
{
public:
    /*!
     * Constructor
     */
    UiProperties()
    {
        hasAbout = false;

    }
    QString name;           /*!< File dialog plugin full name */
    QString shortName;      /*!< File dialog short name for internal usage */
    bool hasAbout;          /*!< Should be \b true if plugin has about dialog, otherwise returns \b false */
};
/*! @brief User interface plugin interface.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class UiFactory
{
public:
    /*!
     * Object destructor.
     */
    virtual ~UiFactory() {}
    /*!
     * Returns ui plugin properties.
     */
    virtual const UiProperties properties() const = 0;
    /*!
     * Creates Ui object instance.
     */
    virtual QObject *create() = 0;
    /*!
     * Shows about dialog.
     * @param parent Parent widget.
     */
    virtual void showAbout(QWidget *parent) = 0;
    /*!
     * Creates QTranslator object of the system locale. Should return \b 0 if translation doesn't exist.
     * @param parent Parent object.
     */
    virtual QTranslator *createTranslator(QObject *parent) = 0;
};

Q_DECLARE_INTERFACE(UiFactory, "UiFactory/1.0")
#endif
