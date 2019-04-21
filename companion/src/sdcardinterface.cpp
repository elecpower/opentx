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
#include "appdata.h"
#include "firmwareinterface.h"
#include "process_sd_dwnld.h"
#include "progresswidget.h"

QString SDCardInterface::currentVersion()
{
  QString filepath = g.profile[g.id()].sdPath();
  if (filepath.isNull() | filepath.isEmpty()) {
    QMessageBox::critical(this, CPN_STR_APP_NAME, tr("No SD Card folder set in this profile. Update settings and retry."), QMessageBox::Ok);
    return;
  }

  QString lastSDCardVerString;
  int lastSDCardMajor;
  int lastSDCardMinor;
  int lastSDCardVersion;

  filepath.append("/" + CPN_SDCARD_VERS_FILE);
  QFile file(filepath);
  if (file.exists()) {
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
      QMessageBox::critical(this, CPN_STR_APP_NAME, tr("Cannot open profile SD card version file"), QMessageBox::Ok);
      return;
    }
    else {
      QTextStream in(&file);
      if (in.status()==QTextStream::Ok) {
        lastSDCardVerString = in.readLine();
        if (!(in.status()==QTextStream::Ok)) {
          QMessageBox::critical(this, CPN_STR_APP_NAME, tr("Cannot read profile SD card version file"), QMessageBox::Ok);
          lastSDCardVerString = "";
          return;
        }
        else {
          qDebug() << "SD card version found: " << lastSDCardVerString;
        }
      }
    }
    file.close();
  }
  else {
    qDebug() << "SD card version file not found";
  }

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

QString SDCardInterface::downloadUrl()
{
  QString url = g.openTxCurrentDownloadBranchUrl() % QStringLiteral("sdcard/");
  QString fwType = g.profile[g.id()].fwType();
  QStringList list = fwType.split("-");
  QString firmware = QString("%1-%2").arg(list[0]).arg(list[1]);
  if (g.boundedOpenTxBranch() != AppData::BRANCH_NIGHTLY_UNSTABLE) {
    url.append(QString("%1/").arg(firmware));
  }
  // QDesktopServices::openUrl(url);

  //  hard code until find a way to determine from fw or app setting
  //  or return list of files and allow user to select or
  //  use the version to find a match as the url points to the folder before we try and add the zip file name
  //  use the version check code model
  QString fwFamily = "taranis-x9";
  // eg sdcard-taranis-x9-2.2V0018.zip
  QString sdZipSrcFile = QString("sdcard-%1-%2.zip").arg(fwFamily).arg(lastSDCardVerString);
  url.append(sdZipSrcFile);

  // uncomment for testing
  url.replace("2.3","2.2");

  return url;
}

bool SDCardInterface::isImageCurrent()
{
  //  once the sd card is downloaded and installed it is not easy to determine the radio it is for and not all contents are the same eg lua scripts
  //  so should the user change the radio type in the profile after an sd card is downloaded there could be a mismatch
  //  make a suggestion to Devs to include radio in version  or another file and store with FW in appdata
  fw.splitSDCardVersion(lastSDCardVerString, lastSDCardMajor, lastSDCardMinor, lastSDCardVersion);
  if (!(fwSDCardVersion > lastSDCardVersion)) {
  }

  return true;
}

QString SDCardInterface::zipSourceFile()
{
  QString file = "";
  return file
}

QString SDCardInterface::defaultDestPath()
{
  return g.lastSDDir() % "/" % zipSourceFile();
}

void SDCardInterface::downloadDest(const QString destPath)
{
  g.profile[g.id()].sdVersion(currentVersion());
  g.profile[g.id()].sdZipSrcFile(zipFile());
  g.profile[g.id()].sdZipDestFile(QFileInfo(destPath).fileName());
  g.lastSDDir(QFileInfo(destPath).dir().absolutePath());
}
