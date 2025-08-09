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
#include "lkChkImgListbox.h"
#include "lkClientData.h"
#include "lkColour.h"

#include <wx/renderer.h>
#include <wx/dc.h>
#include <wx/pen.h>
#include <wx/settings.h>
#include <wx/msgdlg.h>

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// //////////////////////////////////////////////////////////////////////////////////////
// C'tor / D'tor / Initialisation
// //////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(lkChkImgListbox, wxVListBox)

// define event table
// ------------------
wxBEGIN_EVENT_TABLE(lkChkImgListbox, wxVListBox)
    EVT_KEY_DOWN(lkChkImgListbox::OnKeyDown)
    EVT_LEFT_DOWN(lkChkImgListbox::OnLeftClick)
	EVT_RIGHT_DOWN(lkChkImgListbox::OnRightClick)
wxEND_EVENT_TABLE()


const char	lkCheckImageListboxNameStr[] = "lkChkImgListbox";

lkChkImgListbox::lkChkImgListbox() : wxVListBox(), lkImageBox(), lkItemContainer(),
    m_chkSize()
{
    m_actAsCheckListbox = true;
	lkChkImgListbox::Init();
}
lkChkImgListbox::lkChkImgListbox(wxWindow* pParent, wxWindowID nId, const wxPoint& nPos, const wxSize& nSize, long nStyle, const wxValidator& validator)
	: wxVListBox(), lkImageBox(), lkItemContainer(), m_chkSize()
{
	m_actAsCheckListbox = true;
	lkChkImgListbox::Init();
	(void)Create(pParent, nId, nSize, nStyle, nPos, validator);
}

lkChkImgListbox::~lkChkImgListbox()
{
}

bool lkChkImgListbox::Create(wxWindow* pParent, wxWindowID nId, const wxSize& nSize, long nStyle, const wxPoint& nPos, const wxValidator& validator)
{
    if ( nStyle & wxLB_MULTIPLE )
        nStyle = nStyle & ~wxLB_MULTIPLE;
    if ( nStyle & wxLB_EXTENDED )
        nStyle = nStyle & ~wxLB_EXTENDED;
    nStyle |= wxLB_SINGLE;
    nStyle |= wxLB_OWNERDRAW;
	nStyle |= wxLB_ALWAYS_SB;

    if ( !wxVListBox::Create(pParent, nId, nPos, nSize, nStyle, lkCheckImageListboxNameStr) )
        return false;
	SetValidator(validator);

    UpdateInternals();
    return true;
}


void lkChkImgListbox::Init()
{
    m_chkSize = wxRendererNative::Get().GetCheckBoxSize(this);
}


// //////////////////////////////////////////////////////////////////////////////////////
// Overrides
// //////////////////////////////////////////////////////////////////////////////////////

//virtual
bool lkChkImgListbox::IsSorted() const
{
    return HasFlag(wxLB_SORT);
}

//virtual
bool lkChkImgListbox::SetFont(const wxFont& font)
{
    bool res = wxVListBox::SetFont(font);
    UpdateInternals();
    return res;
}

//virtual
void lkChkImgListbox::UpdateInternals()
{
    lkImageBox::UpdateInternals();

    if ( m_actAsCheckListbox )
    {
        int h = m_chkSize.GetHeight();
        m_nBestHeight = wxMax(m_nBestHeight, h);
    }
}

//virtual
int lkChkImgListbox::AddItem(lkBoxInit* pInit, wxBitmap* pBmp)
{
	wxASSERT(pInit != NULL);
	if ( pInit == NULL ) return wxNOT_FOUND;

	wxString s = pInit->GetString();
	int nIndex = Append(s);

	if ( nIndex >= 0 )
	{
		lkImgClientData* clientData = new lkImgClientData(pBmp, pInit->GetParam());
        SetItemClientObject(nIndex, (wxClientData*)clientData);
	}

	return nIndex;
}

//virtual
wxClientData* lkChkImgListbox::GetItemObject(unsigned int item) const
{
    return GetItemClientObject(item);
}

//virtual
wxWindow* lkChkImgListbox::GetControl()
{
	return this;
}

//virtual
void lkChkImgListbox::StartOver()
{
    lkImageBox::StartOver();
    // Should only be called, if already created
    if ( GetHandle() != NULL )
        Clear();
}

//virtual
void lkChkImgListbox::OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const
{
	// based on wxVListBox::OnDrawBackground but some parts of the wx-sources are not virtual or even private !
	int flags = 0;
	if ( !IsEnabled() )
		flags |= wxCONTROL_DISABLED;
	if ( IsSelected(n) )
		flags |= wxCONTROL_SELECTED;
	if ( IsCurrent(n) )
		flags |= wxCONTROL_CURRENT;
	if ( wxWindow::FindFocus() == const_cast<lkChkImgListbox*>(this) )
		flags |= wxCONTROL_FOCUSED;

	wxBrush brush;
	wxPen pen;
	lkColour back, fore;

	if ( flags & wxCONTROL_SELECTED )
	{
		// we need to render selected and current items differently
		back = (GetSelectionBackground().IsOk()) ? GetSelectionBackground() : wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
		if ( flags & wxCONTROL_FOCUSED )
			fore = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
		else
		{
			back.SetLighter(60);
			fore = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
		}
		brush = wxBrush(back, wxBRUSHSTYLE_SOLID);
		pen = wxPen(back);
	}
	else // !selected
	{
		brush = *wxTRANSPARENT_BRUSH;
		pen = *wxTRANSPARENT_PEN;
		fore = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
	}
	if ( flags & wxCONTROL_DISABLED )
	{
		if (back.IsOk() )
			back = back.MakeDisabled();
		if (fore.IsOk() )
			fore = fore.MakeDisabled();
	}
	dc.SetBrush(brush);
	dc.SetTextForeground(fore);
	dc.SetPen(pen);
	dc.DrawRectangle(rect);

/*
	if ( GetSelectionBackground().IsOk() )
	{
		// we need to render selected and current items differently
//		const bool isSelected = IsSelected(n), isCurrent = IsCurrent(n);
		if ( flags & wxCONTROL_SELECTED )
		{
			if ( flags & wxCONTROL_FOCUSED )
			{
				brush = wxBrush(GetSelectionBackground(), wxBRUSHSTYLE_SOLID);
				dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
			}
			else // !focused
			{
				if ( GetSelectedNoFocusColour().IsOk() )
					brush = wxBrush(GetSelectedNoFocusColour(), wxBRUSHSTYLE_SOLID);
				else
					brush = *wxTRANSPARENT_BRUSH;
				dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
			}
//			dc.SetPen(*(isCurrent ? wxBLACK_PEN : wxTRANSPARENT_PEN));
		}
		else
		{
			brush = *wxTRANSPARENT_BRUSH;
			dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
		}
		dc.SetBrush(brush);
		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.DrawRectangle(rect);
	}
	else
	{
		if ( flags & wxCONTROL_SELECTED )
		{
			if ( flags & wxCONTROL_FOCUSED )
			{
				brush = wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
				dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
			}
			else // !focused
			{
				brush = wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
				dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
			}
		}
		else // !selected
		{
			brush = *wxTRANSPARENT_BRUSH;
			dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
		}

		dc.SetBrush(brush);
		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.DrawRectangle(rect);
	}
*/
}

//virtual
void lkChkImgListbox::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const
{
    wxRect _rect = rect;
    if ( m_actAsCheckListbox )
    {
        // Draw Checkbox
        wxSize size = m_chkSize;

        int flags = wxCONTROL_FLAT;
        if ( IsChecked(n) )
            flags |= wxCONTROL_CHECKED;

        wxRect rc = _rect; // space available for drawing into
        rc.x += lkCX_BORDER; // where to start drawing
        rc.y += lkCY_BORDER + (rect.GetHeight() - size.GetHeight()) / 2;
        rc.SetSize(size);

        wxRendererNative::Get().DrawCheckBox((wxWindow*)this, dc, rc, flags);
        _rect.x += size.GetWidth() + lkCX_BORDER * 2;
    }

    // do draw the rest
    lkImageBox::DrawItem(dc, _rect, n, GetString(n), 0);
}

//virtual
wxCoord lkChkImgListbox::OnMeasureItem(size_t WXUNUSED(n)) const
{
    if ( m_nBestHeight > 0 )
        return m_nBestHeight;

    // else
    return 0;
}

//virtual
wxSize lkChkImgListbox::DoGetBestClientSize() const
{
  if ( m_nBestHeight == 0 )
      ((lkChkImgListbox*)this)->UpdateInternals();

    wxSize client = GetClientSize();

    int y = client.GetY() / lkImageBox::GetBestHeight();

    int yy = wxMax(y, m_nImageCount);
    if ( yy < 2 ) yy = 2;
    if ( yy > 4 ) yy = 4;

    y = yy * lkImageBox::GetBestHeight();

    wxSize sz = wxSize(client.GetX(), y);
    return sz;
}


// Item management
// ---------------

bool lkChkImgListbox::IsEmpty() const
{
    return lkItemContainer::IsEmpty();
}

void lkChkImgListbox::Insert(const wxString& item, int pos)
{
    unsigned int nPos = lkItemContainer::InsertItem((unsigned int)pos, item, NULL, wxClientDataType::wxClientData_None);
    wxASSERT((int)nPos == pos);

    wxVListBox::SetRowCount(lkItemContainer::GetCount());
}

int lkChkImgListbox::Append(const wxString& item)
{
//  int pos = lkItemContainer::GetCount();
    unsigned int nPos = lkItemContainer::AppendItem(item, NULL, wxClientDataType::wxClientData_None);

	unsigned int nCount = lkItemContainer::GetCount();
    wxVListBox::SetRowCount(nCount);
	wxVListBox::ScrollToRow(nCount-1);

    return nPos;
}

void lkChkImgListbox::Clear()
{
    lkItemContainer::Clear();

    wxVListBox::SetRowCount(0);
}

void lkChkImgListbox::Delete(unsigned int item)
{
    lkItemContainer::RemoveAt(item);

    int sel = GetSelection();

    wxVListBox::SetItemCount(lkItemContainer::GetCount());

    // Fix selection
    if ( (int)item < sel )
        SetSelection(sel - 1);
    else if ( (int)item == sel )
        SetSelection(wxNOT_FOUND);
}

void lkChkImgListbox::SetItemClientData(unsigned int n, const void* clientData)
{
    lkItemContainer::SetClientData(n, clientData);
}

void lkChkImgListbox::SetItemClientObject(unsigned int n, wxClientData* pO)
{
    lkItemContainer::SetClientObject(n, pO);
}

void* lkChkImgListbox::GetItemClientData(unsigned int n) const
{
    return lkItemContainer::GetClientData(n);
}

wxClientData* lkChkImgListbox::GetItemClientObject(unsigned int n) const
{
    return lkItemContainer::GetClientObject(n);
}

void lkChkImgListbox::SetString(int item, const wxString& str)
{
    if ( item < 0 )
        return;

    lkItemContainer::SetString((unsigned int)item, str);
}

wxString lkChkImgListbox::GetString(int item) const
{
    return (item >= 0) ? lkItemContainer::GetString((unsigned int)item) : wxString();
}

unsigned int lkChkImgListbox::GetCount() const
{
    return lkItemContainer::GetCount();
}

int lkChkImgListbox::FindString(const wxString& s, bool bCase) const
{
    return lkItemContainer::FindString(s, bCase);
}

bool lkChkImgListbox::IsChecked(int n) const
{
    bool ret = (m_actAsCheckListbox && n >= 0) ? lkItemContainer::IsChecked((unsigned int)n) : false;
    return ret;
}
void lkChkImgListbox::SetCheck(int n, bool c)
{
    if ( !m_actAsCheckListbox )
        return;
    // Only makes sense if we have checkboxes
    if ( n >= 0 )
        lkItemContainer::SetCheck((unsigned int)n, c);
    else
    {
        // operate at all
        for ( unsigned int i = 0; i < GetCount(); i++ )
            lkItemContainer::SetCheck(i, c);
    }
}
void lkChkImgListbox::ToggleCheck(int n)
{
    if ( !m_actAsCheckListbox || n <= wxNOT_FOUND )
        return;

    bool cur = IsChecked(n);
    cur = !cur;
    SetCheck(n, cur);
}

void lkChkImgListbox::ActAsCheckListbox(bool c)
{ // if entered 'false', no checkboxes are used
    bool prev = m_actAsCheckListbox;
    m_actAsCheckListbox = c;
    if ( GetHandle() && (m_actAsCheckListbox != prev) )
        Refresh();
}

int lkChkImgListbox::GetBestHeight() const
{
    return lkImageBox::GetBestHeight();
}

wxUint64 lkChkImgListbox::GetLParam(unsigned int n) const
{
    wxClientData* cont;
    if ( (cont = lkItemContainer::GetClientObject(n)) != NULL )
    {
        lkImgClientData* pData = dynamic_cast<lkImgClientData*>(cont);
        return pData->GetLPARAM();
    }
    return 0;
}
void lkChkImgListbox::SetLParam(unsigned int n, wxUint64 l)
{
    wxClientData* cont;
    if ( (cont = lkItemContainer::GetClientObject(n)) != NULL )
    {
        lkImgClientData* pData = dynamic_cast<lkImgClientData*>(cont);
        pData->SetLPARAM(l);
    }
}

size_t lkChkImgListbox::FindLParam(wxUint64 lParam)
{
	unsigned int nCount = GetCount();

	if ( IsEmpty() || (lParam == wxNOT_FOUND))
		return wxNOT_FOUND;

	// try at the beginning of the list
	if ( GetLParam(0) == lParam )
		return 0;
	else if ( GetLParam(nCount - 1) == lParam ) // or at the end of the list
		return static_cast<size_t>(nCount - 1);
	else
	{
		 // then search throug the complete list
		for ( unsigned int i = 0; i < nCount; i++ )
			if ( GetLParam(i) == lParam )
				return static_cast<size_t>(i);
	}

	// not found..
	return wxNOT_FOUND;
}

void lkChkImgListbox::ScrollIntoView(size_t n)
{
	unsigned int nCount = GetCount();

	if ( IsEmpty() )
		n = -1;
	else if ( n >= nCount )
		n = nCount - 1;

	int c = GetSelection();
	int x = 0;
	if ( c != wxNOT_FOUND )
	{
		if ( n > c )
			x = n - c;
		else if ( n < c )
			x = (c - n) * -1;
		else
			x = 0;
	}
	SetSelection(n);

	size_t f1 = GetVisibleRowsBegin();
	size_t e1 = GetVisibleRowsEnd();
	if ( c != wxNOT_FOUND )
	{
		int h;
		GetClientSize(NULL, &h);
		wxCoord cc = GetUnitsSize(0, 1);
		int w = h / cc; // amount of rows visible at 1 page

		int curpage = (c - (c % w)) / w;
		int newpage = (n - (n % w)) / w;

		if ( curpage != newpage )
			ScrollRows(x);
	}
	else
		ScrollToRow(n);
}

// process events
// --------------

void lkChkImgListbox::OnKeyDown(wxKeyEvent& event)
{
    if ( !m_actAsCheckListbox )
    {
        event.Skip();
        return;
    }

    // what do we do?
    enum class iner
    {
        NONE,
        TOGGLE,
        SET,
        CLEAR
    } oper;

    switch ( event.GetKeyCode() )
    {
        case WXK_SPACE:
            oper = iner::TOGGLE;
            break;

        case WXK_NUMPAD_ADD:
        case '+':
            oper = iner::SET;
            break;

        case WXK_NUMPAD_SUBTRACT:
        case '-':
            oper = iner::CLEAR;
            break;

        default:
            oper = iner::NONE;
    }

    if ( oper != iner::NONE )
    {
        wxArrayInt selections;
        int count = 0;
/*
        if ( HasMultipleSelection() )
        {
            count = GetSelectedCount();
        }
        else
*/
        {
            int sel = GetSelection();
            if ( sel != -1 )
            {
                count = 1;
                selections.Add(sel);
            }
        }

        for ( int i = 0; i < count; i++ )
        {
            int nItem = selections[i];
            wxRect rect = GetItemRect(nItem);
            switch ( oper )
            {
                case iner::TOGGLE:
                    ToggleCheck(nItem);
                    RefreshRect(rect);
                    break;

                case iner::SET:
                case iner::CLEAR:
                    SetCheck(nItem, oper == iner::SET);
                    RefreshRect(rect);
                    break;

                default:
                    wxFAIL_MSG(wxT("what should this key do?"));
            }

            // we should send an event as this has been done by the user and
            // not by the program
            SendEvent(nItem);
        }
    }
    else // nothing to do
    {
        event.Skip();
    }
}

void lkChkImgListbox::OnLeftClick(wxMouseEvent& event)
{
	if ( IsEmpty() )
	{
		event.Skip();
		return;
	}

    wxPoint point = wxPoint(event.GetX(), event.GetY());
    // assume did not hit anything
    int nItem = -1;
    int cyItem = m_nBestHeight;
    int top = GetVisibleRowsBegin();
    nItem = top + point.y / cyItem;

	if ( m_actAsCheckListbox )
	{
		wxRect rect = GetItemRect(nItem);
		// convert item rect to check mark rect
		wxSize size = wxRendererNative::Get().GetCheckBoxSize(this);
		rect.x += lkCX_BORDER;
		rect.y += lkCY_BORDER + (rect.GetHeight() - size.GetHeight()) / 2;
		rect.SetSize(size);
		if ( rect.Contains(point) )
		{
			// people expect to get "kill focus" event for the currently
			// focused control before getting events from the other controls
			// and, equally importantly, they may prevent the focus change from
			// taking place at all (e.g. because the old control contents is
			// invalid and needs to be corrected) in which case we shouldn't
			// generate this event at all
			SetFocus();
			if ( FindFocus() == this )
			{
				ToggleCheck(nItem);
				SendEvent(nItem);

				// scroll one item down if the item is the last one
				// and isn't visible at all
				int h;
				GetClientSize(NULL, &h);
				if ( rect.GetBottom() > h )
					ScrollLines(1);

				rect = GetItemRect(nItem);
				RefreshRect(rect);
			}
		}
		else
		{
			if ( nItem >= 0 )
			{
				if (HasMultipleSelection() )
					Toggle((size_t)nItem);
				else
				{
					if ( IsSelected((size_t)nItem) )
						SetSelection(-1);
					else
						SetSelection(nItem);
				}
			}
		}
	}
    else
    {
		if ( nItem >= 0 )
		{
			if ( HasMultipleSelection() )
				Toggle((size_t)nItem);
			else
			{
				if ( IsSelected((size_t)nItem) )
					SetSelection(-1);
				else
					event.Skip(); // SetSelection(nItem);
			}
		}
    }
}

// send an "item checked" event
void lkChkImgListbox::SendEvent(unsigned int uiIndex)
{
    if ( !m_actAsCheckListbox )
        return;

    wxCommandEvent event(wxEVT_CHECKLISTBOX, GetId());
    event.SetInt(uiIndex);
    event.SetEventObject(this);
    event.SetString(GetString(uiIndex));
    HandleWindowEvent(event);
}

void lkChkImgListbox::OnRightClick(wxMouseEvent& event)
{
#ifdef __WXDEBUG__
	wxWindowDisabler wd(true);

	wxSize sizeC = GetClientSize();
	wxSize sizeF = GetParent()->GetSize();
	wxString s; s.Printf(wxT("Frame Size\nwidth = %d | height = %d\n\nClient Size\nwidth = %d | height = %d"), sizeF.GetWidth(), sizeF.GetHeight(), sizeC.GetWidth(), sizeC.GetHeight());
	wxMessageBox(s, wxT("For your information"), wxOK | wxICON_INFORMATION, this);
#else
	event.Skip();
#endif // WXDEBUG
}

