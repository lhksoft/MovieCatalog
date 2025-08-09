/* lkControls - custom controls using wxWidgets
 * Copyright (C) 2022 Laurens Koehoorn (lhksoft)
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef __LKCOLOUR_H__
#define __LKCOLOUR_H__
#pragma once

#include <wx/colour.h>

#if !defined(__WXMSW__)
typedef unsigned long   WXCOLORREF;
#endif


#ifndef MAKE_RGB

#define MAKE_RGB( r/*ed*/, g/*reen*/, b/*lue*/ )	(WXCOLORREF)( (((wxUint32)((wxUint8)(b) << 16)) & 0xFF0000) | (((wxUint16)((wxUint8)(g) << 8)) & 0xFF00) | (((wxUint8)(r)) & 0xFF) )

#define lkCol_AliceBlue					MAKE_RGB(240,248,255)
#define lkCol_AntiqueWhite				MAKE_RGB(250,235,215)
#define lkCol_Aqua						MAKE_RGB(  0,255,255)
#define lkCol_Aquamarine				MAKE_RGB(127,255,212)
#define lkCol_Azure						MAKE_RGB(240,255,255)
#define lkCol_Beige						MAKE_RGB(245,245,220)
#define lkCol_Bisque					MAKE_RGB(255,228,196)
#define lkCol_Black						MAKE_RGB(  0,  0,  0)
#define lkCol_BlanchedAlmond			MAKE_RGB(255,255,205)
#define lkCol_Blue						MAKE_RGB(  0,  0,255)
#define lkCol_BlueViolet				MAKE_RGB(138, 43,226)
#define lkCol_Brown						MAKE_RGB(165, 42, 42)
#define lkCol_Burlywood					MAKE_RGB(222,184,135)
#define lkCol_CadetBlue					MAKE_RGB( 95,158,160)
#define lkCol_Chartreuse				MAKE_RGB(127,255,  0)
#define lkCol_CholkCol_ate				MAKE_RGB(210,105, 30)
#define lkCol_Coral						MAKE_RGB(255,127, 80)
#define lkCol_CornflowerBlue			MAKE_RGB(100,149,237)
#define lkCol_Cornsilk					MAKE_RGB(255,248,220)
#define lkCol_Crimson					MAKE_RGB(220, 20, 60)
#define lkCol_Cyan						MAKE_RGB(  0,255,255)
#define lkCol_DarkBlue					MAKE_RGB(  0,  0,139)
#define lkCol_DarkCyan					MAKE_RGB(  0,139,139)
#define lkCol_DarkGoldenRod				MAKE_RGB(184,134, 11)
#define lkCol_DarkGray					MAKE_RGB(169,169,169)
#define lkCol_DarkGreen					MAKE_RGB(  0,100,  0)
#define lkCol_DarkKhaki					MAKE_RGB(189,183,107)
#define lkCol_DarkMagenta				MAKE_RGB(139,  0,139)
#define lkCol_DarkOliveGreen			MAKE_RGB( 85,107, 47)
#define lkCol_DarkOrange				MAKE_RGB(255,140,  0)
#define lkCol_DarkOrchid				MAKE_RGB(153, 50,204)
#define lkCol_DarkRed					MAKE_RGB(139,  0,  0)
#define lkCol_DarkSalmon				MAKE_RGB(233,150,122)
#define lkCol_DarkSeaGreen				MAKE_RGB(143,188,143)
#define lkCol_DarkSlateBlue				MAKE_RGB( 72, 61,139)
#define lkCol_DarkSlateGray				MAKE_RGB( 47, 79, 79)
#define lkCol_DarkTurquoise				MAKE_RGB(  0,206,209)
#define lkCol_DarkViolet				MAKE_RGB(148,  0,211)
#define lkCol_DeepPink					MAKE_RGB(255, 20,147)
#define lkCol_DeepSkyBlue				MAKE_RGB(  0,191,255)
#define lkCol_DimGray					MAKE_RGB(105,105,105)
#define lkCol_DodgerBlue				MAKE_RGB( 30,144,255)
#define lkCol_FireBrick					MAKE_RGB(178, 34, 34)
#define lkCol_FloralWhite				MAKE_RGB(255,250,240)
#define lkCol_ForestGreen				MAKE_RGB( 34,139, 34)
#define lkCol_Fuchsia					MAKE_RGB(255,  0,255)
#define lkCol_Gainsboro					MAKE_RGB(220,220,220)
#define lkCol_GhostWhite				MAKE_RGB(248,248,255)
#define lkCol_Gold						MAKE_RGB(255,215,  0)
#define lkCol_GoldenRod					MAKE_RGB(218,165, 32)
#define lkCol_Gray						MAKE_RGB(127,127,127)
#define lkCol_Green						MAKE_RGB(  0,128,  0)
#define lkCol_GreenYellow				MAKE_RGB(173,255, 47)
#define lkCol_HoneyDew					MAKE_RGB(240,255,240)
#define lkCol_HotPink					MAKE_RGB(255,105,180)
#define lkCol_IndianRed					MAKE_RGB(205, 92, 92)
#define lkCol_Indigo					MAKE_RGB( 75,  0,130)
#define lkCol_Ivory						MAKE_RGB(255,255,240)
#define lkCol_Khaki						MAKE_RGB(240,230,140)
#define lkCol_Lavender					MAKE_RGB(230,230,250)
#define lkCol_LavenderBlush				MAKE_RGB(255,240,245)
#define lkCol_Lawngreen					MAKE_RGB(124,252,  0)
#define lkCol_LemonChiffon				MAKE_RGB(255,250,205)
#define lkCol_LightBlue					MAKE_RGB(173,216,230)
#define lkCol_LightCoral				MAKE_RGB(240,128,128)
#define lkCol_LightCyan					MAKE_RGB(224,255,255)
#define lkCol_LightGoldenRodYellow		MAKE_RGB(250,250,210)
#define lkCol_LightGreen				MAKE_RGB(144,238,144)
#define lkCol_LightGrey					MAKE_RGB(211,211,211)
#define lkCol_LightPink					MAKE_RGB(255,182,193)
#define lkCol_LightSalmon				MAKE_RGB(255,160,122)
#define lkCol_LightSeaGreen				MAKE_RGB( 32,178,170)
#define lkCol_LightSkyBlue				MAKE_RGB(135,206,250)
#define lkCol_LightSlateGray			MAKE_RGB(119,136,153)
#define lkCol_LightSteelBlue			MAKE_RGB(176,196,222)
#define lkCol_LightYellow				MAKE_RGB(255,255,224)
#define lkCol_Lime						MAKE_RGB(  0,255,  0)
#define lkCol_LimeGreen					MAKE_RGB( 50,205, 50)
#define lkCol_Linen						MAKE_RGB(250,240,230)
#define lkCol_Magenta					MAKE_RGB(255,  0,255)
#define lkCol_Maroon					MAKE_RGB(128,  0,  0)
#define lkCol_MediumAquamarine			MAKE_RGB(102,205,170)
#define lkCol_MediumBlue				MAKE_RGB(  0,  0,205)
#define lkCol_MediumOrchid				MAKE_RGB(186, 85,211)
#define lkCol_MediumPurple				MAKE_RGB(147,112,219)
#define lkCol_MediumSeaGreen			MAKE_RGB( 60,179,113)
#define lkCol_MediumSlateBlue			MAKE_RGB(123,104,238)
#define lkCol_MediumSpringGreen			MAKE_RGB(  0,250,154)
#define lkCol_MediumTurquoise			MAKE_RGB( 72,209,204)
#define lkCol_MediumVioletRed			MAKE_RGB(199, 21,133)
#define lkCol_MidnightBlue				MAKE_RGB( 25, 25,112)
#define lkCol_MintCream					MAKE_RGB(245,255,250)
#define lkCol_MistyRose					MAKE_RGB(255,228,225)
#define lkCol_Moccasin					MAKE_RGB(255,228,181)
#define lkCol_NavajoWhite				MAKE_RGB(255,222,173)
#define lkCol_Navy						MAKE_RGB(  0,  0,128)
#define lkCol_Navyblue					MAKE_RGB(159,175,223)
#define lkCol_OldLace					MAKE_RGB(253,245,230)
#define lkCol_Olive						MAKE_RGB(128,128,  0)
#define lkCol_OliveDrab					MAKE_RGB(107,142, 35)
#define lkCol_Orange					MAKE_RGB(255,165,  0)
#define lkCol_OrangeRed					MAKE_RGB(255, 69,  0)
#define lkCol_Orchid					MAKE_RGB(218,112,214)
#define lkCol_PaleGoldenRod				MAKE_RGB(238,232,170)
#define lkCol_PaleGreen					MAKE_RGB(152,251,152)
#define lkCol_PaleTurquoise				MAKE_RGB(175,238,238)
#define lkCol_PaleVioletRed				MAKE_RGB(219,112,147)
#define lkCol_PapayaWhip				MAKE_RGB(255,239,213)
#define lkCol_PeachPuff					MAKE_RGB(255,218,185)
#define lkCol_Peru						MAKE_RGB(205,133, 63)
#define lkCol_Pink						MAKE_RGB(255,192,203)
#define lkCol_Plum						MAKE_RGB(221,160,221)
#define lkCol_PowderBlue				MAKE_RGB(176,224,230)
#define lkCol_Purple					MAKE_RGB(128,  0,128)
#define lkCol_Red						MAKE_RGB(255,  0,  0)
#define lkCol_RosyBrown					MAKE_RGB(188,143,143)
#define lkCol_RoyalBlue					MAKE_RGB( 65,105,225)
#define lkCol_SaddleBrown				MAKE_RGB(139, 69, 19)
#define lkCol_Salmon					MAKE_RGB(250,128,114)
#define lkCol_SandyBrown				MAKE_RGB(244,164, 96)
#define lkCol_SeaGreen					MAKE_RGB( 46,139, 87)
#define lkCol_SeaShell					MAKE_RGB(255,245,238)
#define lkCol_Sienna					MAKE_RGB(160, 82, 45)
#define lkCol_Silver					MAKE_RGB(192,192,192)
#define lkCol_SkyBlue					MAKE_RGB(135,206,235)
#define lkCol_SlateBlue					MAKE_RGB(106, 90,205)
#define lkCol_SlateGray					MAKE_RGB(112,128,144)
#define lkCol_Snow						MAKE_RGB(255,250,250)
#define lkCol_SpringGreen				MAKE_RGB(  0,255,127)
#define lkCol_SteelBlue					MAKE_RGB( 70,130,180)
#define lkCol_Tan						MAKE_RGB(210,180,140)
#define lkCol_Teal						MAKE_RGB(  0,128,128)
#define lkCol_Thistle					MAKE_RGB(216,191,216)
#define lkCol_Tomato					MAKE_RGB(255, 99, 71)
#define lkCol_Turquoise					MAKE_RGB( 64,224,208)
#define lkCol_Violet					MAKE_RGB(238,130,238)
#define lkCol_Wheat						MAKE_RGB(245,222,179)
#define lkCol_White						MAKE_RGB(255,255,255)
#define lkCol_WhiteSmoke				MAKE_RGB(245,245,245)
#define lkCol_Yellow					MAKE_RGB(255,255,  0)
#define lkCol_YellowGreen				MAKE_RGB(139,205, 50)

#endif // !MAKE_RGB

class lkColour : public wxColour
{
	wxDECLARE_ABSTRACT_CLASS(lkColour);

public:
	lkColour();
	lkColour(WXCOLORREF);

	// calculates a ForeColour which is always viewable at given BackColour
	static WXCOLORREF	GetViewableForeColor(WXCOLORREF BackColour);

	void				SetLighter(int l); // l: 0..100, 0=no change, 100=completely light
	void				SetDarker(int l); // l: 0..100, 0=no change, 100=completely dark

	lkColour			operator = (const lkColour&);
	lkColour			operator = (const wxColour&);
};

#endif // !__LKCOLOUR_H__
