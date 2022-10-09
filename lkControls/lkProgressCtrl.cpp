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
#include "lkProgressCtrl.h"
#include "lkColour.h"

#include <wx/dcclient.h>
#include <wx/math.h>
#include <wx/string.h>

const char lkProgressCtrlNameStr[] = "lkProgressCtrl";

wxIMPLEMENT_DYNAMIC_CLASS(lkProgressCtrl, wxControl)

wxBEGIN_EVENT_TABLE(lkProgressCtrl, wxControl)
	EVT_PAINT(lkProgressCtrl::OnPaint)
	EVT_SIZE(lkProgressCtrl::OnSize)
wxEND_EVENT_TABLE()

lkProgressCtrl::lkProgressCtrl() : wxControl(), m_Flags(0), m_rangeMin(0), m_rangeMax(100), m_curPos(0), m_Step(1)
{
}
lkProgressCtrl::lkProgressCtrl(wxWindow* parent, int range, long flags, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator) : 
	wxControl(), m_Flags(0), m_rangeMin(0), m_rangeMax(100), m_curPos(0), m_Step(1)
{
	(void)lkProgressCtrl::Create(parent, range, flags, id, pos, size, style, validator);
}

lkProgressCtrl::~lkProgressCtrl()
{
}

bool lkProgressCtrl::Create(wxWindow* parent, int range, long flags, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator)
{
	m_Flags = flags;
	if ( !wxControl::Create(parent, id, pos, size, style, validator, wxASCII_STR(lkProgressCtrlNameStr)) )
		return false;

	SetRange(0, range);
	SetValue(0, false);

	// change the internal font to BOLD
	wxFont fc = GetFont();
	fc.MakeBold();
	// for DrawRotatedText only TTF are accepted, but DrawRotatedText seems not to work very well .. see inside OnPaint
	//	fc.SetFamily(wxFONTFAMILY_SWISS);
	SetFont(fc);

	// Set background colour to WHITE
	SetBackgroundColour(lkCol_White);
	// .. and the foreground to dark-blue
	SetForegroundColour(lkCol_DarkBlue);

	return true;
}

void lkProgressCtrl::SetRange(int min, int max)
{
	m_rangeMin = min;
	m_rangeMax = max;
}
int lkProgressCtrl::GetMinRange() const
{
	return m_rangeMin;
}
int lkProgressCtrl::GetMaxRange() const
{
	return m_rangeMax;
}

void lkProgressCtrl::SetValue(int pos, bool bRefresh)
{
	wxASSERT(pos <= m_rangeMax);
	wxASSERT(pos >= m_rangeMin);
	m_curPos = pos;

	if ( bRefresh )
		Refresh(true);
}
int lkProgressCtrl::GetValue() const
{
	return m_curPos;
}

void lkProgressCtrl::SetStep(int step)
{
	wxASSERT(step < m_rangeMax);
	wxASSERT(step > m_rangeMin);
	m_Step = step;
}
int lkProgressCtrl::GetStep() const
{
	return m_Step;
}

void lkProgressCtrl::SetHorizontal()
{
	if ( m_Flags & pcVERTICAL )
		m_Flags &= ~pcVERTICAL;
}
bool lkProgressCtrl::IsHorizontal() const
{
	return !(m_Flags & pcVERTICAL);
}

void lkProgressCtrl::SetVertical()
{
	m_Flags |= pcVERTICAL;
}
bool lkProgressCtrl::IsVertical() const
{
	return (m_Flags & pcVERTICAL);
}

//virtual
void lkProgressCtrl::StepIt()
{
	Pulse();
}

//virtual
void lkProgressCtrl::Pulse()
{
	int curr = GetValue(), max = GetMaxRange(), min = GetMinRange(), step = GetStep();

	if ( !(m_Flags & pcDIRECTION_LEFT) )
	{
		if ( curr < max )
			SetValue(curr + step);
		else
		{
			SetValue(max - step);
			m_Flags |= pcDIRECTION_LEFT;
		}
	}
	else
	{
		if ( curr > min )
			SetValue(curr - step);
		else
		{
			SetValue(min + step);
			m_Flags &= ~pcDIRECTION_LEFT;
		}
	}
}

//virtual
bool lkProgressCtrl::AcceptsFocus() const
{
	return false;
}

//virtual
wxBorder lkProgressCtrl::GetDefaultBorder() const
{
	return wxBORDER_SIMPLE;
}

//virtual
wxSize lkProgressCtrl::DoGetBestClientSize() const
{
	int fontHeight = GetCharHeight();
	int nBestHeight = (2 * 2) + fontHeight;
	wxSize sz;
	if ( IsHorizontal() )
		sz = wxSize(wxDefaultCoord, nBestHeight);
	/*
		else
			sz = wxSize(nBestHeight, wxDefaultCoord);
	*/

	return sz;
}

//virtual
wxString lkProgressCtrl::GenerateInternalText()
{
	wxString out;
	int cur = GetValue();

	if ( m_Flags & pcTIME )
	{
		int s = cur % 60;
		int m = cur / 60;

		out.Printf(wxT("%d' %02d\""), m, s);
	}
	else if ( m_Flags & pcRAW )
	{
		int max = GetMaxRange();
		out.Printf(wxT("%d / %d"), cur, max);
	}
	else
	{
		int min = GetMinRange(), max = GetMaxRange();
		float f = static_cast<float>(cur - min), t = static_cast<float>(max - min);
		t /= 100; // make it 1%
		f /= t;
		int percent = wxRound(f);

		out.Printf(wxT("%d%%"), percent);
	}

	return out;
}

void lkProgressCtrl::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	wxRect rect = GetClientRect();
	wxString text = GenerateInternalText();

	wxSize size, scale;
	scale.x = rect.GetWidth() - rect.GetLeft();
	scale.y = rect.GetHeight() - rect.GetTop();

	wxCoord x, y;

	int cur = GetValue(), min = GetMinRange(), max = GetMaxRange();
	float percent = static_cast<float>(cur - min), t = static_cast<float>(max - min);
	percent /= t;

	size = GetTextExtent(text);

	// ++ Draw the left part +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if ( IsHorizontal() )
	{
		rect.SetWidth(wxRound(static_cast<float>(scale.x) * percent));
		rect.SetTop(0);
	}
	else
	{ // show vertical
		rect.SetWidth(scale.x);
		rect.SetTop(wxRound(static_cast<float>(scale.y) * (1 - percent)));
	}
	rect.SetBottom(scale.y);

	dc.SetBrush(GetForegroundColour());
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(rect);

	dc.DestroyClippingRegion();
	dc.SetClippingRegion(rect);

	if ( IsHorizontal() )
	{
		y = (scale.y - size.y) / 2;

		if ( m_Flags & pcALIGN_LEFT )
			x = 0;
		else if ( m_Flags & pcALIGN_RIGHT )
			x = scale.x - size.x;
		else
			x = (scale.x - size.x) / 2;
	}
	/*
		else
		{
			x = (scale.x - size.x) / 2;

			if ( m_Flags & pcALIGN_LEFT )
				y = scale.y;
			else if ( m_Flags & pcALIGN_RIGHT )
				y = scale.y - size.y;
			else
				y = (scale.y - size.y) / 2;
		}
	*/

	dc.SetTextBackground(GetForegroundColour());
	dc.SetTextForeground(GetBackgroundColour());

	// draw inner-text
	if ( IsHorizontal() )
		dc.DrawText(text, wxPoint(x, y));
	/*
	* this doesn't seems to work very well .. so until fixed, no text in vertical progressbars
		else
			dc.DrawRotatedText(text, size.y, 0, 90);
	*/
	dc.DestroyClippingRegion();

	// ++ Draw the right part +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if ( IsHorizontal() )
	{
		rect.SetLeft(rect.GetWidth());
		rect.SetWidth(scale.x);
	}
	else
	{ // show vertical
		rect.SetHeight(rect.GetTop());
		rect.SetTop(0);
	}

	dc.SetBrush(GetBackgroundColour());
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(rect);

	dc.DestroyClippingRegion();
	dc.SetClippingRegion(rect);

	if ( IsHorizontal() )
	{
		y = (scale.y - size.y) / 2;

		if ( m_Flags & pcALIGN_LEFT )
			x = 0;
		else if ( m_Flags & pcALIGN_RIGHT )
			x = scale.x - size.x;
		else
			x = (scale.x - size.x) / 2;
	}

	dc.SetTextBackground(GetBackgroundColour());
	dc.SetTextForeground(GetForegroundColour());

	// draw outer-text
	if ( IsHorizontal() )
		dc.DrawText(text, wxPoint(x, y));

	dc.DestroyClippingRegion();
}

void lkProgressCtrl::OnSize(wxSizeEvent& event)
{
	event.Skip();
	Refresh(true);
}
