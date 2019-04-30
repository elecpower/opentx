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

#define CPN_SDCARD_VERS_FILE           QStringLiteral("opentx.sdcard.version")
#define CPN_SDCARD_REQ_VERSION         QStringLiteral(SDCARD_VERSION)

//  these must be kept in sync with radio/src/sdcard.h
#define CPN_SDCARD_ROOT           "g.profile[g.id()].sdPath()"
#define CPN_CROSSFIRE_PATH        QStringLiteral("CROSSFIRE")
#define CPN_EEPROMS_PATH          QStringLiteral("EEPROM")
#define CPN_FIRMWARES_PATH        QStringLiteral("FIRMWARE")
#define CPN_BITMAPS_PATH          QStringLiteral("IMAGES")
#define CPN_LAYOUTS_PATH          QStringLiteral("LAYOUTS")   //  is this still used?
#define CPN_LOGS_PATH             QStringLiteral("LOGS")
#define CPN_MODELS_PATH           QStringLiteral("MODELS")
#define CPN_SCREENSHOTS_PATH      QStringLiteral("SCREENSHOTS"))
#define CPN_SCRIPTS_PATH          QStringLiteral("SCRIPTS")
#define CPN_SOUNDS_PATH           QStringLiteral("SOUNDS")
#define CPN_SXR_PATH              QStringLiteral("SxR")
#define CPN_THEMES_PATH           QStringLiteral("THEMES")
#define CPN_WIDGETS_PATH          QStringLiteral("WIDGETS")

#define CPN_SOUNDS_LANG_PATH      CPN_SOUNDS_PATH "/{lang}"           //  default
#define CPN_SOUNDS_SYSTEM_PATH    CPN_SOUNDS_LANG_PATH "/SYSTEM"  // might be best to have a function to return since variable see appdata radio setting
#define CPN_SCRIPTS_FUNCS_PATH    CPN_SCRIPTS_PATH "/FUNCTIONS"
#define CPN_SCRIPTS_MIXES_PATH    CPN_SCRIPTS_PATH "/MIXES"
#define CPN_SCRIPTS_TELEM_PATH    CPN_SCRIPTS_PATH "/TELEMETRY"
#define CPN_SCRIPTS_WIZARD_PATH   CPN_SCRIPTS_PATH "/WIZARD"

//  TODO  Full SD card structure here (see radio) and change all localised references throughout Companion
//  TODO  Should really be shared between radio and Companion like may other pieces eg common/src

class SDCardInterface : public QObject
{

    Q_OBJECT

  public:
    enum SDImageRoots {
      SD_IMAGE_ROOT_STD,
      SD_IMAGE_ROOT_CUSTOM
    };

    enum SDFolders {
      SD_FOLDER_ROOT = 0,
      SD_FOLDER_CROSSFIRE,
      SD_FOLDER_EEPROM,
      SD_FOLDER_FIRMWARE,
      SD_FOLDER_IMAGES,
      SD_FOLDER_LAYOUTS,
      SD_FOLDER_LOGS,
      SD_FOLDER_MODELS,
      SD_FOLDER_SCREENSHOTS,
      SD_FOLDER_SCRIPTS,
      SD_FOLDER_SCRIPTS_FUNCTIONS,
      SD_FOLDER_SCRIPTS_MIXES,
      SD_FOLDER_SCRIPTS_TELEMETRY,
      SD_FOLDER_SCRIPTS_WIZARD,
      SD_FOLDER_SOUNDS,
      SD_FOLDER_SOUNDS_LANGUAGE,
      SD_FOLDER_SOUNDS_LANGUAGE_SYSTEM,
      SD_FOLDER_SXR,
      SD_FOLDER_THEMES,
      SD_FOLDER_WIDGETS,
      SD_FOLDER_COUNT
    };

    QString currentVersion();
    bool isStructureCurrent();
    bool hasRootPath(SDImageRoots root);
    QString rootPath(SDImageRoots root);
    QString folderPath(SDFolders folder, SDImageRoots root);
    QString downloadZipUrl();
    QString sourceZipFile();
    QString destZipPath();
    void setLastZipDownload(const QString destPath);
    bool createCustomFolders();
    bool createFolderStructure(SDImageRoots root);
    bool mergeCustomFolder();

  protected:


};


#endif // _SDCARDINTERFACE_H_
