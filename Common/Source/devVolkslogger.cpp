/*
Copyright_License {

  XCSoar Glide Computer - http://xcsoar.sourceforge.net/
  Copyright (C) 2000 - 2005

  	M Roberts (original release)
	Robin Birch <robinb@ruffnready.co.uk>
	Samuel Gisiger <samuel.gisiger@triadis.ch>
	Jeff Goodenough <jeff@enborne.f2s.com>
	Alastair Harrison <aharrison@magic.force9.co.uk>
	Scott Penrose <scottp@dd.com.au>
	John Wharington <jwharington@bigfoot.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

}
*/


// ToDo

// adding baro alt sentance parser to support baro source priority  if (d == pDevPrimaryBaroSource){...}

#include "stdafx.h"

#include "externs.h"
#include "utils.h"
#include "parser.h"
#include "port.h"

#include "devVolkslogger.h"

#include "Volkslogger/vlapi2.h"
#include "Volkslogger/vlapihlp.h"

static BOOL fDeclarationPending = FALSE;
static int nDeclErrorCode = 0;
static int ewDecelTpIndex = 0;

// RMN: Volkslogger
// Source data:
// $PGCS,1,0EC0,FFF9,0C6E,02*61
// $PGCS,1,0EC0,FFFA,0C6E,03*18
BOOL vl_PGCS1(PDeviceDescriptor_t d, TCHAR *String, NMEA_INFO *GPS_INFO)
{

  TCHAR ctemp[80];
  double InternalAltitude;

  NMEAParser::ExtractParameter(String,ctemp,2);
  // four characers, hex, barometric altitude
  InternalAltitude = HexStrToDouble(ctemp,NULL);

  if (d == pDevPrimaryBaroSource) {

    if(InternalAltitude > 60000)
      GPS_INFO->BaroAltitude =
        AltitudeToQNHAltitude(InternalAltitude - 65535);
    // Assuming that altitude has wrapped around.  60 000 m occurs at
    // QNH ~2000 hPa
    else
      GPS_INFO->BaroAltitude =
        AltitudeToQNHAltitude(InternalAltitude);
    // typo corrected 21.04.07
    // Else the altitude is good enough.

    GPS_INFO->BaroAltitudeAvailable = TRUE;
  }

  // ExtractParameter(String,ctemp,3);
  // four characters, hex, constant.  Value 1371 (dec)

  // nSatellites = (int)(min(12,HexStrToDouble(ctemp, NULL)));

  if (GPS_INFO->SatellitesUsed <= 0) {
    GPS_INFO->SatellitesUsed = 4;
    // just to make XCSoar quit complaining.  VL doesn't tell how many
    // satellites it uses.  Without this XCSoar won't do wind
    // measurements.
  }

  return FALSE;
}


BOOL VLParseNMEA(PDeviceDescriptor_t d, TCHAR *String, NMEA_INFO *GPS_INFO){
  (void)d;

  if (!NMEAParser::NMEAChecksum(String) || (GPS_INFO == NULL)){
    return FALSE;
  }

  if(_tcsstr(String,TEXT("$PGCS,")) == String){
    return vl_PGCS1(d, &String[6], GPS_INFO);
  }

  return FALSE;

}


BOOL VLOpen(PDeviceDescriptor_t d, int Port){
  d->Port = Port;
  return(TRUE);
}


VLAPI vl;

static int nturnpoints = 0;

BOOL VLDeclBegin(PDeviceDescriptor_t d,
                 TCHAR *PilotsName, TCHAR *Class, TCHAR *ID){

  CreateProgressDialog(gettext(TEXT("Comms with Volkslogger")));

  vl.set_device(d);
  nturnpoints = 0;

  int err;
  if((err = vl.open(1, 20, 1, 38400L)) != VLA_ERR_NOERR) {
    //    _isConnected = false;
  }
  else {
    //    _isConnected = true;
  }

  if (err == VLA_ERR_NOERR) {
    if ((err = vl.read_info()) == VLA_ERR_NOERR) {
      //      vl.read_db_and_declaration();
    }

    char temp[100];
    sprintf(temp, "%S", PilotsName);
    strncpy(vl.declaration.flightinfo.pilot, temp, 64);

    sprintf(temp, "%S", ID);
    strncpy(vl.declaration.flightinfo.competitionid, temp, 3);

    sprintf(temp, "%S", Class);
    strncpy(vl.declaration.flightinfo.competitionclass, temp, 12);

    if (ValidWayPoint(HomeWaypoint)) {
      sprintf(temp, "%S", WayPointList[HomeWaypoint].Name);

      strncpy(vl.declaration.flightinfo.homepoint.name, temp, 6);
      vl.declaration.flightinfo.homepoint.lon =
        WayPointList[HomeWaypoint].Longitude;
      vl.declaration.flightinfo.homepoint.lat =
        WayPointList[HomeWaypoint].Latitude;
    }
  }

  return (err == VLA_ERR_NOERR);
}


BOOL VLDeclEnd(PDeviceDescriptor_t d){
  vl.declaration.task.nturnpoints = max(min(nturnpoints-2, 12), 0);

  LockTaskData();

  // start..
  switch(StartLine) {
  case 0: // cylinder
    vl.declaration.task.startpoint.oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
    vl.declaration.task.startpoint.lw = min(1500,StartRadius);
    vl.declaration.task.startpoint.rz = min(1500,StartRadius);
    vl.declaration.task.startpoint.rs = 0;
    break;
  case 1: // line
    vl.declaration.task.startpoint.oztyp = VLAPI_DATA::DCLWPT::OZTYP_LINE;
    vl.declaration.task.startpoint.lw = min(1500,StartRadius*2);
    vl.declaration.task.startpoint.rs = 0;
    vl.declaration.task.startpoint.rz = 0;
    break;
  case 2: // fai sector
    vl.declaration.task.startpoint.oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
    vl.declaration.task.startpoint.lw = min(1500,StartRadius);
    vl.declaration.task.startpoint.rz = 0;
    vl.declaration.task.startpoint.rs = min(1500,StartRadius);
    break;
  }
  vl.declaration.task.startpoint.ws = 360;

  // rest of task...
  for (int i=0; i<nturnpoints; i++) {
    // note this is for non-aat only!
    switch (SectorType) {
    case 0: // cylinder
      vl.declaration.task.turnpoints[i].oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
      vl.declaration.task.turnpoints[i].rz = SectorRadius;
      vl.declaration.task.turnpoints[i].rs = 0;
      vl.declaration.task.turnpoints[i].lw = 0;
      break;
    case 1: // sector
      vl.declaration.task.turnpoints[i].oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
      vl.declaration.task.turnpoints[i].rz = 0;
      vl.declaration.task.turnpoints[i].rs = 15000;
      vl.declaration.task.turnpoints[i].lw = 0;
      break;
    case 2: // German DAe 0.5/10
      vl.declaration.task.turnpoints[i].oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
      vl.declaration.task.turnpoints[i].rz = 500;
      vl.declaration.task.turnpoints[i].rs = 10000;
      vl.declaration.task.turnpoints[i].lw = 0;
      break;
    };
    vl.declaration.task.turnpoints[i].ws = 360; // auto direction

  }

  // Finish
  switch(FinishLine) {
  case 0: // cylinder
    vl.declaration.task.finishpoint.oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
    vl.declaration.task.finishpoint.lw = min(1500,FinishRadius);
    vl.declaration.task.finishpoint.rz = min(1500,FinishRadius);
    vl.declaration.task.finishpoint.rs = 0;
    break;
  case 1: // line
    vl.declaration.task.finishpoint.oztyp = VLAPI_DATA::DCLWPT::OZTYP_LINE;
    vl.declaration.task.finishpoint.lw = FinishRadius*2;
    vl.declaration.task.finishpoint.rz = 0;
    vl.declaration.task.finishpoint.rs = 0;
    break;
  case 2: // fai sector
    vl.declaration.task.finishpoint.oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
    vl.declaration.task.finishpoint.lw = min(1500,FinishRadius);
    vl.declaration.task.finishpoint.rz = 0;
    vl.declaration.task.finishpoint.rs = min(1500,FinishRadius);
    break;
  }
  vl.declaration.task.finishpoint.ws = 360;

  UnlockTaskData();

  BOOL ok = (vl.write_db_and_declaration() == VLA_ERR_NOERR);

  vl.close(1);

  CloseProgressDialog();

  return ok;
}


BOOL VLDeclAddWayPoint(PDeviceDescriptor_t d, WAYPOINT *wp){
  char temp[100];
  sprintf(temp, "%S", wp->Name);

  if (nturnpoints == 0) {
    strncpy(vl.declaration.task.startpoint.name, temp, 6);
    vl.declaration.task.startpoint.lon =
      wp->Longitude;
    vl.declaration.task.startpoint.lat =
      wp->Latitude;
    nturnpoints++;
  } else {
    strncpy(vl.declaration.task.turnpoints[nturnpoints-1].name, temp, 6);
    vl.declaration.task.turnpoints[nturnpoints-1].lon =
      wp->Longitude;
    vl.declaration.task.turnpoints[nturnpoints-1].lat =
      wp->Latitude;
    nturnpoints++;
  }
  strncpy(vl.declaration.task.finishpoint.name, temp, 6);
  vl.declaration.task.finishpoint.lon =
    wp->Longitude;
  vl.declaration.task.finishpoint.lat =
    wp->Latitude;

  return(TRUE);

}


BOOL VLIsLogger(PDeviceDescriptor_t d){
  (void)d;
  return(TRUE);
}


BOOL VLIsGPSSource(PDeviceDescriptor_t d){
  (void)d;
  return(TRUE);
}


BOOL VLLinkTimeout(PDeviceDescriptor_t d){
  (void)d;
  return(TRUE);
}


BOOL vlInstall(PDeviceDescriptor_t d){
  _tcscpy(d->Name, TEXT("Volkslogger"));
  d->ParseNMEA = VLParseNMEA;
  d->PutMacCready = NULL;
  d->PutBugs = NULL;
  d->PutBallast = NULL;
  d->Open = NULL;
  d->Close = NULL;
  d->Init = NULL;
  d->LinkTimeout = VLLinkTimeout;
  d->DeclBegin = VLDeclBegin;
  d->DeclEnd = VLDeclEnd;
  d->DeclAddWayPoint = VLDeclAddWayPoint;
  d->IsLogger = VLIsLogger;
  d->IsGPSSource = VLIsGPSSource;

  return(TRUE);

}


BOOL vlRegister(void){
  return(devRegister(
    TEXT("Volkslogger"),
    (1l << dfGPS)
    | (1l << dfBaroAlt)
    | (1l << dfLogger),
    vlInstall
  ));
}
