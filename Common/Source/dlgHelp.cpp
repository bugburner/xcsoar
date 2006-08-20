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

#include "stdafx.h"
#include "externs.h"
#include "units.h"
#include "device.h"
#include "InputEvents.h"
#include "WindowControls.h"
#include "dlgTools.h"

extern HWND   hWndMainWindow;
static WndForm *wf=NULL;


//
//

static void OnCloseClicked(WindowControl * Sender){
  wf->SetModalResult(mrOK);
}




static CallBackTableEntry_t CallBackTable[]={
  DeclearCallBackEntry(OnCloseClicked),
  DeclearCallBackEntry(NULL)
};



void dlgHelpShowModal(TCHAR* Caption, TCHAR* HelpText) {
  if (!Caption || !HelpText) {
    return;
  }

  wf = dlgLoadFromXML(CallBackTable, LocalPathS(TEXT("dlgHelp.xml")),
		      hWndMainWindow,
		      TEXT("IDR_XML_HELP"));

  WndProperty* wp;

  if (wf) {

    TCHAR fullcaption[100];
    _stprintf(fullcaption,TEXT("Help: %s"), Caption);

    wf->SetCaption(fullcaption);

    wp = (WndProperty*)wf->FindByName(TEXT("prpHelpText"));
    if (wp) {
      wp->SetText(HelpText);
      wp->RefreshDisplay();
    }
    wf->ShowModal();
    delete wf;
  }
  wf = NULL;

}

