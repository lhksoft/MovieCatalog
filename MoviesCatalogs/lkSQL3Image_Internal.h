/* ********************************************************************************
 * MoviesCatalog - a Programm to catalogue a private collection of Movies using SQLite3
 * Copyright (C) 2022 by Laurens Koehoorn (lhksoft)
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * ********************************************************************************/
#ifndef __LK_SQL3IMAGE_INTERNAL_H__
#define __LK_SQL3IMAGE_INTERNAL_H__
#pragma once

#include <wx/image.h>
#include <wx/string.h>

// helper Class to create BLOB from image
class lkSQL3Image_In
{
public:
	lkSQL3Image_In();
	virtual ~lkSQL3Image_In();

public:
	// Sets internal data from an Image --- internal data will become the BLOB
	bool						SetFromImage(const wxImage&, const wxString& _name = wxEmptyString);

	const void*					GetBlob(void) const;
	size_t						GetSize(void) const;

	static bool					CopyImageToBuffer(const wxImage& _img, void** pBuf, size_t& size);

protected:
	wxUint8*					m_pBuf;
	size_t						m_nSize;

private:
	wxDECLARE_NO_COPY_CLASS(lkSQL3Image_In);
};

// --------------------------------------------------------------------------------------------------------------- //

// helper Class to extract Image from BLOB
class lkSQL3Image_Out
{
public:
	lkSQL3Image_Out();
	virtual ~lkSQL3Image_Out();

public:
	// Analizes internal Blob whether it's something of ours
	bool						SetFromBlob(const void*, size_t);

	wxImage						GetImage(void) const;
	wxString					GetName(void) const;
	wxBitmapType				GetType(void) const;

private:
	const wxBitmapType			ConvertLkImageType(wxUint32) const;

protected:
	wxImage						m_Image;
	wxString					m_Name;
	wxBitmapType				m_Type;

private:
	wxDECLARE_NO_COPY_CLASS(lkSQL3Image_Out);
};


#endif // !__LK_SQL3IMAGE_INTERNAL_H__
