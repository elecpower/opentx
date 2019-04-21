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

#include "process_sd_dwnld.h"
#include "constants.h"
#include "progresswidget.h"
#include <QEventLoop>
#include <QFile>
#include <QMessageBox>
#include <QTimer>

#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"
#undef MINIZ_HEADER_FILE_ONLY

ProcessSDDwnld::ProcessSDDwnld(const QString &source, const QString &destination, ProgressWidget *progress):
progress(progress),
source(source),
destination(destination),
result(true)
{
}

bool ProcessSDDwnld::run()
{
  progress->lock(true);
  // progress->setInfo(tr("Copying file..."));

  QEventLoop loop;
  connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
  QTimer::singleShot(500, this, SLOT(onTimer()));
  loop.exec();

  return result;
}

void ProcessSDDwnld::onTimer()
{
  char buf[BLKSIZE];

  QFile sourceFile(source);
  int blocks = (sourceFile.size() + BLKSIZE - 1) / BLKSIZE;
  progress->setMaximum(blocks-1);

  if (sourceFile.open(QIODevice::ReadOnly)) {
    QFile destinationFile(destination);
    if (destinationFile.open(QIODevice::ReadWrite)) {
      // progress->addText(tr("Writing file: "));
      for (int i=0; i<blocks; i++) {
        int read = sourceFile.read(buf, BLKSIZE);
        if (destinationFile.write(buf, read) == read) {
          destinationFile.flush();
          progress->setValue(i);
        }
        else {
          QMessageBox::warning(NULL, CPN_STR_TTL_ERROR, tr("Write error"));
          result = false;
          break;
        }
      }
      destinationFile.close();
    }
    else {
      QMessageBox::warning(NULL, CPN_STR_TTL_ERROR,tr("Cannot write %1 (reason: %2)").arg(destinationFile.fileName()).arg(sourceFile.errorString()));
      result = false;
    }
  }
  else {
    QMessageBox::warning(NULL, CPN_STR_TTL_ERROR,tr("Cannot open %1 (reason: %2)").arg(sourceFile.fileName()).arg(sourceFile.errorString()));
    result = false;
  }

  sourceFile.close();

  progress->lock(false);
  emit finished();
}

bool ProcessSDDwnld::installSDImage(ProgressWidget * progress)
{
  QString zipfile = g.lastSDDir() + "/" + g.profile[g.id()].sdZipDestFile();
  QString unzipPath = zipfile;
  unzipPath.replace(".zip", "");
  if (!unzip(zipfile, unzipPath)) {
    QMessageBox::critical(this, CPN_STR_APP_NAME, tr("Error: Failed to unzip the downloaded SD card!"), QMessageBox::Ok);
    //  delete the temp folder
    if (!QDir(unzipPath).removeRecursively()) {
      QMessageBox::critical(this, CPN_STR_APP_NAME, tr("Error: Failed to completely tidy up!"), QMessageBox::Ok);
    }
    return false;
  }

  QString sdpath = g.profile[g.id()].sdPath();
  if (QDir(sdpath).exists()) {
    QString bksdpath = sdpath % ".bak";
    if (QDir(bksdpath).exists()) {
      if (!QDir(bksdpath).removeRecursively()) {
        QMessageBox::critical(this, CPN_STR_APP_NAME, tr("Error: Failed to delete previous SD folder backup!"), QMessageBox::Ok);
        return false;
      }
    }
    if (!QDir().rename(sdpath, bksdpath)) {
      QMessageBox::critical(this, CPN_STR_APP_NAME, tr("Error: Unable to backup SD folder!"), QMessageBox::Ok);
      return false;
    }
  }

  if (!copyRecursively(unzipPath, sdpath)) {
    QMessageBox::critical(this, CPN_STR_APP_NAME, tr("Error: Unable to replace SD folder with downloaded version!"), QMessageBox::Ok);
    return false;
  }
  else if (!QDir(unzipPath).removeRecursively()) {
    QMessageBox::critical(this, CPN_STR_APP_NAME, tr("Error: Failed to delete temporary unzipped SD card folder!"), QMessageBox::Ok);
    return false;
  }
  else {
    QMessageBox::information(this, CPN_STR_APP_NAME, tr("Downloaded SD card successfully installed"), QMessageBox::Ok);
  }
  return true;
}

bool ProcessSDDwnld::unzip(const QString & file, const QString & path)
{
  mz_zip_archive zip_archive;
  memset(&zip_archive, 0, sizeof(zip_archive));

  bool status = mz_zip_reader_init_file(&zip_archive, qPrintable(file), 0);
  if (!status) {
    return false;
  }

  int fileCount = (int)mz_zip_reader_get_num_files(&zip_archive);
  if (fileCount == 0) {
    mz_zip_reader_end(&zip_archive);
    //qDebug() << "File count:" << fileCount;
    return false;
  }
  //qDebug() << "File count:" << fileCount;

  mz_zip_archive_file_stat file_stat;
  if (!mz_zip_reader_file_stat(&zip_archive, 0, &file_stat)) {
    mz_zip_reader_end(&zip_archive);
    qDebug() << "File status error";
    return false;
  }

  QString lastDir = "";

  for (int i = 0; i < fileCount; i++) {
    if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
      continue;
    }
    if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
      // mirror full directory structure
      //qDebug() << "Archive directory:" << file_stat.m_filename;
      QString newDir = path + "/";
      newDir.append(file_stat.m_filename);
      if (!QDir().mkpath(newDir)) {
        qDebug() << "Failed to create directory:" << newDir;
        return false;
      }
      //qDebug() << "Created directory:" << newDir;
    }
    else {
      //qDebug() << "Extracting file:" << file_stat.m_filename;
      QString destFile = path + "/";
      destFile.append(file_stat.m_filename);
      QFileInfo rfp(destFile);
      QDir newDir = rfp.absolutePath();
      if (!newDir.exists()) {
        if(!QDir().mkpath(newDir.path())) {
          qDebug() << "Failed to create directory:" << newDir.path();
          return false;
        }
        //qDebug() << "Created directory:" << newDir.path();
        lastDir = newDir.path();
      }
      mz_zip_reader_extract_to_file(&zip_archive, i, qPrintable(destFile), 0);
      //qDebug() << "Extracted file:" << file_stat.m_filename;
    }
  }
  // Close the archive, freeing any resources it was using
  mz_zip_reader_end(&zip_archive);
  return true;
}

bool ProcessSDDwnld::copyRecursively(const QString & srcFilePath, const QString & destFilePath)
{
  QFileInfo srcFileInfo(srcFilePath);
  if (srcFileInfo.isDir()) {
    QDir targetDir(destFilePath);
    targetDir.cdUp();
    if (!targetDir.mkdir(QFileInfo(destFilePath).fileName()))
      return false;
    QDir sourceDir(srcFilePath);
    QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    foreach (const QString &fileName, fileNames) {
      const QString newSrcFilePath = srcFilePath + "/" + fileName;
      const QString newTgtFilePath = destFilePath + "/" + fileName;
      if (!copyRecursively(newSrcFilePath, newTgtFilePath))
        return false;
    }
  }
  else
  {
    if (!QFile::copy(srcFilePath, destFilePath))
      return false;
  }
  return true;
}
