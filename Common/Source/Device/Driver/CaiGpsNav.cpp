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

//


// CAUTION!
// caiGpsNavParseNMEA is called from com port read thread
// all other functions are called from windows message loop thread

#include "Device/Driver/CaiGpsNav.hpp"
#include "Device/Internal.hpp"
#include "Device/Port.h"

#include <tchar.h>

#define  CtrlC  0x03
#define  swap(x)      x = ((((x<<8) & 0xff00) | ((x>>8) & 0x00ff)) & 0xffff)

class CaiGpsNavDevice : public AbstractDevice {
private:
  ComPort *port;

public:
  CaiGpsNavDevice(ComPort *_port):port(_port) {}

public:
  virtual bool Open();
};

bool
CaiGpsNavDevice::Open()
{
  port->WriteString(_T("\x03"));
  Sleep(50);
  port->WriteString(_T("NMEA\r"));

  // This is for a slightly different mode, that
  // apparently outputs pressure info too...
  //(d->Com.WriteString)(_T("PNP\r\n"));
  //(d->Com.WriteString)(_T("LOG 0\r\n"));

  return true;
}

static Device *
CaiGpsNavCreateOnComPort(ComPort *com_port)
{
  return new CaiGpsNavDevice(com_port);
}

const struct DeviceRegister caiGpsNavDevice = {
  _T("CAI GPS-NAV"),
  drfGPS,
  CaiGpsNavCreateOnComPort,
};
