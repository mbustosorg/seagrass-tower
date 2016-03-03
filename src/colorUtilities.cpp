/*

    Copyright (C) 2016 Mauricio Bustos (m@bustos.org)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Code used without permission from:
    
    http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb

*/

#include "colorUtilities.h"

//! Procedure to convert from RGB to HSV
hsv rgb2hsv(rgb in)
{
    hsv         out;
    float      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (max > 0.0) {
        out.s = (delta / max);                  // s
    } else {
        // r = g = b = 0                        // s = 0, v is undefined
        out.s = 0.0;
        out.h = 0.0;                            // its now undefined
        return out;
    }
    if (in.r >= max) {                           // > is bogus, just keeps compiler happy
        out.h = (in.g - in.b ) / delta;        // between yellow & magenta
	} else {
	  if (in.g >= max) {
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
	  } else {
		out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan
	  }
	}

    out.h *= 60.0;                              // degrees

    if (out.h < 0.0) {
	  out.h += 360.0;
	} else if (out.h >= 360.0) {
	  out.h -= 360.0;
	}

    return out;
}

//! Procedure to convert from HSV to RGB
rgb hsv2rgb(hsv in)
{
    float      hh, p, q, t, ff;
    long        i;
    rgb         out;

    hh = in.h;
	if (hh > 360.0) {
	  hh -= 360.0;
	} else if (hh < 0.0) {
	  hh += 360.0;
	}
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}
