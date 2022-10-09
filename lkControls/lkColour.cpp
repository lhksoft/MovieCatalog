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
#include "lkColour.h"
#include <wx/gdicmn.h>
#include <math.h>

wxIMPLEMENT_ABSTRACT_CLASS(lkColour, wxColour);

lkColour::lkColour() : wxColour()
{
}
lkColour::lkColour(WXCOLORREF col) : wxColour(col)
{
}

// calculates a ForeColour which is always viewable at given BackColour
//static
WXCOLORREF lkColour::GetViewableForeColor(WXCOLORREF BackColour)
{
	lkColour bc(BackColour);
	if (!bc.IsOk())
		return (WXCOLORREF)wxNullColour.GetRGB();

	// calculate a colour that is viewable on given background colour
	WXCOLORREF rgbText;
	int r, g, b, rt, gt, bt;
	div_t Result;

	Result = div(BackColour, 256);
	r = Result.rem;		rt = 255 - r;

	Result = div(Result.quot, 256);
	g = Result.rem;		gt = 255 - g;
	b = Result.quot;	bt = 255 - b;

	if ((abs(rt - r) < 50) && (abs(gt - g) < 50) && (abs(bt - b) < 50))
		rgbText = lkCol_Black;
	else
		rgbText = MAKE_RGB(rt, gt, bt);

	return rgbText;
}

void lkColour::SetLighter(int l) // l: 0..100, 0=no change, 100=completely light
{
	int ll = (l > 100) ? 100 : (l < 0) ? 0 : l;
	int alpha = 100 + ll;

	*this = ChangeLightness(alpha);
}
void lkColour::SetDarker(int l) // l: 0..100, 0=no change, 100=completely dark
{
	int ll = (l > 100) ? 100 : (l < 0) ? 0 : l;
	int alpha = 100 - ll;

	*this = ChangeLightness(alpha);
}

lkColour lkColour::operator = (const lkColour& l)
{
	Set(l.GetRGBA());
	return *this;
}
lkColour lkColour::operator = (const wxColour& w)
{
	Set(w.GetRGBA());
	return *this;
}
