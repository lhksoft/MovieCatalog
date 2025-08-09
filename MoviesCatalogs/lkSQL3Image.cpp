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
#include "lkSQL3Image.h"
#include "lkSQL3Image_Internal.h"

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------- //

///////////////////////////////////////////////////////////////////////////////
// class lkSQL3FieldData_Image

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3FieldData_Image, lkSQL3FieldData_Blob)

lkSQL3FieldData_Image::lkSQL3FieldData_Image() : lkSQL3FieldData_Blob()
{
}
lkSQL3FieldData_Image::lkSQL3FieldData_Image(const lkSQL3FieldData_Image& other) : lkSQL3FieldData_Blob(other)
{
}
lkSQL3FieldData_Image::lkSQL3FieldData_Image(const wxString& _name) : lkSQL3FieldData_Blob(_name)
{
}
lkSQL3FieldData_Image::~lkSQL3FieldData_Image()
{
}

// -- Implementation (public) ----------------------------------------------------------------------------------- //

bool lkSQL3FieldData_Image::SetFromImage(const wxImage& _img, const wxString& _name)
{
	if ( !_img.IsOk() )
	{ // empty image given
		if ( !IsNull() )
			SetValue2(NULL, 0);

		// else.. already set as null - no changes
		return true;
	}

	if ( IsSameAs(_img, _name) )
		return true; // the same, don't do any changes

	// ---
	bool bRet = false;
	lkSQL3Image_In* _in = new lkSQL3Image_In();
	if ( _in->SetFromImage(_img, _name) )
	{
		const void* p = _in->GetBlob();
		size_t t = _in->GetSize();
		if ( p && t )
		{
			SetValue2(p, t);
			bRet = true;
		}
	}
	delete _in;
	return bRet;
}

// Converts internal data to Image
//virtual
wxImage lkSQL3FieldData_Image::GetAsImage(wxString& _name)
{
	wxImage img;
	if ( IsNull() )
		return img;

	size_t t = 0;
	const void* p = GetValue2(t);

	if ( p && t )
	{
		lkSQL3Image_Out* _out = new lkSQL3Image_Out();
		if ( _out->SetFromBlob(p, t) )
		{
			img = _out->GetImage();
			_name = _out->GetName();
		}
		delete _out;
	}

	return img;
}

// -- Implementation (protected) -------------------------------------------------------------------------------- //

// Compares internal and given data, false if they are different
bool lkSQL3FieldData_Image::IsSameAs(const wxImage& _img, const wxString& _name)
{
	wxString oN;
	wxImage oI = GetAsImage(oN);
	bool same = false;
	if ( !oI.IsOk() && !_img.IsOk() )
		same = true; // no changes
	else if ( !oI.IsOk() && _img.IsOk() )
		same = false; // changed
	else if ( oI.IsOk() && !_img.IsOk() )
		same = false; // changed
	else
	{
		wxUint8* obuf = NULL, * nbuf = NULL;
		size_t osize = 0, nsize = 0;
		if ( !lkSQL3Image_In::CopyImageToBuffer(oI, (void**)(&obuf), osize) || !lkSQL3Image_In::CopyImageToBuffer(_img, (void**)(&nbuf), nsize) )
		{
			if ( obuf )
				delete[] obuf;
			if ( nbuf )
				delete[] nbuf;
			return false; // changed
		}

		same = (osize == nsize);
		if ( same )
			same = (memcmp(obuf, nbuf, osize) == 0);
		delete[] obuf;
		delete[] nbuf;

		if ( same )
			same = (!oN.IsEmpty() && !_name.IsEmpty() && oN.IsSameAs(_name, true));
	}

	return same;
}
