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
#include "version.h"
#include "helpers.h"
#include "firmwareinterface.h"
#include "process_sd_dwnld.h"
#include "progresswidget.h"

void SDCardInterface::SDCardInterface()
{
  setFirmwareType(profileFirmwareType());
  setVersion(CPN_SDCARD_REQ_VERSION);
}

void SDCardInterface::SDCardInterface(const QString & fwType)
{
  setFirmwareType(fwType);
  setVersion(CPN_SDCARD_REQ_VERSION);
}

void SDCardInterface::SDCardInterface(const QString & fwType, const QString & version)
{
  setFirmwareType(fwType);
  setVersion(version);
}

void SDCardInterface::setVersion(const QString & version)
{
  m_Version = version;
}

void SDCardInterface::setFirmware(const QString & fwType, const QString & reqSDVersion)
{
  m_fwType = fwType;
  m_fwId = getFirmwareIdFromType(fwType);

  if (m_fwId == "opentx-x7" || m_fwId == "opentx-xlite" || m_fwId == "opentx-xlites")
      m_reqSDFlavour = "taranis-x7";
  else if (m_Id == "opentx-x9d" || m_fwId == "opentx-x9d+" || m_fwId == "opentx-x9e")
      m_reqSDFlavour = "taranis-x9";
  else if (m_Id == "opentx-x10" || m_fwId == "opentx-x12s")
      m_reqSDFlavour = "horus";
  else if (m_Id == "opentx-9xrpro" || m_fwId == "opentx-ar9x" || m_fwId == "opentx-sky9x")
      m_reqSDFlavour = "9xarm";

  if (m_fwFlavour.isEmpty())
    qDebug() << "Error - No SD card flavour defined for firmware id:" << m_fwId;

  m_reqSDVersion = reqSDVersion;
  m_reqSDVersionId = Helpers::reqSDVersionToIndex(reqSDVersion);
}

QString SDCardInterface::getFirmwareIdFromType(const QString & fwType)
{
  return QStringList(fwType.split("-").mid(0, 2)).join("-");
}

QString SDCardInterface::profileFirmwareType()
{
  return g.profile[g.id()].fwType();
}

SDInfo SDCardInterface::getFirmwareInfo()
{
  //  check latest version downloaded from appdata
  //  what if it is not the version in the version.h
  //  if not version.h then cannot work out Req SD version
  //  probably should be saving req sd version as part of registering the firmware download
  //    opentx-x9d+-2.2.3
  //  req SD ver 2.2V0018
  SDInfo info;
  FirmwareInterface * fw = FirmwareInterface();

  return info;
}

QString SDCardInterface::folderPath(SDImageRoots root, SDFolders folder)
{
  //  these folders are minimum expected ones to exist though not all needed for all radios
  //  sub folders may exist eg for lua scripts
  QString path = rootPath(root) % QStringLiteral("/");

  switch (folder) {
    case SD_ROOT:
      break;
    case SD_CROSSFIRE:
      path.append(QStringLiteral("CROSSFIRE"));
      break;
    case SD_EEPROM:
      path.append(QStringLiteral("EEPROM"));
      break;
    case SD_FIRMWARE:
      path.append(QStringLiteral("FIRMWARE"));
      break;
    case SD_IMAGES:
      path.append(QStringLiteral("IMAGES"));
      break;
    case SD_LAYOUTS:
      path.append(QStringLiteral("LAYOUTS"));
      break;
    case SD_LOGS:
      path.append(QStringLiteral("LOGS"));
      break;
    case SD_MODELS:
      path.append(QStringLiteral("MODELS"));
      break;
    case SD_SCREENSHOTS:
      path.append(QStringLiteral("SCREENSHOTS"));
      break;
    case SD_SCRIPTS:
      path.append(QStringLiteral("SCRIPTS"));
      break;
    case SD_SCRIPTS_FUNCTIONS:
      path.append(QStringLiteral("SCRIPTS/FUNCTIONS"));
      break;
    case SD_SCRIPTS_MIXES:
      path.append(QStringLiteral("SCRIPTS/MIXES"));
      break;
    case SD_SCRIPTS_TELEMETRY:
      path.append(QStringLiteral("SCRIPTS/TELEMETRY"));
      break;
    case SD_SCRIPTS_WIZARD:
      path.append(QStringLiteral("SCRIPTS/WIZARD"));
      break;
    case SD_SOUNDS:
      path.append(QStringLiteral("SOUNDS"));
      break;
    case SD_SOUNDS_LANGUAGE:
      path.append("SOUNDS/" % g.locale());
      break;
    case SD_SOUNDS_LANGUAGE_SYSTEM:
      path.append("SOUNDS/"  % g.locale() % "/SYSTEM");
      break;
    case SD_SXR:
      path.append(QStringLiteral("SxR"));
      break;
    case SD_THEMES:
      path.append(QStringLiteral("THEMES"));
      break;
    case SD_WIDGETS:
      path.append(QStringLiteral("WIDGETS"));
      break;
    default:
      qDebug() << "No mapping for folder id:" << folder;
  }
  qDebug() << "Path:" << path;
  return path;
}

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

QString SDCardInterface::installedFlavour()
{
  return readFileRecord(rootPath(SD_IMAGE_ROOT_STD) % "/" % CPN_SDCARD_FLAVOUR_FILE);
}

QString SDCardInterface::installedVersion()
{
  return readFileRecord(rootPath(SD_IMAGE_ROOT_STD) % "/" % CPN_SDCARD_VERS_FILE);
}

bool SDCardInterface::isUpdateAvailable()
{
  bool result = false;
  SDVersion instver = splitVersionString(installedVersion());
  SDVersion currver = splitVersionString(currentVersion());

  if (instver.major < currver.major) {
    result true;
  }
  else if (instver.minor < currver.minor) {
    result true;
  }
  else if (instver.revision < currver.revision) {
    result true;
  }
  return result;
}

bool SDCardInterface::isFlavourCurrent(const QString & flavour)
{
  return flavour == m_fwFlavour;
}

bool SDCardInterface::isVersionCurrent(const QString & version)
{
  return version == m_Version;
}

bool SDCardInterface::isCurrent(const QString & flavour, const QString & version)
{
  return (isFlavourCurrent(flavour) && isVersionCurrent(version));
}

bool SDCardInterface::isCompatible()    //  TODO is this the same as isCurrent????
{
  return true;
}

QString SDCardInterface::downloadZipUrl()
{
  QString url = g.openTxCurrentDownloadBranchSDCardUrl() % "/");

  if (g.boundedOpenTxBranch() != AppData::BRANCH_NIGHTLY_UNSTABLE)
    url.append(m_Id % "/");

  url.append(sourceZipFile());

  url.replace("2.3","2.2");       //  TODO  new version does not exist so force to existing - remove ater testing

  qDebug() << "URL:" << url;
  return url;
}

QString SDCardInterface::sourceZipFile()
{
  return QString("sdcard-%1-%2.zip").arg(m_fwFlavour).arg(m_sdRequired);
}

QString SDCardInterface::destZipPath()
{
  return g.profile[g.id()].sdLastZipFolder() % "/" % sourceZipFile();
}

void SDCardInterface::setLastZip(const QString & destPath)
{
  g.profile[g.id()].sdLastZipVersion(m_Version);
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
  //  TODO merge custom folders and files into standard image

  return true;
}

void SDCardInterface::writeFlavourFile()
{
  if (!fileWriteFile(rootPath(SD_IMAGE_ROOT_STD) % "/" % CPN_SDCARD_FLAVOUR_FILE, m_fwFlavour))
    qDebug() << "Error - failed to write SD Flavour file";
}

QString SDCardInterface::fileReadRecord(const QString & path)
{
  QString data;
  QFile file(path);
  if (file.exists()) {
    if (file.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream in(&file);
      if (in.status() == QTextStream::Ok) {
        data = in.readLine();
        if (!(in.status() == QTextStream::Ok)) {
          data = "";
        }
      }
      file.close();
    }
  }
  return data;
}

bool SDCardInterface::fileWriteRecord(const QString & path, const QString & data)
{
  bool result = false;
  QFile file(path);
  if (file.open(QFile::WriteOnly | QFile::Text)) {
    QTextStream out(&file);
    if (out.status() == QTextStream::Ok) {
      out << data;
      if ((out.status() == QTextStream::Ok)) {
        out.flush();
        if ((out.status() == QTextStream::Ok)) {
          if (file.close()) {
            result = true;
          }
        }
      }
    }
    else {
      file.close();
    }
  }
  return result;
}
