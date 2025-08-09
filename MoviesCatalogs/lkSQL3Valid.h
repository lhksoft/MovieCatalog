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
#ifndef __LK_SQL3_VALID_H__
#define __LK_SQL3_VALID_H__
#pragma once

#include <wx/validate.h>
#include <wx/checkbox.h>
#include "../lkSQLite3/lkSQL3FieldData.h"
#include "../lkSQLite3/lkSQL3Database.h"
#include "../lkControls/lkStaticImage.h"
#include "../lkControls/lkImgComboBox.h"

#include "../lkControls/lkHyperlinkCtrl.h"
#include "lkSQL3Image.h"

////////////////////////////////////////////////////////////////////////////////////////////////
// class lkSQL3StaticTxtValidator -- used in BaseMoviesCanvas, and DGroup
////
class lkSQL3StaticTxtValidator : public wxValidator
{
public:
	lkSQL3StaticTxtValidator();
	lkSQL3StaticTxtValidator(lkSQL3FieldData*, bool* pValid);
	lkSQL3StaticTxtValidator(const lkSQL3StaticTxtValidator&);
	virtual ~lkSQL3StaticTxtValidator();

	// required overrides
	virtual wxObject*			Clone() const wxOVERRIDE;
	virtual bool				Validate(wxWindow* parent) wxOVERRIDE; // always returns TRUE
	virtual bool				TransferToWindow() wxOVERRIDE;
	virtual bool				TransferFromWindow() wxOVERRIDE;

protected:
	lkSQL3FieldData*			m_pField;
	bool*						m_pValid;

	wxControl*					GetControl(void);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3StaticTxtValidator);
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

class lkSQL3ImageValidator : public wxValidator
{
public:
	lkSQL3ImageValidator();
	lkSQL3ImageValidator(lkSQL3FieldData_Image*);
	lkSQL3ImageValidator(const lkSQL3ImageValidator&);

	// Make a clone of this validator (or return NULL) - currently necessary
	// if you're passing a reference to a validator.
	// Another possibility is to always pass a pointer to a new validator
	// (so the calling code can use a copy constructor of the relevant class).
	virtual wxObject*				Clone() const wxOVERRIDE;

	// Called when the value in the window must be validated.
	// This function can pop up an error message.
	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;

	// Called to transfer data to the window
	virtual bool					TransferToWindow(void) wxOVERRIDE;

	// Called to transfer data from the window
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	lkSQL3FieldData_Image*			m_pField;

	lkStaticImage*					GetImageCtrl(void); // works with lkStaticImage and ImageCtrl

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3ImageValidator);
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

// Special case Custom Validator : 2 checkboxes, each the same field but using an enum to set/get its value
class lkSQL3CountryChkbValidator : public wxValidator
{
public:
	enum AudioSubs
	{
		audio = 1,
		subs = 2
	};
	lkSQL3CountryChkbValidator();
	lkSQL3CountryChkbValidator(lkSQL3FieldData_Int*, AudioSubs val);
	lkSQL3CountryChkbValidator(const lkSQL3CountryChkbValidator&);


	virtual wxObject*				Clone() const wxOVERRIDE;

	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	lkSQL3FieldData_Int*			m_pField;

	wxCheckBox*						GetCheckbox(void);

	int								m_nVal;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3CountryChkbValidator);
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

// a Validator for lkImgComboBox
class lkSQL3ImgComboValidator : public wxValidator
{
public:
	lkSQL3ImgComboValidator();
	lkSQL3ImgComboValidator(lkSQL3FieldData_Int*, const wxString& valTitle, bool* bReqAdd);
	lkSQL3ImgComboValidator(const lkSQL3ImgComboValidator&);

	virtual wxObject*				Clone() const wxOVERRIDE;

	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	lkSQL3FieldData_Int*			m_pField;
	bool*							m_bReqAdd; // true=Required when Adding
	wxString						m_sValTitle;

	lkImgComboBox*					GetControl(void);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3ImgComboValidator);
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

// class for Validating ImagePaths for inside MoviesCanvas
class lkSQL3ImgPathValidator : public wxValidator
{
public:
	lkSQL3ImgPathValidator();
	lkSQL3ImgPathValidator(lkSQL3FieldData_Text*);
	lkSQL3ImgPathValidator(const lkSQL3ImgPathValidator&);

	virtual wxObject*				Clone() const wxOVERRIDE;

	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	lkSQL3FieldData_Text*			m_pField;
	wxString						m_sCovers; // from INI : [Movies]Covers (if set)

	void							GetCoversPath(void); // called from inside c'tor
	lkStaticImage*					GetImageCtrl(void); // works with lkStaticImage and ImageCtrl
	wxString						MakeFullPath(const wxString& partial);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3ImgComboValidator);
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

// A validator for an URL through a lkHyperlinkCtrl

class lkSQL3HyperlinkValidator : public wxValidator
{
public:
	lkSQL3HyperlinkValidator();
	lkSQL3HyperlinkValidator(lkSQL3FieldData_Text*);
	lkSQL3HyperlinkValidator(const lkSQL3HyperlinkValidator&);
	virtual ~lkSQL3HyperlinkValidator();

	// required overrides
	virtual wxObject*			Clone() const wxOVERRIDE;
	virtual bool				Validate(wxWindow* parent) wxOVERRIDE; // always returns TRUE
	virtual bool				TransferToWindow() wxOVERRIDE;
	virtual bool				TransferFromWindow() wxOVERRIDE;

protected:
	lkSQL3FieldData_Text*		m_pField;

	lkHyperlinkCtrl*			GetControl(void);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3HyperlinkValidator);
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //


#endif // !__LK_SQL3_VALID_H__
