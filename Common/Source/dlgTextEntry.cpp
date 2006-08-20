
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
#include "XCSoar.h"
#include "Utils.h"
#include "WindowControls.h"
#include "dlgTools.h"
#include "externs.h"

static WndForm *wf=NULL;
static WndOwnerDrawFrame *wGrid=NULL;

#define MAX_TEXTENTRY 40
static unsigned int cursor = 0;
static int lettercursor=0;

static TCHAR edittext[MAX_TEXTENTRY];

static TCHAR EntryLetters[] = TEXT(" ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.-");

#define MAXENTRYLETTERS (sizeof(EntryLetters)/sizeof(EntryLetters[0])-1)

static void OnTextPaint(WindowControl *Sender, HDC hDC) {
  RECT  rcgfx;
  HFONT hfOld;

  CopyRect(&rcgfx, Sender->GetBoundRect());
  // background is painted in the base-class
  hfOld = (HFONT)SelectObject(hDC, Sender->GetFont());
  SetBkMode(hDC, TRANSPARENT);
  SetTextColor(hDC, Sender->GetForeColor());

  ////
  // Do the actual painting of the text

  SIZE tsize;
  GetTextExtentPoint(hDC, edittext, _tcslen(edittext), &tsize);
  SIZE tsizec;
  GetTextExtentPoint(hDC, edittext, cursor, &tsizec);
  SIZE tsizea;
  GetTextExtentPoint(hDC, edittext, cursor+1, &tsizea);

  POINT p[5];
  p[0].x = 10;
  p[0].y = 20;

  p[2].x = p[0].x + tsizec.cx;
  p[2].y = p[0].y + tsize.cy+5;

  p[3].x = p[0].x + tsizea.cx;
  p[3].y = p[0].y + tsize.cy+5;

  p[1].x = p[2].x;
  p[1].y = p[2].y-2;

  p[4].x = p[3].x;
  p[4].y = p[3].y-2;

  SelectObject(hDC, GetStockObject(WHITE_PEN));
  Polyline(hDC, p+1, 4);

  /*
  int x = (int)((xv-x_min)*xscale)+rc.left-tsize.cx/2;
  int y = (int)((y_max-yv)*yscale)+rc.top-tsize.cy/2;
  */
  SetBkMode(hDC, OPAQUE);
  ExtTextOut(hDC, p[0].x, p[0].y, ETO_OPAQUE, NULL,
             edittext, _tcslen(edittext), NULL);
  SetBkMode(hDC, TRANSPARENT);

  ////

  SelectObject(hDC, hfOld);
}



static void UpdateCursor(void) {
  while(lettercursor>=MAXENTRYLETTERS)
    lettercursor -= MAXENTRYLETTERS;
  while(lettercursor<0)
    lettercursor += MAXENTRYLETTERS;
  edittext[cursor] = EntryLetters[lettercursor];

  if (wGrid != NULL)
    wGrid->Redraw();

}


static void MoveCursor(void) {
  if (cursor>=_tcslen(edittext)) {
    edittext[cursor+1] = 0;
  }
  for (lettercursor=0; lettercursor< MAXENTRYLETTERS; lettercursor++) {
    if (edittext[cursor]== EntryLetters[lettercursor])
      break;
  }
  if (lettercursor== MAXENTRYLETTERS) {
    lettercursor = 0;
    edittext[cursor] = EntryLetters[lettercursor];
  }
  if (edittext[cursor]== 0) {
    lettercursor= 0;
  }
  UpdateCursor();
}


static int FormKeyDown(WindowControl * Sender, WPARAM wParam, LPARAM lParam) {
  switch(wParam & 0xffff){
    case VK_LEFT:
      if (cursor<1)
        return(0); // min width
      cursor--;
      MoveCursor();
      return(0);
    case VK_RIGHT:
      if (cursor>=MAX_TEXTENTRY-2)
        return(0); // max width
      cursor++;
      MoveCursor();
      return(0);
    case VK_UP:
      if (lettercursor>0) {
        lettercursor--;
        UpdateCursor();
      }
      return(0);
    case VK_DOWN:
      if (lettercursor<MAXENTRYLETTERS-1) {
        lettercursor++;
        UpdateCursor();
      }
      return(0);
    case VK_RETURN:
      wf->SetModalResult(mrOK);
      return(0);
  }
  return(1);
}



static CallBackTableEntry_t CallBackTable[]={
  DeclearCallBackEntry(OnTextPaint),
  DeclearCallBackEntry(NULL)
};


void dlgTextEntryShowModal(TCHAR *text) {

  wf = NULL;
  wGrid = NULL;

  wf = dlgLoadFromXML(CallBackTable,
		      LocalPathS(TEXT("dlgTextEntry.xml")),
		      hWndMainWindow,
		      TEXT("IDR_XML_TEXTENTRY"));
  if (!wf) return;

  wGrid = (WndOwnerDrawFrame*)wf->FindByName(TEXT("frmGrid"));

  cursor = 0;
  edittext[0]= 0;
  edittext[1]= 0;
  if (_tcslen(text)>0) {
    _tcsupr(text);
    _tcsncpy(edittext, text, MAX_TEXTENTRY-1);
    edittext[MAX_TEXTENTRY-1]= 0;
  }
  MoveCursor();

  wf->SetKeyDownNotify(FormKeyDown);

  wf->ShowModal();

  _tcscpy(text, edittext);

  delete wf;

}