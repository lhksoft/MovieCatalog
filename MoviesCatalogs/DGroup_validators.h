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
#ifndef __D_GROUP_VALIDATORS_H__
#define __D_GROUP_VALIDATORS_H__
#pragma once

#include "lkSQL3Valid.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class DGroupStaticImageValidator
////
class DGroupStaticImageValidator : public lkSQL3ImageValidator
{
public:
	DGroupStaticImageValidator();
	DGroupStaticImageValidator(lkSQL3FieldData_Image*, bool* pValid);
	DGroupStaticImageValidator(const DGroupStaticImageValidator&);

	virtual wxObject*				Clone() const wxOVERRIDE;
	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;

protected:
	bool*							m_pValid;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(DGroupStaticImageValidator);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class DGroupStaticDateValidator
////
class DGroupStaticDateValidator : public wxValidator
{
public:
	DGroupStaticDateValidator();
	DGroupStaticDateValidator(lkSQL3FieldData_Real*, bool* pValid, bool longDate = true, bool dutchFormat = false);
	DGroupStaticDateValidator(const DGroupStaticDateValidator&);

	virtual wxObject*				Clone() const wxOVERRIDE;
	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;

protected:
	bool*							m_pValid;
	lkSQL3FieldData_Real*			m_pField;
	bool							m_bLong, m_bDutch;
	wxWindow*						GetStaticControl(void);

	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(DGroupStaticDateValidator);
};

#endif // !__D_GROUP_VALIDATORS_H__
