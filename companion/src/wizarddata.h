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

#ifndef _WIZARDDATA_H_
#define _WIZARDDATA_H_

#include "eeprominterface.h"

#include <QObject>
#include <QString>

enum Input {
  NO_INPUT,
  RUDDER_INPUT,
  ELEVATOR_INPUT,
  THROTTLE_INPUT,
  AILERONS_INPUT,
  FLAPS_INPUT,
  AIRBRAKES_INPUT,
  THROTTLE_CUT_INPUT
};

//  To Do add extras such as trainer, glider, wing, drone, multi rotor
enum Vehicle {
  NOVEHICLE,  //  to do rename UNKNOWN
  PLANE,
  MULTIROTOR,
  HELICOPTER,
  FLIGHTSIM
};

//  To Do refactor as bit mask as mutually inclusive and extras such as retracts, diff, etc
enum Options {
  THROTTLE_TIMER_OPTION,
  FLIGHT_TIMER_OPTION,
  MAX_NUM
};

enum WizardPage {
  Page_None = -1,
  Page_Models,
  Page_Throttle,
  Page_Wingtypes,
  Page_Ailerons,
  Page_Flaps,
  Page_Airbrakes,
  Page_Elevons,
  Page_Rudder,
  Page_Tails,
  Page_Tail,
  Page_Vtail,
  Page_Simpletail,
  Page_Cyclic,
  Page_Gyro,
  Page_Flybar,
  Page_Fblheli,
  Page_Helictrl,
  Page_Multirotor,
  Page_Options,
  Page_Conclusion
};

#define WIZ_MAX_CHANNELS 8    //  to do user choice as could be 4, 6, 8, 16 so this is MAX Receiver
class Channel
{
  public:
    WizardPage page;     // Originating dialog, only of interest for producer
    bool prebooked;      // Temporary lock variable
    Input input1;
    Input input2;
    int weight1;
    int weight2;
    RawSwitch switch1;
    RawSwitch switch2;

    Channel();
    void clear();
};

class WizMix : public QObject
{
  Q_OBJECT

  public:
    bool complete;
    QString name;
    Vehicle vehicle;
    Channel channel[WIZ_MAX_CHANNELS];    //  for memory allocation though likely less
    bool options[Options::MAX_NUM];       //  to do bit mask
    QString vehicleName(Vehicle vehicle);
    QString optionName(Options option);   //  to do bit mask
    Firmware * firmware;
    GeneralSettings & settings;
    ModelData & originalModelData;

    WizMix(Firmware * firmware, GeneralSettings & settings, unsigned int modelId, ModelData & modelData);
    operator ModelData();

  private:
    WizMix();
    void addMix(ModelData & model, Input input, int weight, int channel, int & mixerIndex, RawSwitch swtch);
    void maxMixSwitch(QString name, MixData & mix, int destCh, RawSwitch sw, int weight, MltpxValue mltpx = MLTPX_ADD);

    unsigned int modelId;
};

#endif // _WIZARDDATA_H_
