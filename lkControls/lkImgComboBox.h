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
/* *************************************************************************************
 * class lkImageBox : base (sub-)class for custom wxOwnerDrawCombo- and -ListBox
 * partially based at <wx/generic/bmpcbox.h> and <wx/generic/bmpcboxg.cpp>
 * ************************************************************************************* */
#ifndef __LKIMGCOMBOBOX_H__
#define __LKIMGCOMBOBOX_H__
#pragma once

#include "lkImageBox.h"

#if !defined(__WXMSW__)

#include <wx/odcombo.h>
class lkImgComboBox : public wxOwnerDrawnComboBox, public lkImageBox
{
public:
	lkImgComboBox();
	lkImgComboBox(wxWindow* parent, wxWindowID id = wxID_ANY, const wxSize& size= wxDefaultSize, long style = 0,
				  const wxValidator& validator = wxDefaultValidator, const wxPoint& pos = wxDefaultPosition);
	virtual ~lkImgComboBox();

	bool						Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxSize& size = wxDefaultSize, long style = 0,
									   const wxValidator& validator = wxDefaultValidator, const wxPoint& pos = wxDefaultPosition);

	// Attributes
private:
	void						Init(void);
	static const char*			lkImageComboBoxNameStr;

public:
	virtual wxString			GetStringSelection(void) const wxOVERRIDE;
	virtual bool				SetFont(const wxFont& font) wxOVERRIDE;

	wxUint64					GetLParam(unsigned int) const;
	void						SetLParam(unsigned int, wxUint64);

	// Searches the list for the lparam, if found will select that item
	virtual void				SetSelectedLParam(wxUint64);
	// From the selected item, the lparam will be returned
	virtual wxUint64			GetSelectedLParam(void) const;

	// Must override folowing while GTK keeps a white background, only takes over foreground
	// and so it gives bizar results. MSW doesn't take foreground by default so that's okay
    virtual void				InheritAttributes() wxOVERRIDE;

	// Overrides
protected:
	// Must override these 4 while it's used inside class lkImageBox

	virtual int					AddItem(lkBoxInit*, wxBitmap*) wxOVERRIDE;
	virtual wxClientData*		GetItemObject(unsigned int) const wxOVERRIDE;
	virtual wxWindow*			GetControl(void) wxOVERRIDE;
	virtual void				StartOver(void) wxOVERRIDE;

	virtual void				OnDrawBackground(wxDC& dc, const wxRect& rect, int item, int flags) const wxOVERRIDE;
	virtual void				OnDrawItem(wxDC& dc, const wxRect& rect, int item, int flags) const wxOVERRIDE;
	virtual wxCoord				OnMeasureItem(size_t item) const wxOVERRIDE;
	virtual wxCoord				OnMeasureItemWidth(size_t item) const wxOVERRIDE;

	virtual wxSize				DoGetBestSize(void) const wxOVERRIDE;

	// Event handlers
protected:
	void						OnSize(wxSizeEvent& event);

private:
	wxDECLARE_EVENT_TABLE();

	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkImgComboBox);
};

#else
// wxMSW only
#include <wx/combobox.h>
class lkImgComboBox : public wxComboBox, public lkImageBox
{
public:
	lkImgComboBox();
	lkImgComboBox(wxWindow* parent, wxWindowID id = wxID_ANY, const wxSize& size = wxDefaultSize, long style = 0,
				  const wxValidator& validator = wxDefaultValidator, const wxPoint& pos = wxDefaultPosition);
	virtual ~lkImgComboBox();

	bool						Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxSize& size=wxDefaultSize, long style = 0,
									   const wxValidator& validator = wxDefaultValidator, const wxPoint& pos = wxDefaultPosition);

	// Attributes
private:
	void						Init(void);

	static const char*			lkImageComboBoxNameStr;

public:
	virtual wxString			GetStringSelection(void) const wxOVERRIDE;
	virtual bool				SetFont(const wxFont& font) wxOVERRIDE;

	wxUint64					GetLParam(unsigned int) const;
	void						SetLParam(unsigned int, wxUint64);

	// Searches the list for the lparam, if found will select that item
	virtual void				SetSelectedLParam(wxUint64);
	// From the selected item, the lparam will be returned
	virtual wxUint64			GetSelectedLParam(void) const;

	// Overrides
protected:
	// Must override these 4 while it's used inside class lkImageBox

	virtual int					AddItem(lkBoxInit*, wxBitmap*) wxOVERRIDE;
	virtual wxClientData*		GetItemObject(unsigned int) const wxOVERRIDE;
	virtual wxWindow*			GetControl(void) wxOVERRIDE;
	virtual void				StartOver(void) wxOVERRIDE;

protected:
	WXDWORD						MSWGetStyle(long style, WXDWORD* exstyle) const wxOVERRIDE;
	virtual bool				MSWOnDraw(WXDRAWITEMSTRUCT* item) wxOVERRIDE;
	virtual bool				MSWOnMeasure(WXMEASUREITEMSTRUCT* item) wxOVERRIDE;

	virtual wxSize				DoGetBestSize(void) const wxOVERRIDE;

private:
	virtual WXHWND				GetEditHWND() const wxOVERRIDE;

	// Event handlers
protected:
	void						OnSize(wxSizeEvent& event);

private:
	wxDECLARE_EVENT_TABLE();

	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkImgComboBox);
};

#endif // defined(__WXMSW__)

/* *************************************************************************************
 * class lkImgComboBoxValidator : Validator for lkImgComboBox based at LParam (wxUint64)
 * ************************************************************************************* */

class lkImgComboBoxValidator : public wxValidator
{
public:
	lkImgComboBoxValidator();
	lkImgComboBoxValidator(wxUint64*);
	lkImgComboBoxValidator(const lkImgComboBoxValidator&);

	virtual wxObject*				Clone() const wxOVERRIDE;

	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	wxUint64*						m_pLParam;

	lkImgComboBox*					GetControl(void);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkImgComboBoxValidator);
};

#endif // !__LKIMGCOMBOBOX_H__

