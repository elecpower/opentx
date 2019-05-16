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
#include "eeprominterface.h"
#include "firmwareinterface.h"
#include "process_sdcard.h"
#include "progresswidget.h"

void SDCardInterface::SDCardInterface()
{
}

int SDCardInterface::versionToIndex(const QString & vers)
{
  int result = 0;
  QStringList parts;

  if (vers.contains("V")) {
    parts = str.split("V");
    result = parts[1].toInt();
  }
  if (parts[0].contains(".")) {
    parts = parts[0].split(".");
    result += parts[1].toInt() * 10000;
    result += parts[0].toInt() * 1000000;
  }
  return result;
}

QString indexToVersion(const int index)
{
  int idx = index;
  int revision = idx % 10000;
  idx /= 10000;
  int minor = idx % 100;
  int major = idx / 100;
  //  format eg 2.2V0018
  return QString("%1.%2V%3").arg(major).arg(minor).arg(revision, 4, 10, QLatin1Char('0'));
}


QString SDCardInterface::getIdFromType(const QString & type)
{
  QStringList strl = type.split("-");
  if (!(strl.size() < 2))
    return strl.mid(0, 2)).join("-");
  else
    return "";
}

QString SDCardInterface::getFamilyFromId(const QString & id)
{
  QString family = "";

  if (id == "opentx-x7" || id == "opentx-xlite" || id == "opentx-xlites")
      family = "taranis-x7";
  else if (id == "opentx-x9d" || id == "opentx-x9d+" || id == "opentx-x9e")
      family = "taranis-x9";
  else if (id == "opentx-x10" || id == "opentx-x12s")
      family = "horus";
  else if (id == "opentx-9xrpro" || id == "opentx-ar9x" || id == "opentx-sky9x")
      family = "9xarm";

  if (family.isEmpty())
    qDebug() << "Error - No family defined for firmware id:" << id;

  return family;
}

QString SDCardInterface::getFamilyFromType(const QString & type)
{
  return getFamilyFromId(getIdFromType(type));
}

void SDCardInterface::setCurrentFirmware(const QString & version, const QString & type, const QString & reqSDVersion)
{
  m_fwVersion = version;
  m_fwVersionId = version2index(m_fwVersionId);
  m_fwType = type;
  m_fwId = getIdFromType(type);
  m_fwFamily = getFamilyFromId(m_fwId);
  m_fwReqSDVersion = reqSDVersion;
  m_fwReqSDVersionId = versionToIndex(m_fwReqSDVersion);
}

QString SDCardInterface::profileFirmwareType()
{
  return g.profile[g.id()].fwType();
}

QString SDCardInterface::profileFirmwareName()
{
  return g.profile[g.id()].fwName();
}

void SDCardInterface::getLastFirmware()
{
  Firmware * fw = Firmware::getCurrentVariant();
  QString filename = profileFirmwareName();
  FirmwareInterface fwif = FirmwareInterface(filename);
  setCurrentFirmware(fw->getId(), filename, fwif.getReqSDVersion());
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

QString SDCardInterface::installedFamily()
{
  return readFileRecord(rootPath(SD_IMAGE_ROOT_STD) % "/" % CPN_SDCARD_FAMILY_FILE);
}

QString SDCardInterface::installedVersion()
{
  return readFileRecord(rootPath(SD_IMAGE_ROOT_STD) % "/" % CPN_SDCARD_VERS_FILE);
}

bool SDCardInterface::isInstalledCompatible(const QString & family, const QString & version)
{
  return (installedFamily() == family) && (installedVersion() == version);
}

bool SDCardInterface::isFamilyCurrent(const QString & family)
{
  return family == m_fwFamily;
}

bool SDCardInterface::isVersionCurrent(const QString & version)
{
  return version == CPN_SDCARD_REQ_VERSION;
}

bool SDCardInterface::isCurrent(const QString & family, const QString & version)
{
  return (isFamilyCurrent(family) && isVersionCurrent(version));
}

bool SDCardInterface::isUpdateAvailable()
{
  return versionToIndex(installedVersion() < versionToIndex(CPN_SDCARD_REQ_VERSION);
}

void SDCardInterface::setCurrent(const QString & family, const QString & version)
{
  m_Family = family;
  m_version = version;
  m_versionId = versionToIndex(m_version);
}

QString SDCardInterface::downloadZipUrl(const QString & version)
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
  return QString("sdcard-%1-%2.zip").arg(m_Family).arg(m_Version);
}

void SDCardInterface::setLastZip(const QString & destPath)
{
  g.profile[g.id()].sdLastZipVersion(m_Version);
  g.profile[g.id()].sdLastZipSrcFile(sourceZipFile());
  g.profile[g.id()].sdLastZipDestFile(QFileInfo(destPath).fileName());
  g.profile[g.id()].sdLastZipFolder(QFileInfo(destPath).dir().absolutePath());
}

QString SDCardInterface::defaultDestZipPath()
{
  return g.profile[g.id()].sdLastZipFolder() % "/" % sourceZipFile();
}

QString SDCardInterface::lastZipFilePath()
{
  return g.profile[g.id()].sdLastZipFolder % "/" % g.profile[g.id()].sdLastZipDestFile();
}

bool SDCardInterface::lastZipFileExists()
{
  return QFile(lastZipFilePath()).exists();
}

void SDCardInterface::updateSDImage()
{
  ProgressDialog progressDialog(this, tr("Install SD card image"), CompanionIcon("save.png"));
  bool result = installSDImage(progressDialog.progress());
  if (!result && !progressDialog.isEmpty())
    progressDialog.exec();
}

bool SDCardInterface::createCustomFolders()
{
  return createFolderStructure(SD_IMAGE_ROOT_CUSTOM);
}

bool SDCardInterface::createFolderStructure(SDImageRoots root)
{
  QString path;
  for (int x=0; x<SD_ENUM_COUNT; x++) {
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

void SDCardInterface::writeFamilyFile()
{
  if (!fileWriteFile(rootPath(SD_IMAGE_ROOT_STD) % "/" % CPN_SDCARD_FAMILY_FILE, m_fwFamily))
    qDebug() << "Error - failed to write SD Family file";
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
