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

#ifndef XCSOAR_SCREEN_LAYOUT_HPP
#define XCSOAR_SCREEN_LAYOUT_HPP

#include "Asset.hpp"

namespace Layout {
  extern bool landscape;
  extern bool square;
  extern double dscale;
  extern bool IntScaleFlag;
  extern int scale;

  /**
   * Initializes the screen layout information provided by this
   * namespace.
   *
   * @param width the width of the screen in pixels
   * @param height the width of the screen in pixels
   */
  void Initalize(unsigned width, unsigned height);

  /**
   * Is scaling supported by this platform?
   */
  static inline bool ScaleSupported() {
    return !is_altair();
  }

  /**
   * Is scaling enabled currently?
   */
  static inline bool ScaleEnabled() {
    return ScaleSupported() && dscale > 1;
  }

  static inline int Scale(int x) {
    if (!ScaleSupported())
      return x;

    return IntScaleFlag
      ? x * scale
      : (int)(x * dscale);
  }

  static inline unsigned Scale(unsigned x) {
    return Scale((int)x);
  }

  static inline long Scale(long x) {
    return Scale((int)x);
  }

  static inline double Scale(double x) {
    if (!ScaleSupported())
      return x;

    return x * dscale;
  }

  static inline int FastScale(int x) {
    if (!ScaleSupported())
      return x;

    return x * scale;
  }
}

#define IBLSCALE(x) Layout::Scale(x)

#endif