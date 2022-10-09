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
#ifndef __LK_RECORD_BAR_H__
#define __LK_RECORD_BAR_H__
#pragma once

#include <wx/toolbar.h>
#include <wx/statusbr.h>
#include <wx/stattext.h>

/****************************************************************************************
 * lkRecordStatusbar -- Class definition
 * A custom status bar which contains controls for record navigation
 ****************************************************************************************/
class lkRecordStatusbar : public wxStatusBar
{
public:
	lkRecordStatusbar();
	virtual ~lkRecordStatusbar();

	bool					Create(wxWindow* pParent, int number = 1, long style = wxSTB_DEFAULT_STYLE, wxWindowID id = wxID_ANY);

public:
	virtual void			SetStatusText(const wxString& text, int i = 0);

	void					SetCurrentRecord(wxUint64);
	void					SetTotalRecords(wxUint64);

private:
	wxStaticText* m_recCurrent, * m_recTotal;

protected:
	wxWindowID				mID_RecFirst, mID_RecPrev, mID_RecNext, mID_RecLast,
							mID_RecNew, mID_RecRemove, mID_RecUpdate, mID_RecCancel;

	virtual void			Init_IDs(void);
	virtual int*			GetWidths(int number);

public:
	wxWindowID				GetID_RecFirst(void) const;
	wxWindowID				GetID_RecPrev(void) const;
	wxWindowID				GetID_RecNext(void) const;
	wxWindowID				GetID_RecLast(void) const;
	wxWindowID				GetID_RecNew(void) const;
	wxWindowID				GetID_RecRemove(void) const;
	wxWindowID				GetID_RecUpdate(void) const;
	wxWindowID				GetID_RecCancel(void) const;


protected:
	void					OnRecordButton(wxCommandEvent& event);

private:
	wxDECLARE_ABSTRACT_CLASS(lkRecordStatusbar);
	wxDECLARE_NO_COPY_CLASS(lkRecordStatusbar);
};


/****************************************************************************************
 * lkRecordToolbar -- Class definition
 * A custom tool bar which contains controls for record navigation
 ****************************************************************************************/
class lkRecordToolbar : public wxToolBar
{
public:
	lkRecordToolbar();
	lkRecordToolbar(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	virtual ~lkRecordToolbar();

	void				AddFirst(wxWindowID);
	void				AddPrev(wxWindowID);
	void				AddNext(wxWindowID);
	void				AddLast(wxWindowID);
	void				AddNew(wxWindowID);
	void				AddRem(wxWindowID);
	void				AddUpd(wxWindowID);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkRecordToolbar);
};

#endif // !__LK_RECORD_BAR_H__
