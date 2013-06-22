/*

  colorUtilites.h

  $Date: 2012-02-25 09:44:01 -0800 (Sat, 25 Feb 2012) $
  $Revision: 273 $
  $Author: mauricio $
  
  Code used without permission from:

  http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb

*/

#ifndef colorUtilities_h
#define colorUtilities_h

typedef struct {
    float r;       // percent
    float g;       // percent
    float b;       // percent
} rgb;

typedef struct {
    float h;       // angle in degrees
    float s;       // percent
    float v;       // percent
} hsv;

hsv rgb2hsv(rgb in);
rgb hsv2rgb(hsv in);

#endif
