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

#include <string.h>
#include "eeprominterface.h"
#include "wizarddata.h"

Channel::Channel()
{
  clear();
}

void Channel::clear()
{
  page = Page_None;
  prebooked = false;
  input1 = NO_INPUT;
  input2 = NO_INPUT;
  weight1 = 0;
  weight2 = 0;
}

WizMix::WizMix(Firmware * firmware, GeneralSettings & settings, unsigned int modelId, ModelData & modelData):
  complete(false),
  vehicle(UNKNOWN),
  firmware(firmware),
  settings(settings),
  originalModelData(modelData),
  modelId(modelId)
{
  name = originalModelData.name;
}

void WizMix::maxMixSwitch(QString name, MixData & mix, int channel, RawSwitch sw, int weight, MltpxValue mltpx)
{
  strncpy(mix.name, name.toLatin1(), sizeof(mix.name)-1);
  mix.destCh = channel;
  mix.srcRaw = RawSource(SOURCE_TYPE_MAX);
  mix.swtch  = sw;
  mix.weight = weight;
  mix.mltpx  = mltpx;
}

void WizMix::addMix(ModelData & model, Input input, int weight, int channel, int & mixIndex, RawSwitch swtch)
{
  if (input != NO_INPUT)  {
    if (input >= RUDDER_INPUT && input <= AILERONS_INPUT) {
      MixData & mix = model.mixData[mixIndex++];
      mix.destCh = channel+1;
      if (IS_SKY9X(getCurrentBoard())) {
        mix.srcRaw = RawSource(SOURCE_TYPE_STICK, input-1);
      }
      else {
        int channel = settings.getDefaultChannel(input-1);
        mix.srcRaw = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, channel);
      }

      mix.weight = weight;
    }
    else if (input==FLAPS_INPUT){
      // There ought to be some kind of constants for switches somewhere...
      maxMixSwitch(tr("Flaps Up"), model.mixData[mixIndex++], channel+1, swtch,  weight);
      maxMixSwitch(tr("Flaps Dn"), model.mixData[mixIndex++], channel+1, swtch, -weight);
    }
    else if (input==AIRBRAKES_INPUT){
      maxMixSwitch(tr("AirbkOff"), model.mixData[mixIndex++], channel+1, swtch, -weight);
      maxMixSwitch(tr("AirbkOn"),  model.mixData[mixIndex++], channel+1, swtch, weight);
    }
    else if (input==THROTTLE_CUT_INPUT) {
      maxMixSwitch(tr("Cut"), model.mixData[mixIndex++], channel+1, swtch , -100, MLTPX_REP);
    }
  }
}

WizMix::operator ModelData()
{
  int throttleChannel = -1;

  ModelData model;
  model.category = originalModelData.category;
  model.used = true;
  model.moduleData[0].modelId = modelId;
  model.setDefaultInputs(settings);

  int mixIndex = 0;
  int timerIndex = 0;

  strncpy(model.name, name.toLatin1().data(), sizeof(model.name)-1);

  // Add the channel mixes
  for (int i=0; i<WIZ_MAX_CHANNELS; i++ )
  {
    Channel ch = channel[i];

    addMix(model, ch.input1, ch.weight1, i, mixIndex, ch.switch1);
    addMix(model, ch.input2, ch.weight2, i, mixIndex, ch.switch2);

    if (ch.input1 == THROTTLE_INPUT || ch.input2 == THROTTLE_INPUT) {
      throttleChannel++;
<<<<<<< HEAD

      /*
      if (options[THROTTLE_CUT_OPTION]) {
        MixData & mix = model.mixData[mixIndex++];
        mix.destCh = i+1;
        mix.srcRaw = SOURCE_TYPE_MAX;
        mix.weight = -100;
        mix.swtch.type = SWITCH_TYPE_SWITCH;
        mix.swtch.index = IS_SKY9X(getCurrentBoard()) ? SWITCH_THR : SWITCH_SF0;
        mix.mltpx = MLTPX_REP;
        strncpy(mix.name, tr("Cut").toLatin1().data(), sizeof(mix.name)-1);
      }
      */
=======
>>>>>>> 0e4701b39 (continued)
    }
  }

  if (throttleChannel >= 0) {
    if (options[FLIGHT_TIMER_OPTION]) {
      strncpy(model.timers[timerIndex].name, tr("Flt").toLatin1().data(), sizeof(model.timers[timerIndex].name)-1);
      model.timers[timerIndex].mode.type = SWITCH_TYPE_TIMER_MODE;
      model.timers[timerIndex].mode.index = TMRMODE_THR_TRG;
      timerIndex++;
    }

    if (options[THROTTLE_TIMER_OPTION]) {
      strncpy(model.timers[timerIndex].name, tr("Thr").toLatin1().data(), sizeof(model.timers[timerIndex].name)-1);
      model.timers[timerIndex].mode.type = SWITCH_TYPE_TIMER_MODE;
      model.timers[timerIndex].mode.index = TMRMODE_THR;
      timerIndex++;
    }
  }

  return model;
}

QString WizMix::vehicleName(Vehicle vehicle)
{
  switch (vehicle) {
    case PLANE:
      return tr("Plane");
    case MULTIROTOR:
      return tr("Multirotor");
    case HELICOPTER:
      return tr("Helicopter");
    default:
      return "---";
  }
}

QString WizMix::optionName(Options option)
{
  switch (option) {
    case THROTTLE_TIMER_OPTION:
      return tr("Throttle Timer");
    case FLIGHT_TIMER_OPTION:
      return tr("Flight Timer");
    default:
      return "---";
  }
}
