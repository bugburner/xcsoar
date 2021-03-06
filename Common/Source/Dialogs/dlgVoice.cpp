/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009

	M Roberts (original release)
	Robin Birch <robinb@ruffnready.co.uk>
	Samuel Gisiger <samuel.gisiger@triadis.ch>
	Jeff Goodenough <jeff@enborne.f2s.com>
	Alastair Harrison <aharrison@magic.force9.co.uk>
	Scott Penrose <scottp@dd.com.au>
	John Wharington <jwharington@gmail.com>
	Lars H <lars_hn@hotmail.com>
	Rob Dunning <rob@raspberryridgesheepfarm.com>
	Russell King <rmk@arm.linux.org.uk>
	Paolo Ventafridda <coolwind@email.it>
	Tobias Lohner <tobias@lohner-net.de>
	Mirek Jezek <mjezek@ipplc.cz>
	Max Kellermann <max@duempel.org>
	Tobias Bieniek <tobias.bieniek@gmx.de>

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

#include "Dialogs/Internal.hpp"
#include "Units.hpp"
#include "UtilsProfile.hpp"
#include "Profile.hpp"
#include "Registry.hpp"
#include "Audio/VegaVoice.h"
#include "DataField/Base.hpp"
#include "MainWindow.hpp"

static WndForm *wf=NULL;

static void OnCloseClicked(WindowControl * Sender){
	(void)Sender;
  wf->SetModalResult(mrOK);
}


static CallBackTableEntry_t CallBackTable[]={
  DeclareCallBackEntry(OnCloseClicked),
  DeclareCallBackEntry(NULL)
};


static void LoadIntoForm(WndForm &form, const SETTINGS_COMPUTER &settings){
  LoadFormProperty(form, _T("prpVoiceClimbRate"),
                   settings.EnableVoiceClimbRate);
  LoadFormProperty(form, _T("prpVoiceTerrain"), settings.EnableVoiceTerrain);
  LoadFormProperty(form, _T("prpVoiceWaypointDistance"),
                   settings.EnableVoiceWaypointDistance);
  LoadFormProperty(form, _T("prpVoiceTaskAltitudeDifference"),
                   settings.EnableVoiceTaskAltitudeDifference);
  LoadFormProperty(form, _T("prpVoiceMacCready"),
                   settings.EnableVoiceMacCready);
  LoadFormProperty(form, _T("prpVoiceNewWaypoint"),
                   settings.EnableVoiceNewWaypoint);
  LoadFormProperty(form, _T("prpVoiceInSector"), settings.EnableVoiceInSector);
  LoadFormProperty(form, _T("prpVoiceAirspace"), settings.EnableVoiceAirspace);
}

static bool
SaveFromForm(const WndForm &form, SETTINGS_COMPUTER &settings)
{
  return
    SaveFormProperty(form, _T("prpVoiceClimbRate"),
                     settings.EnableVoiceClimbRate,
                     szRegistryVoiceClimbRate) ||
    SaveFormProperty(form, _T("prpVoiceTerrain"),
                     settings.EnableVoiceTerrain, szRegistryVoiceTerrain) ||
    SaveFormProperty(form, _T("prpVoiceWaypointDistance"),
                     settings.EnableVoiceWaypointDistance,
                     szRegistryVoiceWaypointDistance) ||
    SaveFormProperty(form, _T("prpVoiceTaskAltitudeDifference"),
                     settings.EnableVoiceTaskAltitudeDifference,
                     szRegistryVoiceTaskAltitudeDifference) ||
    SaveFormProperty(form, _T("prpVoiceMacCready"),
                     settings.EnableVoiceMacCready,
                     szRegistryVoiceMacCready) ||
    SaveFormProperty(form, _T("prpVoiceNewWaypoint"),
                     settings.EnableVoiceNewWaypoint,
                     szRegistryVoiceNewWaypoint) ||
    SaveFormProperty(form, _T("prpVoiceInSector"),
                     settings.EnableVoiceInSector, szRegistryVoiceInSector) ||
    SaveFormProperty(form, _T("prpVoiceAirspace"),
                     settings.EnableVoiceAirspace, szRegistryVoiceAirspace);
}


void dlgVoiceShowModal(void){
  wf = dlgLoadFromXML(CallBackTable,
                      _T("dlgVoice.xml"),
		      XCSoarInterface::main_window,
		      _T("IDR_XML_VOICE"));

  
  if (!wf) return;

  LoadIntoForm(*wf, XCSoarInterface::SettingsComputer());

  wf->ShowModal();

  bool changed = false;

  changed = SaveFromForm(*wf, XCSoarInterface::SetSettingsComputer());
  
  if (changed) {
    Profile::StoreRegistry();

    MessageBoxX(gettext(_T("Changes to configuration saved.")),
		 _T(""), MB_OK);
  }
    
  delete wf;
  wf = NULL;

}

