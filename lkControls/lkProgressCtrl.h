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
// //////////////////////////////////////////////////////////////////////////
// class lkProgressCtrl
// my own wxGauge, directly based from wxControl
#ifndef __LK_PROGRESSCTRL_H__
#define __LK_PROGRESSCTRL_H__
#pragma once

#include <wx/control.h>

// ----------------------------------------------------------------------------
// lkProgressCtrl style flags
// ----------------------------------------------------------------------------

enum lkProgressCtrl_Flags
{
	pcHORIZONTAL = 0,			// horizontal bar (default)
	pcVERTICAL = 0x0001,		// vertical bar
/* pcDIRECTION flags only used internal */
	pcDIRECTION_RIGHT = 0,		// bar goes from left to right (default)
	pcDIRECTION_LEFT = 0x0002,	// bar goes from right to left
/* end - used internal */
	pcALIGN_CENTER = 0,
	pcALIGN_LEFT = 0x0010,		// with a vertical gauge, this is the downside
	pcALIGN_RIGHT = 0x0020,		// with a vertical gauge, this is the upside

	pcPERCENT = 0,				// as percentage      50% / 100% (default)
	pcTIME = 0x0200,			// as minutes/seconds 30' 52" 
	pcRAW = 0x0400				// pure               500 / 1000
};

// ----------------------------------------------------------------------------
// lkProgressCtrl class definition
// ----------------------------------------------------------------------------

//extern const char lkProgressCtrlNameStr[];

class lkProgressCtrl : public wxControl
{
public:
	lkProgressCtrl();
	lkProgressCtrl(wxWindow* parent,
				   int range,
				   long flags = 0,
				   wxWindowID id = wxID_ANY,
				   const wxPoint& pos = wxDefaultPosition,
				   const wxSize& size = wxDefaultSize,
				   long style = 0,
				   const wxValidator& validator = wxDefaultValidator);

	virtual ~lkProgressCtrl();

	bool								Create(wxWindow* parent,
											   int range,
											   long flags,
											   wxWindowID id = wxID_ANY,
											   const wxPoint& pos = wxDefaultPosition,
											   const wxSize& size = wxDefaultSize,
											   long style = pcHORIZONTAL | pcALIGN_CENTER,
											   const wxValidator& validator = wxDefaultValidator);


	// set/get Attributes
	void								SetRange(int min, int max);
	int									GetMinRange(void) const;
	int									GetMaxRange(void) const;

	void								SetValue(int pos, bool bRefresh = true);
	int									GetValue(void) const;

	void								SetStep(int step);
	int									GetStep(void) const;

	void								SetHorizontal(void);
	bool								IsHorizontal(void) const;

	void								SetVertical(void);
	bool								IsVertical(void) const;

	virtual void						StepIt(void);
	virtual void						Pulse(void);


// overridden base class virtuals
	virtual bool						AcceptsFocus() const wxOVERRIDE;


protected:
	virtual wxBorder					GetDefaultBorder() const wxOVERRIDE;
	virtual wxSize						DoGetBestClientSize() const wxOVERRIDE;

	virtual wxString					GenerateInternalText(void);	// generates text to be used inside the bar

	void								OnPaint(wxPaintEvent&);
	void								OnSize(wxSizeEvent& event);


// Atributes
protected:
	// any of the lkProgressCtrl_Flags enum
	long								m_Flags;
	// the min/max position
	int									m_rangeMin; // default = 0
	int									m_rangeMax; // default = 100
	// the current position
	int									m_curPos;
	// the default Step value
	int									m_Step; // default = 1

private:
	wxDECLARE_EVENT_TABLE();

	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkProgressCtrl);
};

#endif // !__LK_PROGRESSCTRL_H__

