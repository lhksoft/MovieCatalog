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
#include "ImageCtrl.h"
#include "../lkControls/lkConfigTools.h"
#include "../lkControls/lkOpenImageDlg.h"
#include <wx/menu.h>
#include <wx/msgdlg.h> 

#define conf_SAVEKEY	wxT("SavePath")
#define conf_OPENKEY	wxT("OpenPath")

//////////////////////////////////////////////////////////////////////////////////////////////////////
// class ImageCtrl : using a wxImage
////

wxIMPLEMENT_DYNAMIC_CLASS(ImageCtrl, lkStaticImage)

wxBEGIN_EVENT_TABLE(ImageCtrl, lkStaticImage)
	EVT_RIGHT_UP(ImageCtrl::OnRightClick)
wxEND_EVENT_TABLE();

ImageCtrl::ImageCtrl() : lkStaticImage(), m_imgName(), m_ConfigSection()
{
	Init();
}
ImageCtrl::ImageCtrl(wxWindow* parent, const wxSize& size, wxWindowID id, const wxValidator& validator) : lkStaticImage(parent, size, id, validator), m_imgName(), m_ConfigSection()
{
	Init();
}
ImageCtrl::~ImageCtrl()
{
}

wxString ImageCtrl::GetImageName() const
{
	return m_imgName;
}
void ImageCtrl::SetImageName(const wxString& _name)
{
	m_imgName = _name;
}

void ImageCtrl::SetView(lkView* v)
{
	m_pView = v;
}
void ImageCtrl::SetConfigSection(const wxString& k)
{
	m_ConfigSection = k;
}

// Implementation

void ImageCtrl::Init()
{
	m_imgName.Empty();
	m_ConfigSection.Empty();
	m_pView = NULL;
}

int ImageCtrl::ConfirmRemoval()
{
	wxString ttl = wxT("Confirmation");
	wxString msg = wxT("Save current image first, before removal out of the database?");
	if ( m_pView )
		m_pView->SetCanClose(false);
	int nRet = wxMessageBox(msg, ttl, wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxICON_QUESTION, GetParent());
	if ( m_pView )
		m_pView->SetCanClose(true);
	return nRet;
}

void ImageCtrl::NewImage()
{
	if ( m_Image.IsOk() )
	{
		int ret = ConfirmRemoval();
		if ( ret == wxCANCEL )
			return; // do nothing
		else if ( ret == wxYES )
		{
			if ( !SaveImage() )
				return; // do nothing
		}
	}

	wxString P = GetConfigString(m_ConfigSection, conf_OPENKEY);
	lkOpenImageDlg* opImg = new lkOpenImageDlg(this, P);
	if ( m_pView )
		m_pView->SetCanClose(false);
	if ( opImg->ShowModal() == wxID_OK )
	{
		wxString fN = opImg->GetFilename();
		wxString dR = opImg->GetDirectory();

		// Get new File title
		wxFileName fP(dR, fN);
		if ( fP.IsOk() && fP.FileExists() )
		{
			wxImage Img;
			Img.LoadFile(fP.GetFullPath());
			SetImage(Img);
			SetImageName(fP.GetName());
		}

		SetConfigString(m_ConfigSection, conf_OPENKEY, fP.GetPath());
	}
	if ( m_pView )
		m_pView->SetCanClose(true);
	delete opImg;
}

void ImageCtrl::ClearImage()
{
	if ( m_Image.IsOk() )
	{
		int ret = ConfirmRemoval();
		if ( ret == wxCANCEL )
			return; // do nothing
		else if ( ret == wxYES )
		{
			if (!SaveImage())
				return; // do nothing
		}
	}
	SetEmpty();
}

//static
bool ImageCtrl::DoSaveImage(wxWindow* pParent, const wxImage& image, const wxString& iName, const wxString& confSection, const wxString& confKey)
{
	if ( !image.IsOk() )
		return true;

	bool bRet = false;
	wxBitmapType imgT = image.GetType();
	wxString savePath;
	wxString P = GetConfigString(confSection, (confKey.IsEmpty()) ? conf_SAVEKEY : confKey);

	if ( !(savePath = lkSaveImageDlg(pParent, imgT, iName, P)).IsEmpty() )
	{
		wxFileName fP(savePath);
		if ( imgT == wxBITMAP_TYPE_INVALID )
			imgT = lkGetImageType(fP.GetExt());

		SetConfigString(confSection, (confKey.IsEmpty()) ? conf_SAVEKEY : confKey, fP.GetPath());

		image.SaveFile(savePath, imgT);
		bRet = true;
	}
	return bRet;
}

bool ImageCtrl::SaveImage()
{
	if ( !m_Image.IsOk() )
		return true;

	if ( m_pView )
		m_pView->SetCanClose(false);

	bool bRet = ImageCtrl::DoSaveImage(GetParent(), m_Image, GetImageName(), m_ConfigSection);

	if ( m_pView )
		m_pView->SetCanClose(true);
	// else
	return bRet;
}


// Events

void ImageCtrl::OnRightClick(wxMouseEvent& event)
{
	wxMenu mnu;
	wxMenuItem* itm;
	bool hasImage = m_Image.IsOk();

	mnu.Append((wxWindowID)ImageCtrl::menu_ids::ID_New, wxT("New"), wxT("Select a new Image"));

	itm = mnu.Append((wxWindowID)ImageCtrl::menu_ids::ID_Clear, wxT("Clear"), wxT("Clear current Image"));
	itm->Enable(hasImage);

	itm = mnu.Append((wxWindowID)ImageCtrl::menu_ids::ID_Save, wxT("Save"), wxT("Save current Image to a File"));
	itm->Enable(hasImage);

	mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ImageCtrl::OnMenu), NULL, this);
	PopupMenu(&mnu);
}

void ImageCtrl::OnMenu(wxCommandEvent& event)
{
	switch ( (ImageCtrl::menu_ids)event.GetId() )
	{
		case ImageCtrl::menu_ids::ID_New:
			NewImage();
			break;
		case ImageCtrl::menu_ids::ID_Clear:
			ClearImage();
			break;
		case ImageCtrl::menu_ids::ID_Save:
			SaveImage();
			break;
		default:
			break;
	}
}

// Helpers
//////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
// class ImgPathCtrl : using a wxString as the path to the Image
////
wxIMPLEMENT_DYNAMIC_CLASS(ImgPathCtrl, lkStaticImage)

wxBEGIN_EVENT_TABLE(ImgPathCtrl, lkStaticImage)
	EVT_RIGHT_UP(ImgPathCtrl::OnRightClick)
wxEND_EVENT_TABLE();


ImgPathCtrl::ImgPathCtrl() : lkStaticImage(), m_sFullPath(), m_ConfigSection()
{
	Init();
}
ImgPathCtrl::ImgPathCtrl(wxWindow* parent, const wxSize& size, wxWindowID id, const wxValidator& validator) : lkStaticImage(parent, size, id, validator), m_sFullPath(), m_ConfigSection()
{
	Init();
}
ImgPathCtrl::~ImgPathCtrl()
{
}

// Implementation

void ImgPathCtrl::Init()
{
	m_sFullPath.Empty();
	m_ConfigSection.Empty();
	m_pView = NULL;
}

void ImgPathCtrl::SetView(lkView* v)
{
	m_pView = v;
}
void ImgPathCtrl::SetConfigSection(const wxString& k)
{
	m_ConfigSection = k;
}

//virtual
void ImgPathCtrl::SetImage(const wxString& strPath)
{
	lkStaticImage::SetImage(strPath);
	m_sFullPath = strPath;
}

wxString ImgPathCtrl::GetImage() const
{
	return m_sFullPath;
}

void ImgPathCtrl::NewImage()
{
	wxString P = GetConfigString(m_ConfigSection, conf_OPENKEY);
	wxString F;
	{
		F = GetImage();
		if (!F.IsEmpty() && wxFileExists(F))
		{
			wxString fPath, fName, fExt;
			wxFileName::SplitPath(F, &fPath, &fName, &fExt);
			if (!fPath.IsEmpty() && (fPath.Right(1) != wxFileName::GetPathSeparator()))
				fPath += wxFileName::GetPathSeparator();
			if (!fPath.IsEmpty() && !fName.IsEmpty() && wxDirExists(fPath))
			{
				P = fPath;
				F = fName;
				if (!fExt.IsEmpty())
					F += (wxT(".") + fExt);
			}
		}
	}
	lkOpenImageDlg* opImg = new lkOpenImageDlg(this, P, F);
	if ( m_pView )
		m_pView->SetCanClose(false);
	if ( opImg->ShowModal() == wxID_OK )
	{
		wxString fN = opImg->GetFilename();
		wxString dR = opImg->GetDirectory();

		// Get new File title
		wxFileName fP(dR, fN);
		if ( fP.IsOk() && fP.FileExists() )
			SetImage(fP.GetFullPath());

		SetConfigString(m_ConfigSection, conf_OPENKEY, fP.GetPath());
	}
	if ( m_pView )
		m_pView->SetCanClose(true);
	delete opImg;
}

//virtual
void ImgPathCtrl::SetEmpty(bool bRefresh/*=true*/)
{
	m_sFullPath.Empty();
	lkStaticImage::SetEmpty(bRefresh);
}


// Events

void ImgPathCtrl::OnRightClick(wxMouseEvent& event)
{
	wxMenu mnu;
	wxMenuItem* itm;
	bool hasImage = m_Image.IsOk();

	mnu.Append((wxWindowID)ImgPathCtrl::menu_ids::ID_New, wxT("New"), wxT("Select a new Image"));

	itm = mnu.Append((wxWindowID)ImgPathCtrl::menu_ids::ID_Clear, wxT("Clear"), wxT("Clear current Image"));
	itm->Enable(hasImage);

	mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ImgPathCtrl::OnMenu), NULL, this);
	PopupMenu(&mnu);
}

void ImgPathCtrl::OnMenu(wxCommandEvent& event)
{
	switch ( (ImgPathCtrl::menu_ids)event.GetId() )
	{
		case ImgPathCtrl::menu_ids::ID_New:
			NewImage();
			break;
		case ImgPathCtrl::menu_ids::ID_Clear:
			SetEmpty();
		default:
			break;
	}
}

