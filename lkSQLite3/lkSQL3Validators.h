/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3VALIDATORS_H__
#define __LK_SQL3VALIDATORS_H__
#pragma once

#include <wx/validate.h>
#include <wx/valtext.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>

#include "lkDateTime.h"
#include "lkSQL3FieldData.h"
#include "lkSQL3RecordSet.h"
#include "lkSQL3Database.h"

// last param 'void*' either is wxUint32* or wxUint64* -- if NULL the routine should assume a default value of '0'
typedef bool(*TextFieldIsUnique_Callback)(lkSQL3Database*, const wxString&, void*);
// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

// Extracts or Makes a Hyperlink (url+label) suitable for storing in the DB (1 text field) from/to separate URL / Label
bool ExtractDbHyperlink(const wxString& lpszHyperlink, wxString& sDisplay, wxString& sURL);
bool MakeDbHyperlink(wxString& sHyperlink, const wxString& szDisplay, const wxString& szURL);

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

const wxString GetRequiredText(void);
const wxString GetNotUniqueText(void);

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

// A validator for a DATE through a TextCtrl
class lkSQL3DateValidator : public wxValidator
{
public:
	lkSQL3DateValidator();
	lkSQL3DateValidator(lkDateTime*, lkSQL3FieldData_Real*, lkSQL3RecordSet*, bool longDate = true, bool dutchFormat = false);
	lkSQL3DateValidator(const lkSQL3DateValidator&);
	virtual ~lkSQL3DateValidator();

	// required overrides
	virtual wxObject*			Clone() const wxOVERRIDE;

	// returns the error message if lkDateTime* is invalid
	virtual wxString			IsValid(void) const;

	// Called when the value in the window must be validated.
	// Override if you need some extra validation
	virtual bool				Validate(wxWindow* parent) wxOVERRIDE;

	// Called to transfer data from m_sql3Field to the window, also initializes internal data
	virtual bool				TransferToWindow() wxOVERRIDE;

	// Called to transfer data from the window
	virtual bool				TransferFromWindow() wxOVERRIDE;

	bool						DateTimeToWindow(bool bAllowEmpty = false); // if true and invalid date, control will be set empty

	void						AllwaysRequired(bool);
	void						AddOnlyRequired(bool);

	void						SetMinYear(int); // <0 will set it to default 1900, 0 = not using minimum when validating
	void						SetMaxYear(int); // <0 will set it to default Current Year, 0 = not using maximum when validating

	

protected:
	lkSQL3RecordSet*			m_pRS;
	lkSQL3FieldData_Real*		m_pField;
	lkDateTime*					m_pDate;
	bool						m_bDutch; // if false, english style will be used when printing full date
	bool						m_bLongDate; // true : "w d m y", false : "dd-mmm-yyyy"

	bool						m_bReqAdd; // if true, required only when adding (setting in database will be ignored)
	bool						m_bReqAll; // if true, always required (setting in database will be ignored)
	int							m_nMinYear; // if !0 it will be the minimal year when validating, default is 1900
	int							m_nMaxYear; // if 0 no maxYear will be used, default is Current Year (when validating)

	wxTextCtrl*					GetTextEntry(void);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3DateValidator);
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

// A validator to be used at checkboxes, setting the checkbox checked if it's 'true' in the database or unchecked if it's 'false' in the db
class lkSQL3ChkBoxValidator : public wxValidator
{
public:
	lkSQL3ChkBoxValidator();
	lkSQL3ChkBoxValidator(lkSQL3FieldData_Int*); // Data will be retrieved/set using this sql3_field, threatend as would it of type bool
	lkSQL3ChkBoxValidator(const lkSQL3ChkBoxValidator&);
	virtual ~lkSQL3ChkBoxValidator();

	// required overrides
	virtual wxObject*			Clone() const wxOVERRIDE;

	// Called when the value in the window must be validated.
	// If valid, the data will be transfered to the database
	virtual bool				Validate(wxWindow* parent) wxOVERRIDE;

	// Called to transfer data from m_sql3Field to the window, also initializes internal data
	virtual bool				TransferToWindow() wxOVERRIDE;

	// Called to transfer data from the window
	virtual bool				TransferFromWindow() wxOVERRIDE;

protected:
	lkSQL3FieldData_Int*		m_sql3Field;

	wxCheckBox*					GetCheckbox(void);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3ChkBoxValidator);
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

class lkSQL3TextValidator : public wxTextValidator
{
public:
	lkSQL3TextValidator();
	lkSQL3TextValidator(lkSQL3FieldData*, long style = wxFILTER_NONE, TextFieldIsUnique_Callback pUnique = NULL, lkSQL3Database* pDB = NULL, void* param = NULL);
	lkSQL3TextValidator(const lkSQL3TextValidator&);

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
	lkSQL3FieldData*				m_pField;
	TextFieldIsUnique_Callback		m_pUniqueTextCallback; // used to verify that entered string is unique if required by the database
	lkSQL3Database*					m_pDB; // parameter for the callback
	void*							m_pParam; // parameter for the callback

	wxTextEntry*					GetTextEntry(void);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3TextValidator);
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

// A validator for a lkDateTimeSpan through a TextCtrl
class lkSQL3DateTimeSpanValidator : public wxValidator
{
public:
	lkSQL3DateTimeSpanValidator();
	lkSQL3DateTimeSpanValidator(lkDateTimeSpan*, lkSQL3FieldData_Real*, lkSQL3RecordSet*, bool bReqAdd = false, bool bReqAll = false);
	lkSQL3DateTimeSpanValidator(const lkSQL3DateTimeSpanValidator&);
	virtual ~lkSQL3DateTimeSpanValidator();

	// required overrides
	virtual wxObject*			Clone() const wxOVERRIDE;

	// returns the error message if lkDateTime* is invalid
	virtual wxString			IsValid(void) const;

	// Called when the value in the window must be validated.
	// Override if you need some extra validation
	virtual bool				Validate(wxWindow* parent) wxOVERRIDE;

	// Called to transfer data from m_sql3Field to the window, also initializes internal data
	virtual bool				TransferToWindow() wxOVERRIDE;

	// Called to transfer data from the window -- it will take the value stored in 'm_pDateSpan', the data in the Ctrl will be ignored !
	virtual bool				TransferFromWindow() wxOVERRIDE;

	virtual bool				DateTimeToWindow(bool bAllowEmpty = false); // if true and invalid date, control will be set empty

	void						AllwaysRequired(bool);
	void						AddOnlyRequired(bool);

protected:
	lkSQL3RecordSet*			m_pRS;
	lkSQL3FieldData_Real*		m_pField;
	lkDateTimeSpan*				m_pDateSpan;

	bool						m_bReqAdd; // if true, required only when adding (setting in database will be ignored)
	bool						m_bReqAll; // if true, always required (setting in database will be ignored)

	wxTextCtrl*					GetTextEntry(void);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3DateTimeSpanValidator);
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //


#endif // !__LK_SQL3VALIDATORS_H__
