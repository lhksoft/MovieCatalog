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
#ifndef __LKTOOLS_H__
#define __LKTOOLS_H__
#pragma once

#include <wx/gdicmn.h>
#include <wx/image.h>

#ifndef lkZeroMemory
#define lkZeroMemory(Destination,Length)	memset((Destination),0,(Length))
#endif // !lkZeroMemory


enum class lkResizeOptions {
	FIT_STRETCHED = 0,
	FIT_SCALED = 1,
	FIT_CENTERED = 2
};

enum class lkExpandFlags {
	EXPAND_Center = 0,		// centers the image
	EXPAND_Left,			// image will be LEFT aligned
	EXPAND_Right			// image will be RIGHT aligned
};

bool CalculateEstimateRect(wxRect*, const wxImage&, lkResizeOptions);
wxRect CalculateEstimateRect(const wxRect&, const wxImage&, lkResizeOptions);

wxRect Expand(const wxSize& expSize, const wxSize& imgSize, lkExpandFlags);
wxImage Expand(const wxImage&, const wxSize&, lkExpandFlags);

#endif // !__LKTOOLS_H__
