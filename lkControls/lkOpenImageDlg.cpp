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
#include "lkOpenImageDlg.h"
#include "lkColour.h"

#include <wx/modalhook.h>
#include <wx/image.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>

/*
OPEN:
wxBITMAP_TYPE_BMP: Load a Windows bitmap file.
wxBITMAP_TYPE_GIF: Load a GIF bitmap file.
wxBITMAP_TYPE_JPEG: Load a JPEG bitmap file.
wxBITMAP_TYPE_PNG: Load a PNG bitmap file.
wxBITMAP_TYPE_PCX: Load a PCX bitmap file.
wxBITMAP_TYPE_PNM: Load a PNM bitmap file.
wxBITMAP_TYPE_TIFF: Load a TIFF bitmap file.
wxBITMAP_TYPE_TGA: Load a TGA bitmap file.
wxBITMAP_TYPE_XPM: Load a XPM bitmap file.
wxBITMAP_TYPE_ICO: Load a Windows icon file (ICO).
wxBITMAP_TYPE_CUR: Load a Windows cursor file (CUR).
wxBITMAP_TYPE_ANI: Load a Windows animated cursor file (ANI).
wxBITMAP_TYPE_ANY: Will try to autodetect the format.


SAVE:
wxBITMAP_TYPE_BMP: Save a BMP image file.
wxBITMAP_TYPE_JPEG: Save a JPEG image file.
wxBITMAP_TYPE_PNG: Save a PNG image file.
wxBITMAP_TYPE_PCX: Save a PCX image file
wxBITMAP_TYPE_PNM: Save a PNM image file
wxBITMAP_TYPE_TIFF: Save a TIFF image file.
wxBITMAP_TYPE_XPM: Save a XPM image file.
wxBITMAP_TYPE_ICO: Save a Windows icon file
wxBITMAP_TYPE_CUR: Save a Windows cursor file (CUR).
*/

#if defined(__WINDOWS__)
# define IsTopMostDir(dir)   (dir.empty())
#else
# define IsTopMostDir(dir)   (dir == wxT("/"))
#endif



// //////////////////////////////////////////////////////////////////////
// class lkOpenImageDlg
// //////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(lkOpenImageDlg, wxFileDialogBase)

long lkOpenImageDlg::ms_lastViewStyle = wxLC_REPORT;
bool lkOpenImageDlg::ms_lastShowHidden = false;

wxString lkOpenImageDlg::GetWildcards()
{
	wxString fmts, ext, ext_all, _name;

	wxList& Handlers = wxImage::GetHandlers();
	wxList::compatibility_iterator Node = Handlers.GetFirst();
	while ( Node )
	{
		wxImageHandler* Handler = (wxImageHandler*)Node->GetData();

		switch ( Handler->GetType() )
		{
			case wxBITMAP_TYPE_GIF:
			case wxBITMAP_TYPE_JPEG:
			case wxBITMAP_TYPE_PNG:
			case wxBITMAP_TYPE_PCX:
			case wxBITMAP_TYPE_PNM:
			case wxBITMAP_TYPE_TIFF:
			case wxBITMAP_TYPE_TGA:
			case wxBITMAP_TYPE_XPM:
			case wxBITMAP_TYPE_BMP:
			case wxBITMAP_TYPE_ICO:
			{
				if ( Handler->GetType() == wxBITMAP_TYPE_GIF )
					_name = wxT("Compuserve Graphics Interchange file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_JPEG )
					_name = wxT("JPeG Compliant file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_PNG )
					_name = wxT("Portable Network Graphic file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_PCX )
					_name = wxT("ZSoft Paintbrush file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_PNM )
					_name = wxT("Portable Network Pixel Map file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_TIFF )
					_name = wxT("Tagged Image file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_TGA )
					_name = wxT("Truevision Targa file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_XPM )
					_name = wxT("X PixMap bitmap file");
				else
					_name = Handler->GetName();

				if ( !fmts.IsEmpty() )
					fmts += wxT("|");
				if ( !ext_all.IsEmpty() )
					ext_all += wxT(";");

				// Name
				fmts += _name;
				fmts += wxT("s (");
				// default Extension
				ext = wxT("*.") + Handler->GetExtension();
				// alternative Extension(s)
				for ( size_t i = 0; i < Handler->GetAltExtensions().size(); i++ )
					ext += wxT(";*.") + Handler->GetAltExtensions()[i];
				fmts += ext;
				fmts += wxT(")|");
				fmts += ext;
				ext_all += ext;
				break;
			}
			default:
				break;
		}
		Node = Node->GetNext();
	}

	wxString fmtfiles;
	if ( ext_all.IsEmpty() )
	{
		// should not be the case, then use default
		ext_all = wxFileSelectorDefaultWildcardStr;
		fmtfiles.Printf(wxT("All Files (%s)|%s"), ext_all, ext_all);
	}
	else
	{
		wxASSERT(!fmts.IsEmpty());
		fmtfiles.Printf(wxT("All supported image files|%s|%s"), ext_all, fmts);
	}
	return fmtfiles;
}

lkOpenImageDlg::lkOpenImageDlg() : wxFileDialogBase()
{
	Init();
}
lkOpenImageDlg::lkOpenImageDlg(wxWindow* parent, const wxString& defaultDir) : wxFileDialogBase()
{
	Init();

	Create(parent, wxEmptyString, defaultDir);
}

lkOpenImageDlg::~lkOpenImageDlg()
{

}

void lkOpenImageDlg::Init()
{
	m_filectrl = NULL;
	m_upDirButton = NULL;
	m_newDirButton = NULL;
	m_imgPreview = NULL;
	m_mainSizer = NULL;

	// connect event handlers
	Connect((wxWindowID)lkOpenImageDlg::ids::ID_LIST_MODE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(lkOpenImageDlg::OnList));
	Connect((wxWindowID)lkOpenImageDlg::ids::ID_REPORT_MODE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(lkOpenImageDlg::OnReport));
	Connect((wxWindowID)lkOpenImageDlg::ids::ID_UP_DIR, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(lkOpenImageDlg::OnUp));
	Connect((wxWindowID)lkOpenImageDlg::ids::ID_HOME_DIR, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(lkOpenImageDlg::OnHome));
	Connect((wxWindowID)lkOpenImageDlg::ids::ID_NEW_DIR, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(lkOpenImageDlg::OnNew));
	Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(lkOpenImageDlg::OnOk));
	Connect((wxWindowID)lkOpenImageDlg::ids::ID_FILE_CTRL, wxEVT_FILECTRL_FILEACTIVATED, wxFileCtrlEventHandler(lkOpenImageDlg::OnFileActivated));
	Connect((wxWindowID)lkOpenImageDlg::ids::ID_UP_DIR, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(lkOpenImageDlg::OnUpdateButtonsUI));
#if defined(__WINDOWS__)
	Connect((wxWindowID)lkOpenImageDlg::ids::ID_NEW_DIR, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(lkOpenImageDlg::OnUpdateButtonsUI));
#endif // defined(__WINDOWS__)
	Connect((wxWindowID)lkOpenImageDlg::ids::ID_FILE_CTRL, wxEVT_FILECTRL_SELECTIONCHANGED, wxFileCtrlEventHandler(lkOpenImageDlg::OnSelectionChanged));
}

bool lkOpenImageDlg::Create(wxWindow* parent, const wxString& defaultFile, const wxString& defaultDir, const wxPoint& pos, const wxSize& sz)
{
	long style = wxFD_OPEN | wxFD_FILE_MUST_EXIST;
	wxString message = wxT("Select an Image to Open");
	wxString wildcard = GetWildcards();

	parent = GetParentForModalDialog(parent, style);


	if ( !wxFileDialogBase::Create(parent, message, defaultDir, defaultFile, wildcard, style, pos, sz) )
		return false;

	if ( !wxDialog::Create(parent, wxID_ANY, message, pos, sz, wxDEFAULT_DIALOG_STYLE | style) )
		return false;

	SetBackgroundColour(lkCol_AliceBlue);
/*
#if wxUSE_CONFIG
	if ( wxConfig::Get(false) )
	{
		wxConfig::Get()->Read(wxT("/wxWindows/wxFileDialog/ViewStyle"),
			&ms_lastViewStyle);
		wxConfig::Get()->Read(wxT("/wxWindows/wxFileDialog/ShowHidden"),
			&ms_lastShowHidden);
	}
#endif
*/
	if ( (m_dir.IsEmpty()) || (m_dir == wxT(".")) )
	{
		m_dir = wxGetCwd();
		if ( m_dir.empty() )
			m_dir = wxFILE_SEP_PATH;
	}

	const size_t len = m_dir.length();
	if ( (len > 1) && (wxEndsWithPathSeparator(m_dir)) )
		m_dir.Remove(len - 1, 1);

	m_filterExtension.clear();

	// layout

	const bool is_pda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);


	wxBoxSizer* szMain = new wxBoxSizer(wxHORIZONTAL);
	szMain->Add((m_imgPreview = new lkStaticImage(this, wxSize(250, 300))), 0, wxALL | wxEXPAND | wxALIGN_TOP, 11);

	m_mainSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* buttonsizer = new wxBoxSizer(wxHORIZONTAL);
	AddBitmapButton((wxWindowID)lkOpenImageDlg::ids::ID_LIST_MODE, wxART_LIST_VIEW, wxT("View files as a list view"), buttonsizer);
	AddBitmapButton((wxWindowID)lkOpenImageDlg::ids::ID_REPORT_MODE, wxART_REPORT_VIEW, wxT("View files as a detailed view"), buttonsizer);
	buttonsizer->Add(30, 5, 1);
	m_upDirButton = AddBitmapButton((wxWindowID)lkOpenImageDlg::ids::ID_UP_DIR, wxART_GO_DIR_UP, wxT("Go to parent directory"), buttonsizer);

	AddBitmapButton((wxWindowID)lkOpenImageDlg::ids::ID_HOME_DIR, wxART_GO_HOME, wxT("Go to home directory"), buttonsizer);
	buttonsizer->Add(20, 20);

	m_newDirButton = AddBitmapButton((wxWindowID)lkOpenImageDlg::ids::ID_NEW_DIR, wxART_NEW_DIR, wxT("Create new directory"), buttonsizer);

	if ( is_pda )
		m_mainSizer->Add(buttonsizer, wxSizerFlags().Expand());
	else
		m_mainSizer->Add(buttonsizer, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT | wxTOP));

	long style2 = 0;
	if ( HasFdFlag(wxFD_MULTIPLE) )
		style2 |= wxFC_MULTIPLE;

	m_filectrl = new lkFileCtrl(this, (wxWindowID)lkOpenImageDlg::ids::ID_FILE_CTRL, m_dir, defaultFile, wildcard, style2, wxDefaultPosition, wxSize(540, 300));
	m_filectrl->ShowHidden(ms_lastShowHidden);

	if ( ms_lastViewStyle == wxLC_LIST )
	{
		m_filectrl->ChangeToListMode();
	}
	else if ( ms_lastViewStyle == wxLC_REPORT )
	{
		m_filectrl->ChangeToReportMode();
	}

	m_mainSizer->Add(m_filectrl, wxSizerFlags(1).Expand().HorzBorder());

	wxSizer* bsizer = CreateButtonSizer(wxOK | wxCANCEL);
	if ( bsizer )
	{
		if ( is_pda )
			m_mainSizer->Add(bsizer, wxSizerFlags().Expand().Border());
		else
			m_mainSizer->Add(bsizer, wxSizerFlags().Expand().DoubleBorder());
	}
	szMain->Add(m_mainSizer);
	SetSizer(szMain);

	if ( !is_pda )
	{
		szMain->SetSizeHints(this);
		Center(wxBOTH);
	}

	return true;

}

wxBitmapButton* lkOpenImageDlg::AddBitmapButton(wxWindowID winId, const wxArtID& artId, const wxString& tip, wxSizer* sizer)
{
	wxBitmapButton* but = new wxBitmapButton(this, winId, wxArtProvider::GetBitmap(artId, wxART_BUTTON));
	but->SetToolTip(tip);
	sizer->Add(but, wxSizerFlags().Border());
	return but;
}


// Overrides

//virtual
void lkOpenImageDlg::SetDirectory(const wxString& dir)
{
	m_filectrl->SetDirectory(dir);
}
//virtual
void lkOpenImageDlg::SetFilename(const wxString& name)
{
	m_filectrl->SetFilename(name);
}
//virtual
void lkOpenImageDlg::SetMessage(const wxString& message)
{
	SetTitle(message);
}
//virtual
void lkOpenImageDlg::SetPath(const wxString& path)
{
	m_filectrl->SetPath(path);
}
//virtual
void lkOpenImageDlg::SetFilterIndex(int filterIndex)
{
	m_filectrl->SetFilterIndex(filterIndex);
}
//virtual
void lkOpenImageDlg::SetWildcard(const wxString& wildCard)
{
	m_filectrl->SetWildcard(wildCard);
}

//virtual
wxString lkOpenImageDlg::GetPath() const
{
	wxCHECK_MSG(!HasFlag(wxFD_MULTIPLE), wxString(), "When using wxFD_MULTIPLE, must call GetPaths() instead");
	return m_filectrl->GetPath();
}
//virtual
void lkOpenImageDlg::GetPaths(wxArrayString& paths) const
{
	m_filectrl->GetPaths(paths);
}
//virtual
wxString lkOpenImageDlg::GetDirectory() const
{
	return m_filectrl->GetDirectory();
}
//virtual
wxString lkOpenImageDlg::GetFilename() const
{
	wxCHECK_MSG(!HasFlag(wxFD_MULTIPLE), wxString(), "When using wxFD_MULTIPLE, must call GetFilenames() instead");
	return m_filectrl->GetFilename();
}
//virtual
void lkOpenImageDlg::GetFilenames(wxArrayString& files) const
{
	m_filectrl->GetFilenames(files);
}
//virtual
wxString lkOpenImageDlg::GetWildcard() const
{
	return m_filectrl->GetWildcard();
}
//virtual
int lkOpenImageDlg::GetFilterIndex() const
{
	return m_filectrl->GetFilterIndex();
}
//virtual
bool lkOpenImageDlg::SupportsExtraControl() const
{ 
	return true;
}

//virtual
int lkOpenImageDlg::ShowModal()
{
	WX_HOOK_MODAL_DIALOG();

	if ( CreateExtraControl() )
	{
		wxSizer* sizer = GetSizer();
		wxASSERT(m_mainSizer != NULL);
		m_mainSizer->Insert(2 /* after m_filectrl */, m_extraControl, wxSizerFlags().Expand().HorzBorder());
		sizer->Fit(this);
	}

	m_filectrl->SetDirectory(m_dir);

	return wxDialog::ShowModal();
}
//virtual
bool lkOpenImageDlg::Show(bool show)
{
	// Called by ShowModal, so don't repeate the update
#ifndef __WIN32__
	if ( show )
	{
		m_filectrl->SetDirectory(m_dir);
	}
#endif

	return wxDialog::Show(show);
}


// Event Processing

void lkOpenImageDlg::OnOk(wxCommandEvent& WXUNUSED(event))
{
	wxArrayString selectedFiles;
	m_filectrl->GetPaths(selectedFiles);

	if ( selectedFiles.Count() == 0 )
		return;

	const wxString& path = selectedFiles[0];

	if ( selectedFiles.Count() == 1 )
	{
		SetPath(path);
	}

	// check that the file [doesn't] exist if necessary
	if ( HasFdFlag(wxFD_SAVE) && HasFdFlag(wxFD_OVERWRITE_PROMPT) && wxFileExists(path) )
	{
		if ( wxMessageBox
		(
			wxString::Format
			(
				wxT("File '%s' already exists, do you really want to overwrite it?"),
				path
			),
			_("Confirm"),
			wxYES_NO
		) != wxYES )
			return;
	}
	else if ( HasFdFlag(wxFD_OPEN) && HasFdFlag(wxFD_FILE_MUST_EXIST) && !wxFileExists(path) )
	{
		wxMessageBox(_("Please choose an existing file."), _("Error"),
			wxOK | wxICON_ERROR);
		return;
	}

	EndModal(wxID_OK);
}

void lkOpenImageDlg::OnList(wxCommandEvent& WXUNUSED(event))
{
	m_filectrl->ChangeToListMode();
	ms_lastViewStyle = wxLC_LIST;
	m_filectrl->GetFileList()->SetFocus();
}

void lkOpenImageDlg::OnReport(wxCommandEvent& WXUNUSED(event))
{
	m_filectrl->ChangeToReportMode();
	ms_lastViewStyle = wxLC_REPORT;
	m_filectrl->GetFileList()->SetFocus();
}

void lkOpenImageDlg::OnUp(wxCommandEvent& WXUNUSED(event))
{
	m_filectrl->GoToParentDir();
	m_filectrl->GetFileList()->SetFocus();
}

void lkOpenImageDlg::OnHome(wxCommandEvent& WXUNUSED(event))
{
	m_filectrl->GoToHomeDir();
	m_filectrl->SetFocus();
}

void lkOpenImageDlg::OnNew(wxCommandEvent& WXUNUSED(event))
{
	m_filectrl->GetFileList()->MakeDir();
}

void lkOpenImageDlg::OnFileActivated(wxFileCtrlEvent& WXUNUSED(event))
{
	wxCommandEvent dummy;
	OnOk(dummy);
}

void lkOpenImageDlg::OnSelectionChanged(wxFileCtrlEvent& event)
{
/* *******************
 * Changed from wxGTK 3.2.4 :
 *
 * wxString File = event.GetFile();
 *  \> gives 'segment fault'
 * so changed into :
*/
	wxArrayString ar = event.GetFiles();
	if (!ar.IsEmpty())
	{
		wxString Dir = event.GetDirectory();
		wxString File = ar[0];
		wxFileName fN(Dir, File);
		if ( fN.IsOk() && fN.FileExists() )
		{
			wxImage Img;
			Img.LoadFile(fN.GetFullPath());
			m_imgPreview->SetImage(Img);
		}
		else
			m_imgPreview->SetEmpty();
	}
	else
		m_imgPreview->SetEmpty();
}

void lkOpenImageDlg::OnUpdateButtonsUI(wxUpdateUIEvent& event)
{
	// surprisingly, we can be called before m_filectrl is set in Create() as
	// wxFileCtrl ctor itself can generate idle events, so we need this test
	if ( m_filectrl )
		event.Enable(!IsTopMostDir(m_filectrl->GetShownDirectory()));
}

/* ********************************************************************************************************************* */

wxString _GetImageSaveWildcards()
{
	wxString fmts, ext, _name;

	wxList& Handlers = wxImage::GetHandlers();
	wxList::compatibility_iterator Node = Handlers.GetFirst();
	while ( Node )
	{
		wxImageHandler* Handler = (wxImageHandler*)Node->GetData();

		switch ( Handler->GetType() )
		{
			case wxBITMAP_TYPE_GIF:
			case wxBITMAP_TYPE_JPEG:
			case wxBITMAP_TYPE_PNG:
			case wxBITMAP_TYPE_PCX:
			case wxBITMAP_TYPE_PNM:
			case wxBITMAP_TYPE_TIFF:
			case wxBITMAP_TYPE_TGA:
			case wxBITMAP_TYPE_XPM:
			case wxBITMAP_TYPE_BMP:
			case wxBITMAP_TYPE_ICO:
			{
				if ( Handler->GetType() == wxBITMAP_TYPE_GIF )
					_name = wxT("Compuserve Graphics Interchange file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_JPEG )
					_name = wxT("JPeG Compliant file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_PNG )
					_name = wxT("Portable Network Graphic file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_PCX )
					_name = wxT("ZSoft Paintbrush file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_PNM )
					_name = wxT("Portable Network Pixel Map file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_TIFF )
					_name = wxT("Tagged Image file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_TGA )
					_name = wxT("Truevision Targa file");
				else if ( Handler->GetType() == wxBITMAP_TYPE_XPM )
					_name = wxT("X PixMap bitmap file");
				else
					_name = Handler->GetName();

				if ( !fmts.IsEmpty() )
					fmts += wxT("|");

				// Name
				fmts += _name;
				fmts += wxT("s (");
				// default Extension
				ext = wxT("*.") + Handler->GetExtension();
				// alternative Extension(s)
				for ( size_t i = 0; i < Handler->GetAltExtensions().size(); i++ )
					ext += wxT(";*.") + Handler->GetAltExtensions()[i];
				fmts += ext;
				fmts += wxT(")|");
				fmts += ext;
				break;
			}
			default:
				break;
		}
		Node = Node->GetNext();
	}

	wxString fmtfiles;
	if ( fmts.IsEmpty() )
	{
		// should not be the case, then use default
		ext = wxFileSelectorDefaultWildcardStr;
		fmtfiles.Printf(wxT("All Files (%s)|%s"), ext, ext);
	}
	else
	{
		wxASSERT(!fmts.IsEmpty());
		fmtfiles = fmts;
	}
	return fmtfiles;
}

int _FindIndex(const wxBitmapType safeTye)
{
	int _index = -1;

	wxList& Handlers = wxImage::GetHandlers();
	wxList::compatibility_iterator Node = Handlers.GetFirst();
	bool b = false;
	while ( Node )
	{
		wxImageHandler* Handler = (wxImageHandler*)Node->GetData();

		switch ( Handler->GetType() )
		{
			case wxBITMAP_TYPE_GIF:
			case wxBITMAP_TYPE_JPEG:
			case wxBITMAP_TYPE_PNG:
			case wxBITMAP_TYPE_PCX:
			case wxBITMAP_TYPE_PNM:
			case wxBITMAP_TYPE_TIFF:
			case wxBITMAP_TYPE_TGA:
			case wxBITMAP_TYPE_XPM:
			case wxBITMAP_TYPE_BMP:
			case wxBITMAP_TYPE_ICO:
			{
				_index++;
				if ( Handler->GetType() == safeTye )
					b = true;
				break;
			}
			default:
				break;
		}
		if ( b )
			break;
		Node = Node->GetNext();
	}
	return _index;
}

wxBitmapType _GetTypeByIndex(int index)
{
	wxBitmapType retVal = wxBITMAP_TYPE_INVALID;
	int i = -1;
	wxList& Handlers = wxImage::GetHandlers();
	wxList::compatibility_iterator Node = Handlers.GetFirst();
	bool b = false;

	if ( (index < 0) || Handlers.IsEmpty() )
		return retVal;

	while ( Node )
	{
		wxImageHandler* Handler = (wxImageHandler*)Node->GetData();

		switch ( Handler->GetType() )
		{
			case wxBITMAP_TYPE_GIF:
			case wxBITMAP_TYPE_JPEG:
			case wxBITMAP_TYPE_PNG:
			case wxBITMAP_TYPE_PCX:
			case wxBITMAP_TYPE_PNM:
			case wxBITMAP_TYPE_TIFF:
			case wxBITMAP_TYPE_TGA:
			case wxBITMAP_TYPE_XPM:
			case wxBITMAP_TYPE_BMP:
			case wxBITMAP_TYPE_ICO:
			{
				i++;
				if ( i == index )
				{
					retVal = Handler->GetType();
					b = true;
				}
				break;
			}
			default:
				break;
		}
		if ( b )
			break;
		Node = Node->GetNext();
	}
	return retVal;
}
wxString lkSaveImageDlg(wxWindow* pParent, wxBitmapType& safeType, const wxString& defaultFile, const wxString& defaultDir)
{
	wxString retFile;
	long style = wxFD_SAVE | wxFD_OVERWRITE_PROMPT;
	wxString message = wxT("Save Image");
	wxString wildcards = _GetImageSaveWildcards();

	wxFileDialog sF(pParent, message, defaultDir, defaultFile, wildcards, style);
	int index = _FindIndex(safeType);
	if ( index >= 0 )
		sF.SetFilterIndex(index);

	if ( sF.ShowModal() == wxID_CANCEL )
		return retFile;

	wxString fN = sF.GetFilename();
	wxString dR = sF.GetDirectory();

	// Get new File title
	wxFileName fP(dR, fN);
	if ( fP.IsOk() )
	{
		retFile = fP.GetFullPath();
		safeType = _GetTypeByIndex(sF.GetFilterIndex());
	}

	return retFile;
}

wxBitmapType lkGetImageType(const wxString& extension)
{
	wxBitmapType bmpType = wxBITMAP_TYPE_INVALID;

	wxString ext;
	wxList& Handlers = wxImage::GetHandlers();
	wxList::compatibility_iterator Node = Handlers.GetFirst();
	while ( Node )
	{
		wxImageHandler* Handler = (wxImageHandler*)Node->GetData();

		ext = Handler->GetExtension();
		if ( !ext.IsEmpty() && (ext.CmpNoCase(extension) == 0) )
		{
			bmpType = Handler->GetType();
			break;
		}

		// alternative Extension(s)
		for ( size_t i = 0; i < Handler->GetAltExtensions().size(); i++ )
			if ( (!(ext = Handler->GetAltExtensions()[i]).IsEmpty()) && (ext.CmpNoCase(extension) == 0) )
			{
				bmpType = Handler->GetType();
				break;
			}

		if ( bmpType != wxBITMAP_TYPE_INVALID )
			break;
		Node = Node->GetNext();
	}
	return bmpType;
}
