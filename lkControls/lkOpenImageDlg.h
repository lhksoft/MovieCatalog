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
#ifndef __LK_OPENIMAGEDLG_H__
#define __LK_OPENIMAGEDLG_H__
#pragma once

#include <wx/filedlg.h>
#include <wx/bmpbuttn.h>
#include "lkFileCtrl.h"
#include <wx/artprov.h>

#include "lkStaticImage.h"

/* ****************************************************************************************
 * class lkOpenImageDlg for Opening an Image
 * 
 * Based at code of wxGenericFileDialog
 * **************************************************************************************** */
class lkOpenImageDlg : public wxFileDialogBase
{
	wxDECLARE_DYNAMIC_CLASS(lkOpenImageDlg);

public:
	lkOpenImageDlg();
	lkOpenImageDlg(wxWindow* parent, const wxString& defaultDir = wxEmptyString);
	virtual ~lkOpenImageDlg();

protected:
	bool					Create(wxWindow* parent, const wxString& defaultFile = wxEmptyString, const wxString& defaultDir = wxEmptyString,
									const wxPoint& pos = wxDefaultPosition, const wxSize& sz = wxDefaultSize);
		
	wxString				GetWildcards(void);

private:
	void					Init(void);

	wxBitmapButton*			AddBitmapButton(wxWindowID winId, const wxArtID& artId, const wxString& tip, wxSizer* sizer);

protected:
	wxString				m_filterExtension;
	lkFileCtrl*				m_filectrl;
	wxBitmapButton*			m_upDirButton;
	wxBitmapButton*			m_newDirButton;

	lkStaticImage*			m_imgPreview;

private:
	enum class ids
	{
		ID_LIST_MODE = wxID_HIGHEST + 1,
		ID_REPORT_MODE,
		ID_UP_DIR,
		ID_HOME_DIR,
		ID_NEW_DIR,
		ID_FILE_CTRL
	};

	static long				ms_lastViewStyle;     // list or report?
	static bool				ms_lastShowHidden;    // did we show hidden files?

	wxBoxSizer*				m_mainSizer;

// Overrides
public:
	virtual void			SetDirectory(const wxString& dir) wxOVERRIDE;
	virtual void			SetFilename(const wxString& name) wxOVERRIDE;
	virtual void			SetMessage(const wxString& message) wxOVERRIDE;
	virtual void			SetPath(const wxString& path) wxOVERRIDE;
	virtual void			SetFilterIndex(int filterIndex) wxOVERRIDE;
	virtual void			SetWildcard(const wxString& wildCard) wxOVERRIDE;

	virtual wxString		GetPath() const wxOVERRIDE;
	virtual void			GetPaths(wxArrayString& paths) const wxOVERRIDE;
	virtual wxString		GetDirectory() const wxOVERRIDE;
	virtual wxString		GetFilename() const wxOVERRIDE;
	virtual void			GetFilenames(wxArrayString& files) const wxOVERRIDE;
	virtual wxString		GetWildcard() const wxOVERRIDE;
	virtual int				GetFilterIndex() const wxOVERRIDE;
	virtual bool			SupportsExtraControl() const wxOVERRIDE;

	virtual int				ShowModal() wxOVERRIDE;
	virtual bool			Show(bool show = true) wxOVERRIDE;

// Event Processing
protected:
	void					OnList(wxCommandEvent& event);
	void					OnReport(wxCommandEvent& event);
	void					OnUp(wxCommandEvent& event);
	void					OnHome(wxCommandEvent& event);
	void					OnOk(wxCommandEvent& event);
	void					OnNew(wxCommandEvent& event);
	void					OnFileActivated(wxFileCtrlEvent& event);
	void					OnSelectionChanged(wxFileCtrlEvent& event);

	// update the state of m_upDirButton and m_newDirButton depending on the
	// currently selected directory
	void					OnUpdateButtonsUI(wxUpdateUIEvent& event);

//	wxDECLARE_EVENT_TABLE();
};


/* ****************************************************************************************
 * Prototype for Saving an Image
 * **************************************************************************************** */

// returns full path of Image to save to, an Empty string if CANCEL pressed
wxString lkSaveImageDlg(wxWindow*, wxBitmapType& safeType, const wxString& defaultFile = wxEmptyString, const wxString& defaultDir = wxEmptyString);

// returns the preserved wxBitmapType according given extension-string
wxBitmapType lkGetImageType(const wxString& extension);

#endif // !__LK_OPENIMAGEDLG_H__


