//==========================================================================
// Utility for QML
//==========================================================================
//  Copyright (c) 2013-Today DogRatIan.  All rights reserved.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//==========================================================================
// Naming conventions
// ~~~~~~~~~~~~~~~~~~
//                Class : Leading C
//               Struct : Leading T
//             Constant : Leading K
//      Global Variable : Leading g
//    Function argument : Leading a
//       Local Variable : All lower case
//==========================================================================
#include <QDebug>
#include <QClipboard>
#include <QApplication>

#include "qml_utils.h"

//==========================================================================
// Constants
//==========================================================================

//==========================================================================
// Constructor
//==========================================================================
CQmlUtils::CQmlUtils (QObject *aParent) :
    QObject(aParent)
{
}

//==========================================================================
// Destroyer
//==========================================================================
CQmlUtils::~CQmlUtils (void)
{
}

//==========================================================================
// Copy Text to Clipboard
//==========================================================================
void CQmlUtils::textToClipboard (QString aStr)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText (aStr);
}

//==========================================================================
//==========================================================================
QString CQmlUtils::intToString (QString aFormat, int aValue)
{
    QString str;
    str.sprintf(aFormat.toUtf8().data(), aValue);

    return str;
}


