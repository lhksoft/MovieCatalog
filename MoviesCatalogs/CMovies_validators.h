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
#ifndef __CMOVIES_VALIDATORS_H__
#define __CMOVIES_VALIDATORS_H__
#pragma once

#include "MoviesCopy.h"

#include "../lkSQLite3/lkSQL3RecordSet.h"
#include "../lkSQLite3/lkDateTime.h"
#include <wx/validate.h>
#include "../lkSQLite3/lkSQL3Validators.h"
#include "lkSQL3Valid.h"
#include "GenresBox.h" // for its validator

// /////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesDateValidator
// -------------------------
// A Validator to be used if only using a Year instead of a full date
////
class MoviesYearValidator : public wxTextValidator
{
public:
	MoviesYearValidator();
	MoviesYearValidator(lkDateTime*, lkSQL3FieldData_Real*, lkSQL3RecordSet*, MoviesCopy* pMC = NULL, MoviesCopy::What = MoviesCopy::What::Invalid);
	MoviesYearValidator(const MoviesYearValidator&);
	virtual ~MoviesYearValidator();

	// required overrides
	virtual wxObject*			Clone() const wxOVERRIDE;

	// returns the error message if the contents of 'str' are invalid
	virtual wxString			IsValid(const wxString& str) const wxOVERRIDE;

	virtual bool				Validate(wxWindow* parent) wxOVERRIDE;

	// Called to transfer data from m_sql3Field to the lkDateTime* object, also initializes internal data
	virtual bool				TransferToWindow() wxOVERRIDE;

	// Called to transfer data from the lkDateTime* object to the m_sql3Field, only takes the value from the txtCtrl if working with Year-only
	virtual bool				TransferFromWindow() wxOVERRIDE;

	bool						WindowToDateTime(void);
	bool						DateTimeToWindow(void);

	void						AllwaysRequired(bool);
	void						AddOnlyRequired(bool);

	void						SetMinYear(int); // <0 will set it to default 1900, 0 = not using minimum when validating
	void						SetMaxYear(int); // <0 will set it to default Current Year, 0 = not using maximum when validating

protected:
	lkSQL3RecordSet*			m_pRS;
	lkSQL3FieldData_Real*		m_pField;
	lkDateTime*					m_pDate;

	bool						m_bReqAdd; // if true, required only when adding (setting in database will be ignored)
	bool						m_bReqAll; // if true, always required (setting in database will be ignored)
	int							m_nMinYear; // if !0 it will be the minimal year when validating, default is 1900
	int							m_nMaxYear; // if 0 no maxYear will be used, default is Current Year (when validating)

	MoviesCopy::What			m_eWhat;
	MoviesCopy*					m_pMoviesCopy;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(MoviesYearValidator);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Validators to use within CMovies
////

/////////////////////////////////////////////////////////////////////////////
// Image based at filepath
////
class mcImgPathValidator : public lkSQL3ImgPathValidator
{
public:
	mcImgPathValidator();
	mcImgPathValidator(const mcImgPathValidator&);
	mcImgPathValidator(lkSQL3FieldData_Text*, MoviesCopy*, MoviesCopy::What);

	virtual wxObject*				Clone() const wxOVERRIDE;
	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	MoviesCopy::What				m_eWhat;
	MoviesCopy*						m_pMoviesCopy;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(mcImgPathValidator);
};

/////////////////////////////////////////////////////////////////////////////
// (EditCtrl) Text
////
class mcTextValidator : public lkSQL3TextValidator
{
public:
	mcTextValidator();
	mcTextValidator(const mcTextValidator&);
	mcTextValidator(lkSQL3FieldData*, MoviesCopy*, MoviesCopy::What, long style = wxFILTER_NONE, TextFieldIsUnique_Callback pUnique = NULL, lkSQL3Database* pDB = NULL, void* param = NULL);

	virtual wxObject*				Clone() const wxOVERRIDE;
	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	MoviesCopy::What				m_eWhat;
	MoviesCopy*						m_pMoviesCopy;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(mcTextValidator);
};

/////////////////////////////////////////////////////////////////////////////
// ImgCombo
////
class mcImgComboValidator : public lkSQL3ImgComboValidator
{
public:
	mcImgComboValidator();
	mcImgComboValidator(const mcImgComboValidator&);
	mcImgComboValidator(lkSQL3FieldData_Int*, MoviesCopy*, MoviesCopy::What, const wxString& valTitle, bool* bReqAdd);

	virtual wxObject*				Clone() const wxOVERRIDE;
	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	MoviesCopy::What				m_eWhat;
	MoviesCopy*						m_pMoviesCopy;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(mcImgComboValidator);
};

/////////////////////////////////////////////////////////////////////////////
// Genres
////
class mcGenresValidator : public GenresValidator
{
public:
	mcGenresValidator();
	mcGenresValidator(const mcGenresValidator&);
	mcGenresValidator(lkSQL3FieldData_Int*, MoviesCopy*, MoviesCopy::What);

	virtual wxObject*				Clone() const wxOVERRIDE;
	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	MoviesCopy::What				m_eWhat;
	MoviesCopy*						m_pMoviesCopy;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(mcGenresValidator);
};

/////////////////////////////////////////////////////////////////////////////
// Date
////
class mcDateValidator : public lkSQL3DateValidator
{
public:
	mcDateValidator();
	mcDateValidator(const mcDateValidator&);
	mcDateValidator(lkDateTime*, lkSQL3FieldData_Real*, lkSQL3RecordSet*, MoviesCopy*, MoviesCopy::What, bool longDate = true, bool dutchFormat = false);

	virtual wxObject*				Clone() const wxOVERRIDE;
	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	MoviesCopy::What				m_eWhat;
	MoviesCopy*						m_pMoviesCopy;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(mcDateValidator);
};


/////////////////////////////////////////////////////////////////////////////
// CheckBox
////
class mcChkBoxValidator : public lkSQL3ChkBoxValidator
{
public:
	mcChkBoxValidator();
	mcChkBoxValidator(const mcChkBoxValidator&);
	mcChkBoxValidator(lkSQL3FieldData_Int*, MoviesCopy*, MoviesCopy::What);

	virtual wxObject*				Clone() const wxOVERRIDE;
	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	MoviesCopy::What				m_eWhat;
	MoviesCopy*						m_pMoviesCopy;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(mcDateValidator);
};


/////////////////////////////////////////////////////////////////////////////
// Hyperlink
////
class mcHyperlinkValidator : public lkSQL3HyperlinkValidator
{
public:
	mcHyperlinkValidator();
	mcHyperlinkValidator(const mcHyperlinkValidator&);
	mcHyperlinkValidator(lkSQL3FieldData_Text*, MoviesCopy*, MoviesCopy::What);

	virtual wxObject*				Clone() const wxOVERRIDE;
	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	MoviesCopy::What				m_eWhat;
	MoviesCopy*						m_pMoviesCopy;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(mcHyperlinkValidator);
};

#endif // !__CMOVIES_VALIDATORS_H__
