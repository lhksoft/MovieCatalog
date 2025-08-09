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
#ifndef __LK_SQL3IMAGE_H__
#define __LK_SQL3IMAGE_H__
#pragma once

#include <wx/image.h>
#include "../lkSQLite3/lkSQL3FieldData.h"

// --------------------------------------------------------------------------------------------------------------- //

class lkSQL3FieldData_Image : public lkSQL3FieldData_Blob
{
public:
	lkSQL3FieldData_Image();
	lkSQL3FieldData_Image(const lkSQL3FieldData_Image&);
	lkSQL3FieldData_Image(const wxString&);

	virtual ~lkSQL3FieldData_Image();

// Implementation
public:
	// Sets internal data from an Image -- true if success
	bool						SetFromImage(const wxImage&, const wxString& _name = wxEmptyString);

	// Analizes internal Blob whether it's something of ours, if not an empty-image is returned -- on succes internal data will be set, returning the image
	wxImage						GetAsImage(wxString& _name);

// Implementation
protected:
	// Compares internal and given data, false if they are different
	bool						IsSameAs(const wxImage&, const wxString& _name = wxEmptyString);

private:
	wxDECLARE_DYNAMIC_CLASS(lkSQL3FieldData_Image);
	wxDECLARE_NO_ASSIGN_CLASS(lkSQL3FieldData_Image);
};

// --------------------------------------------------------------------------------------------------------------- //

#endif // !__LK_SQL3IMAGE_H__
