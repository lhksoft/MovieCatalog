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
#ifndef __LKIMGCLIENTDATA_H__
#define __LKIMGCLIENTDATA_H__
#pragma once

#include <wx/clntdata.h>
#include <wx/bitmap.h>


/* ****************************************************************
 * class lkStrImgData : derived from wxClientData containing
 * ------------------
 *                       - id of the item in the Control
 *                       - id to bitmap in lkImageList
 *                       - optional data (lParam) as a wxUint64
 * **************************************************************** */
 
class lkImgClientData : public wxClientData
{
public:
	lkImgClientData(wxBitmap* bmp = NULL, wxUint64 lParam = 0);
	virtual ~lkImgClientData();

	wxUint64	GetLPARAM(void) const;
	void		SetLPARAM(wxUint64);

	wxBitmap*	GetBitmap(void) const;
	void		SetBitmap(wxBitmap*); // if NULL removes internal bitmap, otherwise takes copy of the bitmap

private:
	wxUint64	m_lParam; // optional data
	wxBitmap*	m_bitmap; // error with wxImageList, so best keep it in here
};

#endif // !__LKIMGCLIENTDATA_H__
