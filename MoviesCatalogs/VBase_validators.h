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
#ifndef __V_BASE_VALIDATORS_H__
#define __V_BASE_VALIDATORS_H__
#pragma once

#include "../lkControls/lkStaticImage.h"
#include "../lkSQLite3/lkSQL3Validators.h"
#include "lkSQL3Valid.h"

////////////////////////////////////////////////////////////////////////////////////////////////
// class lkStaticHyperlinkValidator -- used in BaseMoviesCanvas-Control
////
class lkStaticHyperlinkValidator : public lkSQL3HyperlinkValidator
{
public:
	lkStaticHyperlinkValidator();
	lkStaticHyperlinkValidator(lkSQL3FieldData_Text*, bool* pValid);
	lkStaticHyperlinkValidator(const lkStaticHyperlinkValidator&);
	virtual ~lkStaticHyperlinkValidator();

	// required overrides
	virtual wxObject*			Clone() const wxOVERRIDE;
	virtual bool				Validate(wxWindow* parent) wxOVERRIDE; // always returns TRUE
	virtual bool				TransferToWindow() wxOVERRIDE;
	virtual bool				TransferFromWindow() wxOVERRIDE;

protected:
	bool*						m_pValid;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkStaticHyperlinkValidator);
};

////////////////////////////////////////////////////////////////////////////////////////////////
// class lkStaticChkbxValidator -- used in BaseMoviesCanvas-Control
////
class lkStaticChkbxValidator : public lkSQL3ChkBoxValidator
{
public:
	lkStaticChkbxValidator();
	lkStaticChkbxValidator(lkSQL3FieldData_Int*, bool* pValid);
	lkStaticChkbxValidator(const lkStaticChkbxValidator&);
	virtual ~lkStaticChkbxValidator();

	// required overrides
	virtual wxObject*			Clone() const wxOVERRIDE;
	virtual bool				Validate(wxWindow* parent) wxOVERRIDE; // always returns TRUE
	virtual bool				TransferToWindow() wxOVERRIDE;
	virtual bool				TransferFromWindow() wxOVERRIDE;

protected:
	bool*						m_pValid;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkStaticChkbxValidator);
};

////////////////////////////////////////////////////////////////////////////////////////////////
// class lkStaticCoverValidator -- used in BaseMoviesCanvas-Control
////
class lkStaticCoverValidator : public lkSQL3ImgPathValidator
{
public:
	lkStaticCoverValidator();
	lkStaticCoverValidator(lkSQL3FieldData_Text*, bool* pValid);
	lkStaticCoverValidator(const lkStaticCoverValidator&);
	virtual ~lkStaticCoverValidator();

	// required overrides
	virtual wxObject*			Clone() const wxOVERRIDE;
	virtual bool				Validate(wxWindow* parent) wxOVERRIDE; // always returns TRUE
	virtual bool				TransferToWindow() wxOVERRIDE;
	virtual bool				TransferFromWindow() wxOVERRIDE;

protected:
	bool*						m_pValid;
	lkStaticImage*				GetStatImageCtrl(void); // works with lkStaticImage and ImageCtrl

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkStaticChkbxValidator);
};
	
////////////////////////////////////////////////////////////////////////////////////////////////
// class lkStaticCountyYearValidator -- used in BaseMoviesCanvas-Control : combination of Country/Year fields
////
class lkStaticCountyYearValidator : public lkSQL3StaticTxtValidator
{
public:
	lkStaticCountyYearValidator();
	lkStaticCountyYearValidator(lkSQL3FieldData*, lkSQL3FieldData_Real*, bool* pValid);
	lkStaticCountyYearValidator(const lkStaticCountyYearValidator&);
	virtual ~lkStaticCountyYearValidator();

	// required overrides
	virtual wxObject*			Clone() const wxOVERRIDE;
	virtual bool				TransferToWindow() wxOVERRIDE;

protected:
	lkSQL3FieldData_Real*		m_pFldDate;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3StaticTxtValidator);
};

////////////////////////////////////////////////////////////////////////////////////////////////
// class AudioSubsValidator
////
class lkAudioSubsValidator : public wxValidator
{
public:
	lkAudioSubsValidator();
	lkAudioSubsValidator(lkSQL3FieldData_Text*);
	lkAudioSubsValidator(wxString*);
	lkAudioSubsValidator(const lkAudioSubsValidator&);
	virtual ~lkAudioSubsValidator();

	virtual wxObject*				Clone() const wxOVERRIDE;

	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	lkSQL3FieldData_Text*			m_pField;
	wxString*						m_pString;
	wxWindow*						GetControl(void);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkAudioSubsValidator);
};

////////////////////////////////////////////////////////////////////////////////////////////////
// class GroupValidator
////
class lkGroupValidator : public wxValidator
{
public:
	lkGroupValidator();
	lkGroupValidator(wxUint64*);
	lkGroupValidator(const lkGroupValidator&);
	virtual ~lkGroupValidator();

	virtual wxObject*				Clone() const wxOVERRIDE;

	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	wxUint64*						m_pCurRow;
	wxWindow*						GetControl(void);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkGroupValidator);
};

#endif // !__V_BASE_VALIDATORS_H__
