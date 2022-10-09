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
#include "lkClientData.h"


/* **********************************************************************************************************
 * class lkImgClientData
 * ********************************************************************************************************** */

lkImgClientData::lkImgClientData(wxBitmap* bmp, wxUint64 lParam) : wxClientData()
{
	m_bitmap = NULL;
	m_lParam = lParam;

	SetBitmap(bmp);
}
lkImgClientData::~lkImgClientData()
{
	if (m_bitmap)
		delete m_bitmap;
}

wxUint64 lkImgClientData::GetLPARAM() const
{
	return m_lParam;
}
void lkImgClientData::SetLPARAM(wxUint64 lParam)
{
	m_lParam = lParam;
}

wxBitmap* lkImgClientData::GetBitmap() const
{
	return m_bitmap;
}
void lkImgClientData::SetBitmap(wxBitmap* bmp)
{
	if (m_bitmap) delete m_bitmap;

	if (bmp && bmp->IsOk())
		m_bitmap = new wxBitmap(*bmp);
	else
		m_bitmap = NULL;
}
