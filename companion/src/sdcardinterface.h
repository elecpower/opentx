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

#define CPN_SDCARD_FLAVOUR_FILE         QStringLiteral("opentx.sdcard.flavour")
#define CPN_SDCARD_VERS_FILE           QStringLiteral("opentx.sdcard.version")
#define CPN_SDCARD_REQ_VERSION         QStringLiteral(SDCARD_VERSION)

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

    void SDCardInterface();
    int reqSDVersionToIndex(const QString & version);
    QString getIdFromType(const QString & type);
    QString getFlavourFromId(const QString & id);
    QString getFlavourFromType(const QString & type);

    void setCurrentFirmware(const QString & version, const QString & type, const QString & reqSDVersion);

    QString profileFirmwareId();
    QString firmwareFlavour();
    QString firmwareVersion();
    QString firmwareReqSDVersion();

    bool hasRootPath(SDImageRoots root);
    QString rootPath(SDImageRoots root);
    QString folderPath(SDFolders folder, SDImageRoots root);

    bool isFlavourCurrent(const QString & flavour);
    bool isVersionCurrent(const QString & version);
    bool isCurrent(const QString & flavour, const QString & version);
    QString installedFlavour();
    QString installedVersion();
    bool isInstalledCompatible():
    bool isUpdateAvailable();

    void setCurrent(const QString & flavour, const QString & version);

    QString downloadZipUrl();
    QString sourceZipFile();
    QString destZipPath();
    void setLastZip(const QString & destPath);

    void updateSDImage(const QString & version, bool download = true);
    bool createFolderStructure(SDImageRoots root);
    bool createCustomFolders();
    bool mergeCustomFolder();
    void writeFamilyFile();

  protected:
    QString fileReadRecord(const QString & path);
    bool fileWriteRecord(const QString & path, const QString & data);

    QObject * parent;
    QString m_fwType;
    QString m_fwId;
    int m_fwVersionId;
    QString fwFlavour;
    QString m_fwReqSDVersion;
    int m_fwReqSDVersionId;
    QString m_flavour;
    QString m_version;
    int m_versionId;
};


#endif // _SDCARDINTERFACE_H_
