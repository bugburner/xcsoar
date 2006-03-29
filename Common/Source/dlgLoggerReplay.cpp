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
#if (NEWINFOBOX>0)

#include "stdafx.h"

#include "statistics.h"

#include "externs.h"
#include "units.h"
#include "McReady.h"
#include "device.h"

#include "Logger.h"

#include "WindowControls.h"
#include "dlgTools.h"
#include "Port.h"

extern HWND   hWndMainWindow;
static WndForm *wf=NULL;


static void OnStopClicked(WindowControl * Sender){
  ReplayLogger::Stop();
}

static void OnStartClicked(WindowControl * Sender){
  WndProperty* wp;
  wp = (WndProperty*)wf->FindByName(TEXT("prpIGCFile"));
  if (wp) {
    DataFieldFileReader* dfe;
    dfe = (DataFieldFileReader*)wp->GetDataField();
    ReplayLogger::SetFilename(dfe->GetPathFile());
  }
  ReplayLogger::Start();
}


static void OnCloseClicked(WindowControl * Sender){
  wf->SetModalResult(mrOK);
}


static void OnRateData(DataField *Sender, DataField::DataAccessKind_t Mode){

  switch(Mode){
    case DataField::daGet:
      Sender->Set(ReplayLogger::TimeScale);
    break;
    case DataField::daPut:
    case DataField::daChange:
      ReplayLogger::TimeScale = Sender->GetAsFloat();
    break;
  }

}


static CallBackTableEntry_t CallBackTable[]={
  DeclearCallBackEntry(OnStopClicked),
  DeclearCallBackEntry(OnStartClicked),
  DeclearCallBackEntry(OnRateData),
  DeclearCallBackEntry(OnCloseClicked),
  DeclearCallBackEntry(NULL)
};


void dlgLoggerReplayShowModal(void){

  wf = dlgLoadFromXML(CallBackTable, LocalPathS(TEXT("dlgLoggerReplay.xml")),
		      hWndMainWindow,
		      TEXT("IDR_XML_LOGGERREPLAY"));

  WndProperty* wp;

  if (wf) {

    wp = (WndProperty*)wf->FindByName(TEXT("prpRate"));
    if (wp) {
      wp->GetDataField()->SetAsFloat(ReplayLogger::TimeScale);
      wp->RefreshDisplay();
    }

    wp = (WndProperty*)wf->FindByName(TEXT("prpIGCFile"));
    if (wp) {
      DataFieldFileReader* dfe;
      dfe = (DataFieldFileReader*)wp->GetDataField();
      dfe->ScanDirectoryTop(TEXT("*.igc"));
      dfe->Lookup(ReplayLogger::GetFilename());
      wp->RefreshDisplay();
    }

    wf->ShowModal();
    delete wf;
  }
  wf = NULL;
}


#endif