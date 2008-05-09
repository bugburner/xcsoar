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
#include <Aygshell.h>
#include "XCSoar.h"
#include "WindowControls.h"
#include "Statistics.h"
#include "Externs.h"
#include "McReady.h"
#include "dlgTools.h"
#include "InfoBoxLayout.h"
#include "Airspace.h"

static int index_circle = -1;
static int index_area = -1;
static WndForm *wf=NULL;

static void OnAcknowledgeClicked(WindowControl * Sender){
  (void)Sender;

  TCHAR *Name;
  if (index_circle>=0) {
    Name = AirspaceCircle[index_circle].Name;
  } else if (index_area>=0) {
    Name = AirspaceArea[index_area].Name;
  }
  if (Name) {
    if(MessageBoxX(hWndMapWindow,
		   Name,
		   gettext(TEXT("Acknowledge for day?")),
		   MB_YESNO|MB_ICONQUESTION) == IDYES) {

      if (index_circle>=0) {
	AirspaceWarnListAdd(&GPS_INFO, false, true,
			    index_circle, true);
      } else if (index_area>=0) {
	AirspaceWarnListAdd(&GPS_INFO, false, false,
			    index_area, true);
      }
    }
  }

}


static void OnCloseClicked(WindowControl * Sender){
	(void)Sender;
  wf->SetModalResult(mrOK);
}

static CallBackTableEntry_t CallBackTable[]={
  DeclearCallBackEntry(OnAcknowledgeClicked),
  DeclearCallBackEntry(OnCloseClicked),
  DeclearCallBackEntry(NULL)
};


static void SetValues(void) {
  int atype;
  AIRSPACE_ALT* top;
  AIRSPACE_ALT* base;
  TCHAR *name = 0;
  WndProperty* wp;
  TCHAR buffer[80];
  TCHAR buffer2[80];
  bool inside = false;
  double range;
  double bearing;

  if (index_area >=0) {
    atype = AirspaceArea[index_area].Type;
    top = &AirspaceArea[index_area].Top;
    base = &AirspaceArea[index_area].Base;
    name = AirspaceArea[index_area].Name;
    inside = InsideAirspaceArea(GPS_INFO.Longitude, GPS_INFO.Latitude,
				index_area);
    range =
      RangeAirspaceArea(GPS_INFO.Longitude, GPS_INFO.Latitude,
			index_area, &bearing);
  }
  if (index_circle >=0) {
    atype = AirspaceCircle[index_circle].Type;
    top = &AirspaceCircle[index_circle].Top;
    base = &AirspaceCircle[index_circle].Base;
    name = AirspaceCircle[index_circle].Name;
    inside = InsideAirspaceCircle(GPS_INFO.Longitude, GPS_INFO.Latitude,
				  index_circle);
    range =
      RangeAirspaceCircle(GPS_INFO.Longitude, GPS_INFO.Latitude,
			  index_circle);

    DistanceBearing(GPS_INFO.Latitude,
		    GPS_INFO.Longitude,
		    AirspaceCircle[index_circle].Latitude,
		    AirspaceCircle[index_circle].Longitude,
		    NULL, &bearing);
    if (inside) {
      bearing = AngleLimit360(bearing+180);
    }
  }

  if (range<0) {
    range = -range;
  }

  wp = (WndProperty*)wf->FindByName(TEXT("prpName"));
  if (wp) {
    wp->SetText(name);
    wp->RefreshDisplay();
  }

  wp = (WndProperty*)wf->FindByName(TEXT("prpType"));
  if (wp) {
    switch (atype) {
    case RESTRICT:
      wp->SetText(gettext(TEXT("Restricted"))); break;
    case PROHIBITED:
      wp->SetText(gettext(TEXT("Prohibited"))); break;
    case DANGER:
      wp->SetText(gettext(TEXT("Danger Area"))); break;
    case CLASSA:
      wp->SetText(gettext(TEXT("Class A"))); break;
    case CLASSB:
      wp->SetText(gettext(TEXT("Class B"))); break;
    case CLASSC:
      wp->SetText(gettext(TEXT("Class C"))); break;
    case CLASSD:
      wp->SetText(gettext(TEXT("Class D"))); break;
    case CLASSE:
      wp->SetText(gettext(TEXT("Class E"))); break;
    case CLASSF:
      wp->SetText(gettext(TEXT("Class F"))); break;
    case NOGLIDER:
      wp->SetText(gettext(TEXT("No Glider"))); break;
    case CTR:
      wp->SetText(gettext(TEXT("CTR"))); break;
    case WAVE:
      wp->SetText(gettext(TEXT("Wave"))); break;
    default:
      wp->SetText(gettext(TEXT("Unknown")));
    }
    wp->RefreshDisplay();
  }

  wp = (WndProperty*)wf->FindByName(TEXT("prpTop"));
  if (wp) {
    switch (top->Base){
    case abUndef:
      _stprintf(buffer, TEXT("%.0f[m] %.0f[ft] [?]"),
		top->Altitude, top->Altitude*TOFEET);
      break;
    case abMSL:
      _stprintf(buffer, TEXT("%.0f[m] %.0f[ft] MSL"),
		top->Altitude, top->Altitude*TOFEET);
      break;
    case abAGL:
      _stprintf(buffer, TEXT("%.0f[m] %.0f[ft] AGL"),
		top->AGL, top->AGL*TOFEET);
      break;
    case abFL:
      _stprintf(buffer, TEXT("FL %.0f (%.0f[m] %.0f[ft])"),
		top->FL, top->Altitude, top->Altitude*TOFEET);
      break;
    }
    wp->SetText(buffer);
    wp->RefreshDisplay();
  }

  wp = (WndProperty*)wf->FindByName(TEXT("prpBase"));
  if (wp) {
    switch (base->Base){
    case abUndef:
      _stprintf(buffer, TEXT("%.0f[m] %.0f[ft] [?]"),
		base->Altitude, base->Altitude*TOFEET);
      break;
    case abMSL:
      _stprintf(buffer, TEXT("%.0f[m] %.0f[ft] MSL"),
		base->Altitude, base->Altitude*TOFEET);
      break;
    case abAGL:
      _stprintf(buffer, TEXT("%.0f[m] %.0f[ft] AGL"),
		base->AGL, base->AGL*TOFEET);
      break;
    case abFL:
      _stprintf(buffer, TEXT("FL %.0f (%.0f[m] %.0f[ft])"),
		base->FL, base->Altitude, base->Altitude*TOFEET);
      break;
    }
    wp->SetText(buffer);
    wp->RefreshDisplay();
  }

  wp = (WndProperty*)wf->FindByName(TEXT("prpRange"));
  if (wp) {
    if (inside) {
      wp->SetCaption(gettext(TEXT("Inside")));
    }
    Units::FormatUserDistance(range, buffer, 20);
    _stprintf(buffer2, TEXT(" %d�"), iround(bearing));
    _tcscat(buffer, buffer2);
    wp->SetText(buffer);
    wp->RefreshDisplay();
  }
}


void dlgAirspaceDetails(int the_circle, int the_area) {
  index_circle = the_circle;
  index_area = the_area;
  if ((index_area<=0) && (index_circle <=0)) {
    return;
  }

  char filename[MAX_PATH];
  LocalPathS(filename, TEXT("dlgAirspaceDetails.xml"));
  wf = dlgLoadFromXML(CallBackTable,
		      filename,
		      hWndMainWindow,
		      TEXT("IDR_XML_AIRSPACEDETAILS"));

  if (!wf) return;

  ASSERT(wf!=NULL);

  SetValues();

  wf->ShowModal();

  delete wf;
  wf = NULL;
  return;
}


/*


			distance,
                    Units::GetDistanceName()

  wp = (WndProperty*)wf->FindByName(TEXT("prpDistance"));
  if (wp) {
    wp->GetDataField()->SetAsFloat(distance);
    wp->GetDataField()->SetUnits(Units::GetDistanceName());
    wp->RefreshDisplay();
  }

*/