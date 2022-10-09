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
#include "lkStaticText.h"
#include <wx/textwrapper.h>
#include <wx/dcclient.h>
#include <wx/settings.h>

// ----------------------------------------------------------------------------
// wxLabelWrapper: helper class for wxStaticTextBase::Wrap()
// ----------------------------------------------------------------------------

class lkLabelWrapper
{
public:
	lkLabelWrapper();
	virtual ~lkLabelWrapper();

	wxString			WrapLabel(wxWindow* text, const wxString& label, int widthMax, int maxLines);
	int					GetLines(void) const;

protected:
	void				OnOutputLine(const wxString& line);
	void				OnNewLine(void);

	// call OnOutputLine() and set m_eol to true
	void				DoOutputLine(const wxString& line);

	// this function is a destructive inspector: when it returns true it also
	// resets the flag to false so calling it again wouldn't return true any
	// more
	bool				IsStartOfNewLine(void);

	// win is used for getting the font, text is the text to wrap, width is the max line width or -1 to disable wrapping
	void				Wrap(wxWindow* win, const wxString& text, int widthMax, int maxLines);

private:
	wxString			m_text;
	int					m_lines;
	bool				m_eol;

	wxDECLARE_NO_COPY_CLASS(lkLabelWrapper);
};

lkLabelWrapper::lkLabelWrapper() : m_text()
{
	m_lines = 0;
	m_eol = false;
}
lkLabelWrapper::~lkLabelWrapper()
{
}
wxString lkLabelWrapper::WrapLabel(wxWindow* text, const wxString& label, int widthMax, int maxLines)
{
	m_text.clear();
	m_lines = 0;
	Wrap(text, label, widthMax, maxLines);
	return m_text;
}
int lkLabelWrapper::GetLines(void) const
{
	return m_lines;
}
void lkLabelWrapper::OnOutputLine(const wxString& line)
{
	m_text += line;
}
void lkLabelWrapper::OnNewLine()
{
	m_text += wxT('\n');
}
void lkLabelWrapper::DoOutputLine(const wxString& line)
{
	OnOutputLine(line);
	m_eol = true;
	m_lines++;
}
bool lkLabelWrapper::IsStartOfNewLine()
{
	if ( !m_eol )
		return false;

	m_eol = false;

	return true;
}
void lkLabelWrapper::Wrap(wxWindow* win, const wxString& text, int widthMax, int maxLines)
{
	const wxClientDC dc(win);

	const wxArrayString ls = wxSplit(text, '\n', '\0');
	int mLines = maxLines;
	if ( mLines <= 0 )
		mLines = 1;
	for ( wxArrayString::const_iterator i = ls.begin(); i != ls.end(); ++i )
	{
		wxString line = *i;

		if ( i != ls.begin() )
		{
			// Do this even if the line is empty, except if it's the first one.
			OnNewLine();
		}

		// Is this a special case when wrapping is disabled?
		if ( widthMax < 0 )
		{
			DoOutputLine(line);
			continue;
		}

		for ( bool newLine = false; !line.empty(); newLine = true )
		{
			if ( newLine )
				OnNewLine();

			wxArrayInt widths;
			dc.GetPartialTextExtents(line, widths);

			const size_t posEnd = std::lower_bound(widths.begin(),
												   widths.end(),
												   widthMax) - widths.begin();

			// Does the entire remaining line fit? or have we reached the last visible line ?
			if ( (posEnd == line.length()) || (--mLines <= 0) )
			{
				DoOutputLine(line);
				break;
			}

			// Find the last word to chop off.
			const size_t lastSpace = line.rfind(' ', posEnd);
			if ( lastSpace == wxString::npos )
			{
				// No spaces, so can't wrap.
				DoOutputLine(line);
				break;
			}

			// Output the part that fits.
			DoOutputLine(line.substr(0, lastSpace));

			// And redo the layout with the rest.
			line = line.substr(lastSpace + 1);
		}
	}
}


// ----------------------------------------------------------------------------
// lkStaticText
// ----------------------------------------------------------------------------


wxIMPLEMENT_DYNAMIC_CLASS(lkStaticText, wxControl)

lkStaticText::lkStaticText() : wxControl(), m_label()
{
	Init();
}
lkStaticText::lkStaticText(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator) : wxControl(), m_label()
{
	Init();
	(void)Create(parent, id, label, pos, size, style, validator);
}
lkStaticText::~lkStaticText()
{
}

bool lkStaticText::Create(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator)
{
	if ( !wxControl::Create(parent, id, pos, size, style, validator, wxT("lkStaticText")) )
		return false;

	SetLabel(label);
	SetInitialSize(size);
	Bind(wxEVT_PAINT, &lkStaticText::OnPaint, this);
	return true;
}

void lkStaticText::Init()
{
	m_mnemonic = -1;
	m_nLines = 0;
}

wxString lkStaticText::Wrap(const wxString& label)
{
	wxClientDC dc(wxConstCast(this, lkStaticText));
	wxSize size = dc.GetTextExtent(label); // gets height of a single line
	wxSize sizeC = GetClientSize();
	int width = sizeC.GetWidth();
	int h = 1;

	if ( size.GetHeight() > 2 )
	{
		h = sizeC.GetHeight();

		// calculate how many lines fits completely in this control
		int m = h % size.GetHeight();
		h -= m;
		h /= size.GetHeight(); // this should be amount of visible lines
	}

	lkLabelWrapper wrapper;
	wxString _label = wrapper.WrapLabel(this, label, width, h);
	m_nLines = wrapper.GetLines();
	return _label;
}

//virtual
bool lkStaticText::AcceptsFocus() const
{
	return false;
}
//virtual
bool lkStaticText::HasTransparentBackground()
{
	return true;
}
bool lkStaticText::IsEllipsized() const
{
	return (GetWindowStyle() & wxST_ELLIPSIZE_MASK) != 0;
}
// choose the default border for this window
//virtual
wxBorder lkStaticText::GetDefaultBorder() const
{
	return wxBORDER_NONE;
}

void lkStaticText::AutoResizeIfNecessary()
{
	// This flag is specifically used to prevent the control from resizing even
	// when its label changes.
	if ( HasFlag(wxST_NO_AUTORESIZE) )
		return;

	// This method is only called if either the label or the font changed, i.e.
	// if the label extent changed, so the best size is not the same neither
	// any more.
	//
	// Note that we don't invalidate it when wxST_NO_AUTORESIZE is on because
	// this would result in the control being effectively resized during the
	// next Layout() and this style is used expressly to prevent this from
	// happening.
	InvalidateBestSize();

	SetSize(GetBestSize());
}

//virtual
void lkStaticText::SetLabel(const wxString& label)
{
//	const wxString _label = RemoveMarkup(label); // can return empty string
	wxControl::SetLabel(label);

	m_label = Wrap(m_labelOrig);

	WXSetVisibleLabel(GetEllipsizedLabel());

	AutoResizeIfNecessary();

	Refresh();
}

//virtual
bool lkStaticText::SetFont(const wxFont& font)
{
	if ( !wxControl::SetFont(font) )
		return false;

	AutoResizeIfNecessary();

	Refresh();
	return true;
}

//virtual
wxSize lkStaticText::DoGetBestClientSize() const
{
	wxClientDC dc(wxConstCast(this, lkStaticText));
	return dc.GetMultiLineTextExtent(GetLabel());
}

void lkStaticText::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
	wxControl::DoSetSize(x, y, width, height, sizeFlags);
	UpdateLabel();
	Refresh();
}


// ----------------------------------------------------------------------------
// wxStaticTextBase - generic implementation for wxST_ELLIPSIZE_* support
// ----------------------------------------------------------------------------

void lkStaticText::UpdateLabel()
{
	m_label = Wrap(m_labelOrig);

	if ( !IsEllipsized() )
		return;

	const wxString& newlabel = GetEllipsizedLabel();

	// we need to touch the "real" label (i.e. the text set inside the control,
	// using port-specific functions) instead of the string returned by GetLabel().
	//
	// In fact, we must be careful not to touch the original label passed to
	// SetLabel() otherwise GetLabel() will behave in a strange way to the user
	// (e.g. returning a "Ver...ing" instead of "Very long string") !
	if ( newlabel == WXGetVisibleLabel() )
		return;
	WXSetVisibleLabel(newlabel);
}

wxString lkStaticText::GetEllipsizedLabel() const
{
	// this function should be used only by ports which do not support
	// ellipsis in static texts: we first remove markup (which cannot
	// be handled safely by Ellipsize()) and then ellipsize the result.

	wxString ret(m_label);

	if ( IsEllipsized() )
		ret = Ellipsize(ret);

	return ret;
}

wxString lkStaticText::Ellipsize(const wxString& label) const
{
	wxSize sz(GetClientSize());
	if ( sz.GetWidth() < 2 || sz.GetHeight() < 2 )
	{
		// the size of this window is not valid (yet)
		return label;
	}

	wxEllipsizeMode mode;
	if ( HasFlag(wxST_ELLIPSIZE_START) )
		mode = wxELLIPSIZE_START;
	else if ( HasFlag(wxST_ELLIPSIZE_MIDDLE) )
		mode = wxELLIPSIZE_MIDDLE;
	else if ( HasFlag(wxST_ELLIPSIZE_END) )
		mode = wxELLIPSIZE_END;
	else
	{
		wxFAIL_MSG("should only be called if have one of wxST_ELLIPSIZE_XXX");

		return label;
	}

	wxClientDC dc(const_cast<lkStaticText*>(this));
	return wxControl::Ellipsize(label, dc, mode, sz.GetWidth());
}

wxString lkStaticText::WXGetVisibleLabel() const
{
	return m_label;
}

void lkStaticText::WXSetVisibleLabel(const wxString& str)
{
	m_mnemonic = FindAccelIndex(str, &m_label);
}


void lkStaticText::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxPaintDC dc(this);

	wxRect rect = GetClientRect();
	if ( !IsEnabled() )
	{
		// draw shadow of the text
		dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
		wxRect rectShadow = rect;
		rectShadow.Offset(1, 1);
		DoDrawLabel(dc, rectShadow);
		dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
	}
	DoDrawLabel(dc, rect);
}

void lkStaticText::DoDrawLabel(wxDC& dc, const wxRect& rect)
{
	dc.DrawLabel(m_label, rect, GetAlignment(), m_mnemonic);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(lkStaticTextValidator, wxGenericValidator)

lkStaticTextValidator::lkStaticTextValidator(wxString* val) : wxGenericValidator(val)
{
}
lkStaticTextValidator::lkStaticTextValidator(const lkStaticTextValidator& other) : wxGenericValidator(other)
{
}
lkStaticTextValidator::~lkStaticTextValidator()
{
}

//virtual
wxObject* lkStaticTextValidator::Clone() const
{
	return new lkStaticTextValidator(*this);
}

// Called to transfer data to the window
bool lkStaticTextValidator::TransferToWindow()
{
	if ( wxGenericValidator::TransferToWindow() )
		return true;

	if ( m_validatorWindow && wxDynamicCast(m_validatorWindow, lkStaticText) )
	{
		lkStaticText* pControl = (lkStaticText*)m_validatorWindow;
		if ( m_pString )
		{
			pControl->SetLabel(*m_pString);
			return true;
		}
	}

	return false;
}

// Called to transfer data from the window
bool lkStaticTextValidator::TransferFromWindow()
{
	if ( wxGenericValidator::TransferFromWindow() )
		return true;

	if ( m_validatorWindow && wxDynamicCast(m_validatorWindow, lkStaticText) )
	{
		lkStaticText* pControl = (lkStaticText*)m_validatorWindow;
		if ( m_pString )
		{
			*m_pString = pControl->GetLabel();
			return true;
		}
	}

	return false;
}
