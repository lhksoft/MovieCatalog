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
#include "lkImgComboBox.h"
#include "lkClientData.h"

#include <wx/msgdlg.h> // used by our Validator
#include <wx/renderer.h>

//static
const char* lkImgComboBox::lkImageComboBoxNameStr = "lkImgComboBox";

#if !defined(__WXMSW__)

// //////////////////////////////////////////////////////////////////////////////////////
// C'tor / D'tor / Initialisation
// //////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(lkImgComboBox, wxOwnerDrawnComboBox);


wxBEGIN_EVENT_TABLE(lkImgComboBox, wxOwnerDrawnComboBox)
	EVT_SIZE(lkImgComboBox::OnSize)
wxEND_EVENT_TABLE()


lkImgComboBox::lkImgComboBox() : wxOwnerDrawnComboBox(), lkImageBox()
{
	Init();
}
lkImgComboBox::lkImgComboBox(wxWindow* parent, wxWindowID id, const wxSize& size, long style, const wxValidator& validator, const wxPoint& pos) : wxOwnerDrawnComboBox(), lkImageBox()
{
	Init();
	(void)Create(parent, id, size, style, validator, pos);
}
lkImgComboBox::~lkImgComboBox()
{
	wxOwnerDrawnComboBox::DoClear();
}

void lkImgComboBox::Init()
{
}

bool lkImgComboBox::Create(wxWindow* parent, wxWindowID id, const wxSize& size, long style, const wxValidator& validator, const wxPoint& pos)
{
	// with images, only ReadOnly allowed
	if ( !(style & wxCB_READONLY) )
		style |= wxCB_READONLY;

	if ( !wxOwnerDrawnComboBox::Create(parent, id, wxEmptyString, pos, size, style, validator, wxASCII_STR(lkImageComboBoxNameStr)) )
		return false;

	StartOver();
	UpdateInternals();

	return true;
}


wxUint64 lkImgComboBox::GetLParam(unsigned int n) const
{
	wxClientData* cont;
	if ( (cont = GetItemObject(n)) != NULL )
	{
		lkImgClientData* pData = dynamic_cast<lkImgClientData*>(cont);
		return pData->GetLPARAM();
	}
	return 0;
}
void lkImgComboBox::SetLParam(unsigned int n, wxUint64 lParam)
{
	wxClientData* cont;
	if ( (cont = GetItemObject(n)) != NULL )
	{
		lkImgClientData* pData = dynamic_cast<lkImgClientData*>(cont);
		pData->SetLPARAM(lParam);
	}
}

//virtual
void lkImgComboBox::SetSelectedLParam(wxUint64 u)
{ // Searches the list for the lparam, if found will select that item
	if ( IsListEmpty() )
		return;

	unsigned int i, c = GetCount();
	for ( i = 0; i < c; i++ )
		if ( GetLParam(i) == u )
		{
			SetSelection(i);
			return;
		}

	SetSelection(wxNOT_FOUND);
}

//virtual
wxUint64 lkImgComboBox::GetSelectedLParam() const
{ // From the selected item, the lparam will be returned
	if ( IsListEmpty() )
		return 0;

	int i = GetSelection();
	return (i == wxNOT_FOUND) ? 0 : GetLParam((unsigned int)i);
}

void lkImgComboBox::InheritAttributes()
{
    const wxWindow* const parent = GetParent();
    if ( !parent )
        return;

    // we only inherit attributes which had been explicitly set for the parent
    // which ensures that this only happens if the user really wants it and
    // not by default which wouldn't make any sense in modern GUIs where the
    // controls don't all use the same fonts (nor colours)
    if ( !m_hasFont )
        SetFont(parent->GetFont());
}

// //////////////////////////////////////////////////////////////////////////////////////
// Overrides
// //////////////////////////////////////////////////////////////////////////////////////

//virtual
wxString lkImgComboBox::GetStringSelection() const
{
	return wxItemContainer::GetStringSelection();
}

//virtual
bool lkImgComboBox::SetFont(const wxFont& font)
{
	bool res = wxOwnerDrawnComboBox::SetFont(font);
	UpdateInternals();
	return res;
}


//virtual
int lkImgComboBox::AddItem(lkBoxInit* pInit, wxBitmap* pBmp)
{
	wxASSERT(pInit != NULL);
	if (pInit == NULL) return -1;

	wxString s = pInit->GetString();
	int nIndex = wxOwnerDrawnComboBox::Append(s);

	if (nIndex >= 0)
	{
		lkImgClientData* clientData = new lkImgClientData(pBmp, pInit->GetParam());
		wxOwnerDrawnComboBox::SetClientObject(nIndex, (wxClientData*)clientData);
	}

	return nIndex;
}

//virtual
wxClientData* lkImgComboBox::GetItemObject(unsigned int item) const
{
	wxASSERT(HasClientObjectData());
	return GetClientObject(item);
}

//virtual
wxWindow* lkImgComboBox::GetControl()
{
	return this;
}

//virtual
void lkImgComboBox::StartOver()
{
	lkImageBox::StartOver();
	// only should be called if already created
	if (GetHandle() != NULL )
		wxOwnerDrawnComboBox::Clear();
}

// ----------------------------------------------------------------------------
// wxBitmapComboBox item drawing and measuring
// ----------------------------------------------------------------------------

// virtual
void lkImgComboBox::OnDrawBackground(wxDC& dc, const wxRect& rect, int item, int flags) const
{
	if ((GetCustomPaintWidth() == 0) || !(flags & wxODCB_PAINTING_SELECTED) || (item < 0) ||
		((flags & wxODCB_PAINTING_CONTROL) && (GetInternalFlags() & wxCC_FULL_BUTTON)))
	{
		wxOwnerDrawnComboBox::OnDrawBackground(dc, rect, item, flags);
		return;
	}
	// else
	lkImageBox::DrawBackground(dc, rect, item, flags);
}

// virtual
void lkImgComboBox::OnDrawItem(wxDC& dc, const wxRect& rect, int item, int flags) const
{
	wxString text;
	int imgAreaWidth = GetAreaWidth();

	if (imgAreaWidth == 0)
	{
		wxOwnerDrawnComboBox::OnDrawItem(dc, rect, item, flags);
		return;
	}

	if (flags & wxODCB_PAINTING_CONTROL)
	{
		text = GetValue();
		if (!HasFlag(wxCB_READONLY))
			text.clear();
	}
	else
	{
		text = GetString(item);
	}

	lkImageBox::DrawItem(dc, rect, item, text, flags);
}

//virtual
wxCoord lkImgComboBox::OnMeasureItem(size_t item) const
{
	return lkImageBox::MeasureItem(item);
}

//virtual
wxCoord lkImgComboBox::OnMeasureItemWidth(size_t item) const
{
	wxCoord x, y;
	GetTextExtent(GetString(item), &x, &y, 0, 0);
	x += GetAreaWidth();
	return x;
}

//virtual
wxSize lkImgComboBox::DoGetBestSize() const
{
	wxSize sz = wxOwnerDrawnComboBox::DoGetBestSize();

	if (HasFlag(wxCB_READONLY))
	{
		// Scale control to match height of highest image.
		int h2 = lkImageBox::GetBestHeight();

		if (h2 > sz.y)
			sz.y = h2;
	}

	return sz;
}

// //////////////////////////////////////////////////////////////////////////////////////
// Event handling
// //////////////////////////////////////////////////////////////////////////////////////

void lkImgComboBox::OnSize(wxSizeEvent& event)
{
	DetermineIndent();
	event.Skip();
}


#else

#include <wx/odcombo.h>
#include <wx/dc.h>
#include <wx/msw/dc.h>
#include <wx/dcclient.h>
#include <wx/msw/dcclient.h>
#include <wx/msw/private.h>
#include <wx/msw/private/paint.h>


// //////////////////////////////////////////////////////////////////////////////////////
// C'tor / D'tor / Initialisation
// //////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(lkImgComboBox, wxComboBox)

wxBEGIN_EVENT_TABLE(lkImgComboBox, wxComboBox)
	EVT_SIZE(lkImgComboBox::OnSize)
wxEND_EVENT_TABLE()


lkImgComboBox::lkImgComboBox() : wxComboBox(), lkImageBox()
{
	Init();
}
lkImgComboBox::lkImgComboBox(wxWindow* parent, wxWindowID id, const wxSize& size, long style, const wxValidator& validator, const wxPoint& pos) : wxComboBox(), lkImageBox()
{
	Init();
	(void)Create(parent, id, size, style, validator, pos);
}
lkImgComboBox::~lkImgComboBox()
{
}

bool lkImgComboBox::Create(wxWindow* parent, wxWindowID id, const wxSize& size, long style, const wxValidator& validator, const wxPoint& pos)
{
	// with images, only ReadOnly allowed
	if ( !(style & wxCB_READONLY) )
		style |= wxCB_READONLY;

	if ( !wxComboBox::Create(parent, id, wxEmptyString, pos, size, 0, NULL, style, validator, wxASCII_STR(lkImageComboBoxNameStr)) )
		return false;

	StartOver();
	UpdateInternals();

	return true;
}

void lkImgComboBox::Init()
{
}


wxUint64 lkImgComboBox::GetLParam(unsigned int n) const
{
	wxClientData* cont;
	if ( (cont = GetItemObject(n)) != NULL )
	{
		lkImgClientData* pData = dynamic_cast<lkImgClientData*>(cont);
		return pData->GetLPARAM();
	}
	return 0;
}
void lkImgComboBox::SetLParam(unsigned int n, wxUint64 lParam)
{
	wxClientData* cont;
	if ( (cont = GetItemObject(n)) != NULL )
	{
		lkImgClientData* pData = dynamic_cast<lkImgClientData*>(cont);
		pData->SetLPARAM(lParam);
	}
}


//virtual
void lkImgComboBox::SetSelectedLParam(wxUint64 u)
{ // Searches the list for the lparam, if found will select that item
	if ( IsListEmpty() )
		return;

	unsigned int i, c = GetCount();
	for ( i = 0; i < c; i++ )
		if ( GetLParam(i) == u )
		{
			SetSelection(i);
			return;
		}

	SetSelection(wxNOT_FOUND);
}

//virtual
wxUint64 lkImgComboBox::GetSelectedLParam() const
{ // From the selected item, the lparam will be returned
	if ( IsListEmpty() )
		return 0;

	int i = GetSelection();
	return (i == wxNOT_FOUND) ? 0 : GetLParam((unsigned int)i);
}

// //////////////////////////////////////////////////////////////////////////////////////
// Overrides
// //////////////////////////////////////////////////////////////////////////////////////

//virtual
wxString lkImgComboBox::GetStringSelection() const
{
	return wxItemContainer::GetStringSelection();
}

//virtual
bool lkImgComboBox::SetFont(const wxFont& font)
{
	bool res = wxComboBox::SetFont(font);
	UpdateInternals();
	return res;
}


//virtual
int lkImgComboBox::AddItem(lkBoxInit* pInit, wxBitmap* pBmp)
{
	wxASSERT(pInit != NULL);
	if ( pInit == NULL ) return -1;

	wxString s = pInit->GetString();
	int nIndex = wxComboBox::Append(s);

	if ( nIndex >= 0 )
	{
		lkImgClientData* clientData = new lkImgClientData(pBmp, pInit->GetParam());
		wxComboBox::SetClientObject(nIndex, (wxClientData*)clientData);
	}

	return nIndex;
}

//virtual
wxClientData* lkImgComboBox::GetItemObject(unsigned int item) const
{
	wxASSERT(HasClientObjectData());
	return GetClientObject(item);
}

//virtual
wxWindow* lkImgComboBox::GetControl()
{
	return this;
}

//virtual
void lkImgComboBox::StartOver()
{
	lkImageBox::StartOver();
	// only call when already created
	if ( GetHandle() != NULL )
		wxComboBox::Clear();
}


WXDWORD lkImgComboBox::MSWGetStyle(long style, WXDWORD* exstyle) const
{
	return wxComboBox::MSWGetStyle(style, exstyle) | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS;
}

//virtual
wxSize lkImgComboBox::DoGetBestSize() const
{
	wxSize sz = wxComboBox::DoGetBestSize();

	if ( HasFlag(wxCB_READONLY) )
	{
		// Scale control to match height of highest image.
		int h2 = lkImageBox::GetBestHeight();

		if ( h2 > sz.y )
			sz.y = h2;
	}

	return sz;
}

//virtual
WXHWND lkImgComboBox::GetEditHWND() const
{
	if ( HasFlag(wxCB_READONLY) )
		return NULL;

	return GetEditHWNDIfAvailable();
}


// ----------------------------------------------------------------------------
// wxBitmapComboBox item drawing and measuring
// ----------------------------------------------------------------------------

// create a wxRect from Windows RECT (in wxWidgets, this is a low-level private MSW function)
inline wxRect lkRectFromRECT(const RECT& rc)
{
	return wxRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
}

//virtual
bool lkImgComboBox::MSWOnDraw(WXDRAWITEMSTRUCT* item)
{
	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)item;
	int pos = lpDrawItem->itemID;

	wxPaintDCEx dc(this, lpDrawItem->hDC);
	wxRect rect = lkRectFromRECT(lpDrawItem->rcItem);
	// Draw default for item -1, which means 'focus rect only'
	if ( pos != -1 )
	{
		int flags = 0;
		if ( lpDrawItem->itemState & ODS_COMBOBOXEDIT )
			flags |= wxODCB_PAINTING_CONTROL;
		if ( lpDrawItem->itemState & ODS_SELECTED )
			flags |= wxODCB_PAINTING_SELECTED;

		lkImageBox::DrawBackground(dc, rect, pos, flags);

		wxString text;
		int imgAreaWidth = GetAreaWidth();

		if ( flags & wxODCB_PAINTING_CONTROL )
		{
			text = GetValue();
			if ( !HasFlag(wxCB_READONLY) )
				text.clear();
		}
		else
		{
			text = GetString(pos);
		}

		lkImageBox::DrawItem(dc, rect, pos, text, flags);
	}

	// If the item has the focus, draw focus rectangle.
	// Commented out since regular combo box doesn't
	// seem to do it either.
	if ( lpDrawItem->itemState & ODS_FOCUS )
		wxRendererNative::GetGeneric().DrawFocusRect(this, dc, rect, (lpDrawItem->itemState & ODS_SELECTED) ? wxCONTROL_SELECTED : wxCONTROL_NONE);

	return true;
}

bool lkImgComboBox::MSWOnMeasure(WXMEASUREITEMSTRUCT* item)
{
	LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT)item;
	int pos = lpMeasureItem->itemID;

	// Measure edit field height if item list is not empty,
	lpMeasureItem->itemHeight = lkImageBox::MeasureItem(pos);

	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////
// Event handling
// //////////////////////////////////////////////////////////////////////////////////////

void lkImgComboBox::OnSize(wxSizeEvent& event)
{
	DetermineIndent();
	event.Skip();
}

#endif // defined(__WXMSW__)




//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lkImgComboBoxValidator

wxIMPLEMENT_DYNAMIC_CLASS(lkImgComboBoxValidator, wxValidator)

lkImgComboBoxValidator::lkImgComboBoxValidator() : wxValidator()
{
	m_pLParam = NULL;
}
lkImgComboBoxValidator::lkImgComboBoxValidator(wxUint64* pLParam) : wxValidator()
{
	m_pLParam = pLParam;
}
lkImgComboBoxValidator::lkImgComboBoxValidator(const lkImgComboBoxValidator& other) : wxValidator(other)
{
	m_pLParam = other.m_pLParam;
}

//virtual
wxObject* lkImgComboBoxValidator::Clone() const
{
	return new lkImgComboBoxValidator(*this);
}

lkImgComboBox* lkImgComboBoxValidator::GetControl()
{
	if ( wxDynamicCast(m_validatorWindow, lkImgComboBox) )
		return dynamic_cast<lkImgComboBox*>(m_validatorWindow);

	wxFAIL_MSG(wxT("lkImgComboBoxValidator can only be used with a lkImgComboBox"));

	return NULL;
}

//virtual
bool lkImgComboBoxValidator::Validate(wxWindow* parent)
{
	wxASSERT(m_validatorWindow != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	lkImgComboBox* ctrl = GetControl();
	if ( !ctrl )
		return false;

	wxString msg;
	if ( ctrl->GetSelection() == wxNOT_FOUND)
		msg = wxT("No selection set yet, allthough this field is required.");

	if ( !msg.IsEmpty() )
	{
		m_validatorWindow->SetFocus();
		wxMessageBox(msg, wxT("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);

		return false;
	}

	return true;
}

//virtual
bool lkImgComboBoxValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);

	lkImgComboBox* ctrl = GetControl();
	if ( !ctrl )
		return false;

	if ( m_pLParam )
		ctrl->SetSelectedLParam(*m_pLParam);
	else
		ctrl->SetSelection(wxNOT_FOUND);

	return true;
}

//virtual
bool lkImgComboBoxValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	lkImgComboBox* ctrl = GetControl();
	if ( !ctrl )
		return false;

	if ( m_pLParam )
		*m_pLParam = ctrl->GetSelectedLParam();

	return true;
}

