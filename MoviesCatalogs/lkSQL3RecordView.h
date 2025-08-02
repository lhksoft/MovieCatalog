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
#ifndef __LK_SQL3RECORDVIEW_H__
#define __LK_SQL3RECORDVIEW_H__
#pragma once

#include <wx/docview.h>
#include <wx/panel.h>
#include <wx/toolbar.h>
#include <wx/event.h>

#include "../lkSQLite3/lkSQL3RecordSet.h"
#include "../lkControls/lkDocument.h"
#include "../lkControls/lkRecordChildFrame.h"
#include "../lkControls/lkCanvas.h"
#include "../lkControls/lkView.h"

class lkSQL3MoveEvent;

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class lkSQL3RecordDocument
// --------------------------
// Any child documents in this program should be derived from this document-class
// this document integrates sql3Recordset capabilities
// the document is a lkDocument and also a lkSQL3Recordset
class lkSQL3RecordView;
class lkSQL3RecordDocument : public lkDocument
{
public:
	lkSQL3RecordDocument(wxDocument* pParent);
	virtual ~lkSQL3RecordDocument();

// Attributes
public:
	// Gets a pointer to the real DB in the parent-doc
	lkSQL3Database*					GetDB(void);
	lkSQL3RecordSet*				GetRecordset(void);

	bool							HasChanged(void) const;
	void							ResetChanged(void); // sets m_bChanged to false

protected:
	// used internally
	virtual wxWindow*				GetFrame(void);

public:
	virtual void					BindButtons(void);
	void							SetCanRemove(void); // called from inside UpdateData, also might be called inside lkSQL3RecordView::InitialUpdate

protected:
	lkSQL3RecordSet*				m_pSet; // base RecordSet, opened in derived class via GetRecordSet
	bool							m_bChanged; // set to true in sql3Update if something got chanced, false otherwise
	bool							m_bCanRemove; // will be set in lkSQL3RecordDocument::UpdateData(false) or in derived classes, default will be FALSE (safety)

	// As derived child-docs also are derived from a custum TableSet, this should return 'this' in derived classes
	virtual lkSQL3RecordSet*		GetBaseSet(void) = 0; 

	lkSQL3RecordView*				sql3GetView(void);
	virtual bool					UpdateData(bool bSaveAndValidate = false);

// Operations
public:
	bool							IsOk(void) const; // true if IsOpen && m_bValidDB==true

	virtual wxString				GetDefaultFilter(void) const; // default returns nothing, but you could set here a default filter -- wil be called in GetRecordset()
	virtual wxString				GetDefaultOrder(void) const; // default returns nothing, but you could set here a default order -- wil be called in GetRecordset()

	bool							CanAppend(void); // will get it from the recordset
	bool							CanRemove(void); // will get it from the recordset

	bool							sql3Delete(void);
	bool							sql3Update(void);
	bool							sql3Add(void);
	void							sql3Cancel(void); // when cancel pressed in Frame, the frame then will call this function

	enum MoveIDs
	{
		RecordFirst,
		RecordPrev,
		RecordNext,
		RecordLast,
		RecordINVALID
	};
	bool							sql3Move(MoveIDs nIDMoveCommand);

protected:
	// This will be called before actual pSet->Update() .. if you need to set hidden fields or so
	virtual void					DoThingsBeforeUpdate(void);

	// This will be called before actual sqlAdd() .. e.g. to set focus to a specific control
	virtual void					DoThingsBeforeAdd(void);

	bool							ConfirmDelete(void);

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

	void							OnMove(wxCommandEvent& event);
	void							OnAdd(wxCommandEvent& event);
	void							OnUpdate(wxCommandEvent& event);
	void							OnDelete(wxCommandEvent& event);

	void							OnSearchNext(wxCommandEvent& event);
	void							OnSearchPrev(wxCommandEvent& event);

public:
	// this one send by the Preview-Listbox in derived classes : dbl-click will move to the selected record
	void							OnSQL3Move(lkSQL3MoveEvent&);

private:
	wxDECLARE_EVENT_TABLE();
	wxDECLARE_ABSTRACT_CLASS(lkSQL3RecordDocument);
	wxDECLARE_NO_COPY_CLASS(lkSQL3RecordDocument);
};


// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class lkSQL3panel
// -----------------
// any Panels associated with lkSQL3RecordView should be derived from this panel class
// what it does :
// it catches any InitDialog Messages, which by default Transfers data to the window,
// but this Transfer is performed by lkSQL3RecordView

class lkSQL3panel : public lkCanvas
{
public:
	lkSQL3panel(wxView* view, wxWindow* parent = NULL);

	lkSQL3RecordDocument*	GetDocument(void);
	lkSQL3RecordView*		GetView(void);

protected:
	void					OnRightClick(wxMouseEvent& event);

	wxDECLARE_EVENT_TABLE();

private:
	wxDECLARE_CLASS(lkSQL3panel);
	wxDECLARE_NO_COPY_CLASS(lkSQL3panel);
};


class lkSQL3RecordView : public lkView
{
public:
	lkSQL3RecordView();
	virtual ~lkSQL3RecordView();

// Attributes
protected:
	lkSQL3Database*					GetDB(void); // get it from the document

protected:
	lkSQL3panel*					m_pCanvas;
	bool							m_bClosing; // set to true if user Cancels adding new record in empty recordset so no 2nd Validation will be performed, otherwise allways FALSE
	bool							m_bInitial; // true in InitialUpdate, otherwise allways false

	// Required for setting config-options
	virtual wxString				GetConfigPath(void) const = 0;

public:
	void							SetCurrentRecord(wxUint64);
	void							SetTotalRecords(wxUint64);

	void							SetClosing(bool);
	bool							IsInitial(void) const;

	lkSQL3panel*					GetCanvas(void);

// Implementation
public:
	// Initial Update : initialize controls with settings at first show (like MFC's CView::OnInitialUpdate)
	virtual void					InitialUpdate(void) wxOVERRIDE;

	// this func like MFC's CWnd::UpdateData -- validates and transfers data from controls to/from this recordset -- only makes sense if using wxValidators
	virtual bool					UpdateData(bool bSaveAndValidate = false) wxOVERRIDE;
	// if bSaveAndValidate==true, Validate and TransferDataFromWindow will be performed
	// if bSaveAndValidate==false (default) TransferDataToWindow will be performed

protected:
//	virtual void					DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void					ProcessRecordset(lkSQL3RecordSet*); // called when ( pHint->IsKindOf(RUNTIME_CLASS(CSQL3RecordSet)) )

	void							UpdateRecordStatusbar(void);

	// called in 'OnClose' -- will store frame's current position in cofig-file
	void							SetConfigPosition(void);
	// called in 'OnCreate' from derived Views to get stored position in config-file, on error/not stored, returns wxDefaultPosition
	wxPoint							GetConfigPosition(void) const;

// Overrides
public:
	virtual void					OnUpdate(wxView* sender, wxObject* hint = 0) wxOVERRIDE;
	virtual bool					OnClose(bool deleteWindow) wxOVERRIDE;

// Messages
/* ********************************************************
 * As the IDs for Record-Navigation are defined in the Main-App, you should use following functions to bind them
 * use EVT_UPDATE_UI(id, func) for the 'OnUpdateRecord...' functions
 * - or -
 * EVT_BUTTON(id, func),
 * EVT_MENU(id, func) : if the id's are in a menu -- also accepted is EVT_MENU_RANGE(id1, id2, func)
 * or
 * EVT_TOOL(id, func) : if the id's are in a toolbar -- also accepted is EVT_TOOL_RANGE(id1, id2, func)
 * for the OnRecordMove function
*/

private:
	wxDECLARE_ABSTRACT_CLASS(lkSQL3RecordView);
	wxDECLARE_NO_COPY_CLASS(lkSQL3RecordView);
};


class lkSQL3RecordChildFrame : public lkRecordChildFrame
{
public:
	lkSQL3RecordChildFrame();

protected:
	// called in OnCancel, default does nothing
	virtual void			DoCancel(void) wxOVERRIDE;

private:
	wxDECLARE_CLASS(lkSQL3RecordChildFrame);
	wxDECLARE_NO_COPY_CLASS(lkSQL3RecordChildFrame);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class lkSQL3MoveEvent
////
wxDECLARE_EVENT(lkEVT_SQL3MOVE, lkSQL3MoveEvent);

// event table entry
#define EVT_SQL3MOVE(id, func) \
 	wx__DECLARE_EVT1(lkEVT_SQL3MOVE, id, lkSQL3MoveEventHandler(func))

class lkSQL3MoveEvent : public wxCommandEvent
{
public:
	lkSQL3MoveEvent(int id = wxID_ANY);
	lkSQL3MoveEvent(const lkSQL3MoveEvent&);

	virtual wxEvent* Clone() const wxOVERRIDE;

	wxUint64				GetRow(void) const;
	void					SetRow(wxUint64);

private:
	wxUint64				m_nRow;

	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3MoveEvent);
};

typedef void (wxEvtHandler::* lkSQL3MoveEventFunction)(lkSQL3MoveEvent&);

#define lkSQL3MoveEventHandler(func) wxEVENT_HANDLER_CAST(lkSQL3MoveEventFunction, func)                    

#endif // !__LK_SQL3RECORDVIEW_H__
