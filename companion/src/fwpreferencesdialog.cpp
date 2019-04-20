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

#include "fwpreferencesdialog.h"
#include "ui_fwpreferencesdialog.h"
#include "mainwindow.h"
#include "helpers.h"
#include "appdata.h"

FirmwarePreferencesDialog::FirmwarePreferencesDialog(QWidget * parent) :
  QDialog(parent),
  ui(new Ui::FirmwarePreferencesDialog)
{
  ui->setupUi(this);
  setWindowIcon(CompanionIcon("fwpreferences.png"));
  initSettings();
  MainWindow * mw = qobject_cast<MainWindow *>(this->parent());
  if (mw)
    connect(mw, &MainWindow::firmwareDownloadCompleted, this, &FirmwarePreferencesDialog::initSettings);
}

FirmwarePreferencesDialog::~FirmwarePreferencesDialog()
{
  delete ui;
}

void FirmwarePreferencesDialog::initSettings()
{
  ui->fwTypeLbl->setText(g.currentProfile().fwType());
  int version = g.fwRev.get(g.currentProfile().fwType());
  if (version > 0)
    ui->lastRevisionLbl->setText(index2version(version));
  else
    ui->lastRevisionLbl->setText(tr("Unknown"));

  ui->sdTypeLbl->setText(g.profile[g.id()].sdZipSrcFile());
  QString sdVer = g.profile[g.id()].sdVersion();
  if (sdVer.isEmpty())
    ui->lastSDVersionLbl->setText(tr("Unknown"));
  else
    ui->lastSDVersionLbl->setText(sdVer);
}

void FirmwarePreferencesDialog::on_checkFWUpdates_clicked()
{
  MainWindow * mw = qobject_cast<MainWindow *>(this->parent());
  if (mw)
    mw->checkForFirmwareUpdate();
}

void FirmwarePreferencesDialog::on_fw_dnld_clicked()
{
  MainWindow * mw = qobject_cast<MainWindow *>(this->parent());
  if (mw)
    mw->downloadLastFirmwareUpdate();
}

void FirmwarePreferencesDialog::on_sd_dnld_clicked()
{
  MainWindow * mw = qobject_cast<MainWindow *>(this->parent());
  if (mw)
    mw->downloadLastSDCardUpdate();
}
