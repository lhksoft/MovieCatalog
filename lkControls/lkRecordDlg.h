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
#ifndef __LK_RECORDDLG_H__
#define __LK_RECORDDLG_H__
#pragma once

#include "lkDialogEx.h"
/////////////////////////////////////////////////////////////////////////////////////////////
// class lkRecordDlg
//
// A combination of a wxDialog, acting like a wxFrame with a Tool- and Statusbar and optional a Menubar
//                    wxDocument (record-style)
//                    wxPanel
////
// For this class, Create must be called seperately because of virtual routines used inside Create
/////////////////////////////////////////////////////////////////////////////////////////////
class lkRecordDlg : public lkDialogEx
{
public:
	lkRecordDlg();
	virtual ~lkRecordDlg();

	bool							Create(wxWindow* parent, const wxBitmap& background, const wxString& title,
										   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	bool							Create(wxWindow* parent, const wxImage& background, const wxString& title,
										   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// return a new status bar
	virtual wxStatusBar*			OnCreateStatusBar(int number, long style, wxWindowID winid) wxOVERRIDE;

	// return a new toolbar
	// creates the tool-items of the Toolbar -- override to replace or add items (then call this func too)
	virtual wxToolBar*				OnCreateToolBar(long style, wxWindowID winid, const wxString& name) wxOVERRIDE;

	virtual void					BindButtons(void);

protected:
	// Adds entries to the frame's accelerator key table.  Derived classes should call up to their parents.
	virtual void					AddAcceleratorTableEntries(wxVector<wxAcceleratorEntry>& Entries);

	// Assembles an accelerator key table for the frame by calling down to AddAcceleratorTableEntries.
	void							DoSetAcceleratorTable(void);


// Attributes
public:
	bool							AnythingChanged(void) const; 
	bool							HasChanged(void) const;
	void							ResetChanged(void); // sets m_bChanged to false

protected:
	bool							m_bChangedAny; // if true anything in given pSet was changed
	bool							m_bChanged; // set to true in sql3Update if something got chanced, false otherwise
	bool							m_bInitial; // set to true in OnInitDialog, otherwise allways false

	void							SetChanged(void); // sets m_bChanged and m_bChangedAny to true

// Operations
public:
	virtual bool					IsOk(void) const; // true if IsOpen && m_bValidDB==true

	// this func like MFC's CWnd::UpdateData -- validates and transfers data from controls to/from this recordset -- only makes sense if using wxValidators
	virtual bool					UpdateData(bool bSaveAndValidate = false);
	// if bSaveAndValidate==true, Validate and TransferDataFromWindow will be performed
	// if bSaveAndValidate==false (default) TransferDataToWindow will be performed

	virtual wxString				GetDefaultFilter(void) const; // default returns nothing, but you could set here a default filter -- wil be called in GetRecordset()
	virtual wxString				GetDefaultOrder(void) const; // default returns nothing, but you could set here a default order -- wil be called in GetRecordset()

	// needs to be overwritten, default returns FALSE
	virtual bool					CanAppend(void);
	virtual bool					CanRemove(void);

	// called in OnEscape, default does nothing
	virtual void					DoEscape(void);

	// needs to be overwritten, default does nothing
	virtual bool					DoDelete(void);
	virtual bool					DoUpdate(void);
	virtual bool					DoAdd(void);

	enum class MoveIDs
	{
		RecordFirst,
		RecordPrev,
		RecordNext,
		RecordLast,
		RecordINVALID
	};
	virtual bool					DoMove(MoveIDs nIDMoveCommand);

protected:
	// This will be called before actual pSet->Update() .. if you need to set hidden fields or so
	virtual void					DoThingsBeforeUpdate(void);

	// This will be called before actual DoAdd() .. e.g. to set focus to a specific control
	virtual void					DoThingsBeforeAdd(void);

	bool							ConfirmDelete(void);

	virtual void					UpdateRecordStatusbar(void);


protected:
	// called by OnUpdate... needs to be overwritten, default returns FALSE
	virtual bool					DoUpdateRecordFirst(void);
	virtual bool					DoUpdateRecordPrev(void);
	virtual bool					DoUpdateRecordNext(void);
	virtual bool					DoUpdateRecordLast(void);
	virtual bool					DoUpdateRecordNew(void);
	virtual bool					DoUpdateRecordRemove(void);
	virtual bool					DoUpdateRecordUpdate(void);

	virtual bool					DoUpdateRecordFind(void);
	virtual bool					DoUpdateRecordFindNext(void);
	virtual bool					DoUpdateRecordFindPrev(void);

	// show the help string for the given menu item using DoGiveHelp() if the
	// given item does have a help string (as determined by FindInMenu()),
	// return false if there is no help for such item
	bool							ShowMenuHelp(int helpid, wxMenu*);

	// Following are created, stored and destroyed (at destruction) in the internal lkRecordStatusbar class
	// they're created using wxWindow::NewControlID for uniqness and only valid in the current (and any derived) class
public:
	wxWindowID						GetID_RecFirst(void) const;
	wxWindowID						GetID_RecPrev(void) const;
	wxWindowID						GetID_RecNext(void) const;
	wxWindowID						GetID_RecLast(void) const;
	wxWindowID						GetID_RecNew(void) const;
	wxWindowID						GetID_RecRemove(void) const;
	wxWindowID						GetID_RecUpdate(void) const;
	wxWindowID						GetID_RecCancel(void) const;

public:
	void							SetCurrentRecord(wxUint64);
	void							SetTotalRecords(wxUint64);

// Event Handling
protected:
	void							OnUpdateRecordFirst(wxUpdateUIEvent& event);
	void							OnUpdateRecordPrev(wxUpdateUIEvent& event);
	void							OnUpdateRecordNext(wxUpdateUIEvent& event);
	void							OnUpdateRecordLast(wxUpdateUIEvent& event);
	void							OnUpdateRecordAdd(wxUpdateUIEvent& event);
	void							OnUpdateRecordDelete(wxUpdateUIEvent& event);
	void							OnUpdateRecordUpdate(wxUpdateUIEvent& event);

	void							OnUpdateFind(wxUpdateUIEvent& event);
	void							OnUpdateFindNext(wxUpdateUIEvent& event);
	void							OnUpdateFindPrev(wxUpdateUIEvent& event);

	void							OnInitDialog(wxInitDialogEvent& event);
	void							OnButton(wxCommandEvent& event);

	void							OnMove(wxCommandEvent& event);
	void							OnAdd(wxCommandEvent& event);
	void							OnUpdate(wxCommandEvent& event);
	void							OnDelete(wxCommandEvent& event);
	// responds to <esc> key-click, only available in wxwidgets through a wxAcceleratorTable. This command calls DoCancel()
	void							OnEscape(wxCommandEvent& event);
	// catches <Esc> key, not to be processed by wxDialog but instead triggers an OnEscape-event
	void							OnCharHook(wxKeyEvent& event);

	// responds to EVT_MENU_HIGHLIGHT, default calls FindItemInMenuBar(..) but whatif it's been used with popup menus? Allthough the event has the pointer to the calling (popup)menu ..
	void							OnMenuHighlight(wxMenuEvent& event);

	// responds to EVT_MENU_CLOSE so we can blank the Help-string (if any) as shown in our statusbar
	void							OnMenuClose(wxMenuEvent& event);

private:
	wxDECLARE_CLASS(lkRecordDlg);
	wxDECLARE_NO_COPY_CLASS(lkRecordDlg);
};
#endif // !__LK_RECORDDLG_H__
