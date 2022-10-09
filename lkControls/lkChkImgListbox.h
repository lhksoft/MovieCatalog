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
#ifndef __LK_CHKIMGLISTBOX_H__
#define __LK_CHKIMGLISTBOX_H__

#pragma once

#include <wx/window.h>
#include <wx/vlbox.h>

#include "lkImageBox.h"
#include "lkItemContainer.h"

extern const char	lkCheckImageListboxNameStr[];

class lkChkImgListbox : public wxVListBox, public lkImageBox,  private lkItemContainer
{
public:
	lkChkImgListbox();
	lkChkImgListbox(wxWindow* pParent, wxWindowID nId = wxID_ANY, const wxPoint& nPos = wxDefaultPosition, const wxSize& nSize = wxDefaultSize,
					long nStyle = 0, const wxValidator& validator = wxDefaultValidator);

    // One needs to initialize this list using lkImageBox::InitializeFromList,
    // So after C'tor, call InitializeFromList, then call Create
    bool Create(wxWindow* pParent, wxWindowID nId = wxID_ANY, const wxSize& nSize = wxDefaultSize,
				long nStyle = 0, const wxPoint& nPos = wxDefaultPosition, const wxValidator& validator = wxDefaultValidator);

    virtual ~lkChkImgListbox();

	// required virtuals
public:
	virtual bool				IsSorted(void) const;
    virtual bool				SetFont(const wxFont& font) wxOVERRIDE;

protected:
    virtual void				UpdateInternals(void) wxOVERRIDE;

	virtual int					AddItem(lkBoxInit*, wxBitmap*) wxOVERRIDE;
    virtual wxClientData*       GetItemObject(unsigned int) const wxOVERRIDE;
	virtual wxWindow*			GetControl(void) wxOVERRIDE;
    virtual void				StartOver(void) wxOVERRIDE;

	virtual void				OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const wxOVERRIDE;
	virtual void                OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const wxOVERRIDE;
    virtual wxCoord             OnMeasureItem(size_t n) const wxOVERRIDE;

    virtual wxSize              DoGetBestClientSize() const wxOVERRIDE;

public:
    // Item management
    bool                        IsEmpty(void) const;
    void                        Insert(const wxString& item, int pos);
    int                         Append(const wxString& item);
    void                        Clear(void);
    void                        Delete(unsigned int item);
    void                        SetItemClientData(unsigned int n, const void* clientData);
    void                        SetItemClientObject(unsigned int n, wxClientData*);
    void*                       GetItemClientData(unsigned int n) const;
    wxClientData*               GetItemClientObject(unsigned int n) const;
    void                        SetString(int item, const wxString& str);
    wxString                    GetString(int item) const;
    unsigned int                GetCount(void) const;
    int                         FindString(const wxString& s, bool bCase = false) const;

    bool                        IsChecked(int) const;
    void                        SetCheck(int, bool);
    void                        ToggleCheck(int);

    void                        ActAsCheckListbox(bool); // if entered 'false', no checkboxes are used

    int                         GetBestHeight(void) const;

    wxUint64					GetLParam(unsigned int) const;
    void						SetLParam(unsigned int, wxUint64);

	size_t						FindLParam(wxUint64); // if not found will return wxNOT_FOUND
	void						ScrollIntoView(size_t); // Selects given item and Scrolls it into view

    // Attributes
private:
	void                        Init(void);

protected:
    bool                        m_actAsCheckListbox; // if 'false' will act as normal Bitmap/Text Listbox

    wxSize                      m_chkSize; // w/h of checkbox

    // pressing space or clicking the check box toggles the item
    void                        OnKeyDown(wxKeyEvent& event);
    void                        OnLeftClick(wxMouseEvent& event);
	void					    OnRightClick(wxMouseEvent& event);

    // send an "item checked" event
    void                        SendEvent(unsigned int);

private:
    wxDECLARE_EVENT_TABLE();

	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkChkImgListbox);
};

#endif // !__LK_CHKIMGLISTBOX_H__
