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

#include "sdcardinterface.h"
#include "constants.h"
#include "appdata.h"
#include "firmwareinterface.h"
#include "process_sd_dwnld.h"
#include "progresswidget.h"

//  TODO  merge sd sync and refactor Companion code to use this class after PR accepted

QString SDCardInterface::rootPath(SDImageRoots root)
{
  QString path;
  switch (root) {
    case SD_IMAGE_ROOT_STD:
      path = g.profile[g.id()].sdPath();
      break;
    case SD_IMAGE_ROOT_CUSTOM:
      path = g.profile[g.id()].sdPathCustom();
      break;
    default:
      qDebug() << "No mapping for root folder id:" << root;
  }
  path.trimmed();
  qDebug() << "Path:" << path;
  return path;
}

bool SDCardInterface::hasRootPath(SDImageRoots root)
{
  return rootPath(root).isEmpty() ? false : true;
}

QString SDCardInterface::folderPath( SDImageRoots root, SDFolders folder)
{
  //  TODO  Full SD card structure here (see radio) and change all localised references throughout Companion
  //  TODO  Should really be shared between radio and Companion like may other pieces eg common/src

  //  these folders are expected to exist though not all needed for all radios. sub folders may exist eg for lua scripts

  QString path = rootPath(root) % "/";

  switch (folder) {
    case SD_ROOT:
      break;
    case SD_CROSSFIRE:
      path.append("CROSSFIRE");
      break;
    case SD_EEPROM:
      path.append("EEPROM");
      break;
    case SD_FIRMWARE:
      path.append("FIRMWARE");
      break;
    case SD_IMAGES:
      path.append("IMAGES");
      break;
    case SD_LAYOUTS:
      path.append("LAYOUTS");
      break;
    case SD_LOGS:
      path.append("LOGS");
      break;
    case SD_MODELS:
      path.append("MODELS");
      break;
    case SD_SCREENSHOTS:
      path.append("SCREENSHOTS");
      break;
    case SD_SCRIPTS:
      path.append("SCRIPTS");
      break;
    case SD_SCRIPTS_FUNCTIONS:
      path.append("SCRIPTS/FUNCTIONS");
      break;
    case SD_SCRIPTS_MIXES:
      path.append("SCRIPTS/MIXES");
      break;
    case SD_SCRIPTS_TELEMETRY:
      path.append("SCRIPTS/TELEMETRY");
      break;
    case SD_SCRIPTS_WIZARD:
      path.append("SCRIPTS/WIZARD");
      break;
    case SD_SOUNDS:
      path.append("SOUNDS");
      break;
    case SD_SOUNDS_LANGUAGE:
      path.append("SOUNDS/" % g.locale());
      break;
    case SD_SOUNDS_LANGUAGE_SYSTEM:
      path.append("SOUNDS/"  % g.locale() % "/SYSTEM");
      break;
    case SD_SXR:
      path.append("SxR");
      break;
    case SD_THEMES:
      path.append("THEMES");
      break;
    case SD_WIDGETS:
      path.append("WIDGETS");
      break;
    default:
      qDebug() << "No mapping for folder id:" << folder;
  }
  qDebug() << "Path:" << path;
  return path;
}

bool SDCardInterface::isRadioFamilyCurrent(const QString newFamily)
{
  return newFamily == currentRadioFamily();
}

QString SDCardInterface::readFileRecord(const QString path)
{
  QString str;
  QFile file(path);
  if (file.exists()) {
    if (file.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream in(&file);
      if (in.status() == QTextStream::Ok) {
        str = in.readLine();
        if (!(in.status() == QTextStream::Ok)) {
          str = "";
        }
      }
      file.close();
    }
  }
  return str;
}

QString SDCardInterface::currentRadioFamily()
{
  return readFileRecord(rootPath(SD_IMAGE_ROOT_STD) % "/" % CPN_SDCARD_FAMILY_FILE);
}

QString SDCardInterface::currentVersion()
{
  if (hasRootPath(SD_IMAGE_ROOT_STD)) {
    QMessageBox::critical(this, CPN_STR_APP_NAME, tr("No SD Card folder set in this profile. Update settings and retry."), QMessageBox::Ok);
    return;
  }

  int lastSDCardMajor;
  int lastSDCardMinor;
  int lastSDCardVersion;

  QString lastSDCardVerString = readFileRecord(rootPath(SD_IMAGE_ROOT_STD) % "/" % CPN_SDCARD_VERS_FILE);

  int fwSDCardVersion = 0;
  filepath = g.profile[g.id()].fwName();
  FirmwareInterface fw(filepath);
  if (!filepath.isNull()) {
    QFile fwfile(filepath);
    if (fwfile.exists()) {
      fwSDCardVersion = fw.getSDCardVersion();
      if (!fwSDCardVersion) {
        QMessageBox::warning(this, CPN_STR_APP_NAME, tr("Firmware not reporting it supports an SD card!"), QMessageBox::Ok);
        return;
      }
    }
  }
  if (!fwSDCardVersion) {
    int reqSDCardMajor;
    int reqSDCARDMinor;
    fw.splitSDCardVersion(CPN_REQ_SDCARD_VERSION, reqSDCardMajor, reqSDCARDMinor, fwSDCardVersion);
    qDebug() << "Firmware has not been download. Trying Companion SD card version: " << COMPANION_REQ_SDCARD_VERSION;
  }
  return vers;
}

bool SDCardInterface::isStructureCurrent()
{
  //  once the sd card is downloaded and installed it is not easy to determine the radio it is for and not all contents are the same eg lua scripts
  //  so should the user change the radio type in the profile after an sd card is downloaded there could be a mismatch
  //  make a suggestion to Devs to include radio in version  or another file and store with FW in appdata
  fw.splitSDCardVersion(lastSDCardVerString, lastSDCardMajor, lastSDCardMinor, lastSDCardVersion);
  if (!(fwSDCardVersion > lastSDCardVersion)) {
  }

  return true;
}

QString SDCardInterface::downloadZipUrl()
{
  QString url = g.openTxCurrentDownloadBranchSDCardUrl() % "/");

  if (g.boundedOpenTxBranch() != AppData::BRANCH_NIGHTLY_UNSTABLE)
    url.append((QStringList(g.profile[g.id()].fwType().split("-").mid(0, 2)).join("-")) % "/");

  url.append(sourceZipFile());

  url.replace("2.3","2.2");       //  TODO  new version does not exist so force to existing - remove ater testing

  qDebug() << "URL:" << url;
  return url;
}

QString SDCardInterface::radioFamily()
{
  //  TODO this probably should be moved to Firmware as it is related and
  //        to ensure it gets kept in sync with radio changes
  QString fwFamily = "";
  QString fwid = QStringList(g.profile[g.id()].fwType().split("-").mid(0, 2)).join("-");

  if (fwid == "opentx-x7" || fwid == "opentx-xlite" || fwid == "opentx-xlites")
      fwFamily = "taranis-x7";
  else if (fwid == "opentx-x9d" || fwid == "opentx-x9d+" || fwid == "opentx-x9e")
      fwFamily = "taranis-x9";
  else if (fwid == "opentx-x10" || fwid == "opentx-x12s")
      fwFamily = "horus";
  else if (fwid == "opentx-9xrpro" || fwid == "opentx-ar9x" || fwid == "opentx-sky9x")
      fwFamily = "9xarm";

  if (fwFamily.isEmpty()) {
    qDebug() << "Error - Unknown firmware id:" << fwid;

  return fwFamily;
}

QString SDCardInterface::sourceZipFile()
{
  return QString("sdcard-%1-%2.zip").arg(radioFamily()).arg(lastSDCardVerString);
}

QString SDCardInterface::destZipPath()
{
  return g.profile[g.id()].sdLastZipFolder() % "/" % sourceZipFile();
}

void SDCardInterface::setLastZip(const QString destPath)
{
  g.profile[g.id()].sdLastZipVersion(currentVersion());
  g.profile[g.id()].sdLastZipSrcFile(sourceZipFile());
  g.profile[g.id()].sdLastZipDestFile(QFileInfo(destPath).fileName());
  g.profile[g.id()].sdLastZipFolder(QFileInfo(destPath).dir().absolutePath());
}

bool SDCardInterface::createCustomFolders()
{
  return createFolderStructure(SD_IMAGE_ROOT_CUSTOM);
}

bool SDCardInterface::createFolderStructure(SDImageRoots root)
{
  QString path;
  for (int x = 0; x < SD_ENUM_COUNT; x++) {
    path = folderPath(root, x);
    if (!QDir(path).exists() {
      if(!QDir().mkpath(path))) {
          qDebug() << "Failed to create folder:" << path;
          return false;
      }
      else {
        qDebug() << "Created folder:" << path;
      }
    }
  }
  return true;
}

bool SDCardInterface::mergeCustomFolder()
{
  return true;
}
