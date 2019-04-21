/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SDCARDINTERFACE_H_
#define _SDCARDINTERFACE_H_

#include "version.h"

#include <QObject>
#include <QString>

#define CPN_SDCARD_VERS_FILE        QStringLiteral("opentx.sdcard.version")
#define CPN_REQ_SDCARD_VERSION      QStringLiteral(SDCARD_VERSION)



//  TODO  Full SD card structure here (see radio) and change all localised references throughout Companion
//  TODO  Should really be shared between radio and Companion like may other pieces eg common/src

class SDCardInterface : public QObject
{

    Q_OBJECT

  public:
    SDCardInterface();
    QString currentVersion();
    bool isImageCurrent();
    QString downloadUrl();
    QString zipSourceFile();
    void downloadDest(const QString destPath);

  protected:


};


#endif // _SDCARDINTERFACE_H_
