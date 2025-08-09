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
#include "Compact.h"
#include "CompactThread.h"
#include "Backgrounds.h"

#include <wx/panel.h>
#include <wx/custombgwin.h>
#include <wx/image.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/valgen.h>
#include <wx/log.h>
#include "../lkControls/lkProgressCtrl.h"
#include "../lkControls/lkColour.h"
#include "../lkControls/lkStaticText.h"

#include <wx/msgdlg.h>


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  CompactCanvas
////
class CompactCanvas : public wxCustomBackgroundWindow<wxPanel>
{
public:
	CompactCanvas();
	CompactCanvas(wxFrame* parent);
	virtual ~CompactCanvas();

	bool							Create(wxFrame* parent);

public:
	lkStaticText*					m_pStatTxt1, *m_pStatTxt2;
	lkProgressCtrl*					m_pProgress1, *m_pProgress2;
	wxString						m_sStatTxt1, m_sStatTxt2;

protected:
	void							OnRightClick(wxMouseEvent& event);

private:
	wxDECLARE_EVENT_TABLE();
	wxDECLARE_CLASS(CompactCanvas);
	wxDECLARE_NO_COPY_CLASS(CompactCanvas);
};

wxIMPLEMENT_CLASS(CompactCanvas, wxCustomBackgroundWindow<wxPanel>)

wxBEGIN_EVENT_TABLE(CompactCanvas, wxCustomBackgroundWindow<wxPanel>)
	EVT_RIGHT_DOWN(CompactCanvas::OnRightClick)
wxEND_EVENT_TABLE()

CompactCanvas::CompactCanvas() : wxCustomBackgroundWindow<wxPanel>(), m_sStatTxt1(), m_sStatTxt2()
{
	m_pStatTxt1 = m_pStatTxt2 = NULL;
	m_pProgress1 = m_pProgress2 = NULL;
}
CompactCanvas::CompactCanvas(wxFrame* parent) : wxCustomBackgroundWindow<wxPanel>(), m_sStatTxt1(), m_sStatTxt2()
{
	m_pStatTxt1 = m_pStatTxt2 = NULL;
	m_pProgress1 = m_pProgress2 = NULL;

	(void)Create(parent);
}
CompactCanvas::~CompactCanvas()
{
}
bool CompactCanvas::Create(wxFrame* parent)
{
	if ( !wxCustomBackgroundWindow<wxPanel>::Create(parent) )
		return false;

	wxImage iBack = GetImage_FBG_COMPACT();
	if ( iBack.IsOk() )
	{
		wxBitmap bmp(iBack);
		SetBackgroundBitmap(bmp);
	}

	// Build the canvas
	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);

	long border =
#ifdef __WXMSW__
		wxBORDER_STATIC
#else
		wxBORDER_SUNKEN
#endif // __WXMSW__
		;

	wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
	szHor->Add(m_pStatTxt1 = new lkStaticText(this, wxID_ANY, wxT("Line #1"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL | border | wxST_NO_AUTORESIZE), 1, wxEXPAND);
	m_pStatTxt1->SetValidator(lkStaticTextValidator(&m_sStatTxt1));
	szMain->Add(szHor, 0, wxEXPAND | wxALL, 10);

	szHor = new wxBoxSizer(wxHORIZONTAL);
	szHor->Add(m_pStatTxt2 = new lkStaticText(this, wxID_ANY, /*GetExitString()*/wxT("Line #2"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL | border | wxST_NO_AUTORESIZE), 1, wxEXPAND);
	m_pStatTxt2->SetValidator(lkStaticTextValidator(&m_sStatTxt2));
	szMain->Add(szHor, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 10);

	szHor = new wxBoxSizer(wxHORIZONTAL);
	szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Current :"), wxDefaultPosition, wxSize(75, -1), wxALIGN_RIGHT), 0, wxFIXED_MINSIZE | wxRIGHT, 5);
	szHor->Add(m_pProgress1 = new lkProgressCtrl(this, 100, pcHORIZONTAL | pcALIGN_CENTER | pcRAW, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE), 1, wxEXPAND);
	szMain->Add(szHor, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 10);

	szHor = new wxBoxSizer(wxHORIZONTAL);
	szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Total :"), wxDefaultPosition, wxSize(75, -1), wxALIGN_RIGHT), 0, wxFIXED_MINSIZE | wxRIGHT, 5);
	szHor->Add(m_pProgress2 = new lkProgressCtrl(this, 100, pcHORIZONTAL | pcALIGN_CENTER | pcPERCENT, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE), 1, wxEXPAND);
	szMain->Add(szHor, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 10);

	SetSizer(szMain);
	Layout();

	return true;

}

void CompactCanvas::OnRightClick(wxMouseEvent& event)
{
#ifdef __WXDEBUG__
	wxSize sizeC = GetClientSize();
	wxSize sizeF = GetParent()->GetSize();
	wxString s; s.Printf(wxT("Frame Size\nwidth = %d | height = %d\n\nClient Size\nwidth = %d | height = %d"), sizeF.GetWidth(), sizeF.GetHeight(), sizeC.GetWidth(), sizeC.GetHeight());
	wxMessageBox(s, wxT("For your information"), wxOK | wxICON_INFORMATION, this);
#else
	event.Skip();
#endif // WXDEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  CompactFrame
////
wxIMPLEMENT_CLASS(CompactFrame, wxFrame)

wxBEGIN_EVENT_TABLE(CompactFrame, wxFrame)
	EVT_CLOSE(CompactFrame::OnClose)
	EVT_CHAR_HOOK(CompactFrame::OnCharHook)
	EVT_COMPACT_EVENT(wxID_ANY, CompactFrame::OnThread)
wxEND_EVENT_TABLE()

CompactFrame::CompactFrame(const wxString& srcPath) : wxFrame(), m_sSrcPath(srcPath)
{
	m_pThread = NULL;
	m_pCanvas = NULL;
	m_DisableAllOthers = NULL;
	m_bThrPaused = false;
}
CompactFrame::~CompactFrame()
{
	if ( m_DisableAllOthers )
		delete m_DisableAllOthers;
}

bool CompactFrame::Create(wxWindow* parent)
{
	long style = wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR | wxCAPTION | wxRESIZE_BORDER;
#ifdef __WXDEBUG__
	//	style |= wxCLOSE_BOX;
#endif //__WXDEBUG__

	wxSize size =
#ifdef __WXMSW__
		wxSize(450, 167)
#else
		wxSize(470, 167)
#endif
		;
	if ( !wxFrame::Create(parent, wxID_ANY, wxT("Compact Database"), wxDefaultPosition, size, style) )
		return false;

	size =
#ifdef __WXMSW__
		wxSize(434, 128)
#else
		wxSize(470, 143)
#endif
		;
	SetMinClientSize(size);

	m_pCanvas = new CompactCanvas(this);

	return true;
}

bool CompactFrame::IsEscapeKey(const wxKeyEvent& event)
{
#ifdef __WXMAC__
	if ( event.GetKeyCode() == '.' && event.GetModifiers() == wxMOD_CONTROL )
		return true;
#endif // __WXMAC__

	return (event.GetKeyCode() == WXK_ESCAPE);
}
bool CompactFrame::IsPauseKey(const wxKeyEvent& event)
{
	return (event.GetKeyCode() == WXK_PAUSE);
}

bool CompactFrame::CloseThread()
{
	if (m_bThrPaused)
		ResumeThread();

	{
		wxCriticalSectionLocker enter(m_pThreadCS);
		if ( m_pThread )         // does the thread still exist?
		{
			if ( m_pThread->Delete() != wxTHREAD_NO_ERROR )
			{
				wxLogError("Can't delete the thread!");
				return false;
			}
		}
	}
	// exit from the critical section to give the thread
	// the possibility to enter its destructor
	// (which is guarded with m_pThreadCS critical section!)
	while ( 1 )
	{
		{ // was the ~MyThread() function executed?
			wxCriticalSectionLocker enter(m_pThreadCS);
			if ( !m_pThread ) break;
		}
		// wait for thread completion
		wxThread::This()->Sleep(1);
	}

	return true;
}

bool CompactFrame::ThreadActive() // true if 'm_pThread != NULL'
{
	bool bRet = false;

	wxCriticalSectionLocker enter(m_pThreadCS);
	bRet = (m_pThread != NULL);

	return bRet;
}

void CompactFrame::PauseThread()
{
	if (m_bThrPaused)
		return; // already Paused

	wxCriticalSectionLocker enter(m_pThreadCS);

	if ( m_pThread )         // does the thread still exist?
	{
		// without a critical section, once reached this point it may happen
		// that the OS scheduler gives control to the MyThread::Entry() function,
		// which in turn may return (because it completes its work) making
		// invalid the m_pThread pointer

		if ( m_pThread->Pause() != wxTHREAD_NO_ERROR )
			wxLogError("Can't pause the thread!");
		else
			m_bThrPaused = true;
	}
}

void CompactFrame::ResumeThread()
{
	if (!m_bThrPaused)
		return; // not Paused

	wxCriticalSectionLocker enter(m_pThreadCS);

	if ( m_pThread )         // does the thread still exist?
	{
		// without a critical section, once reached this point it may happen
		// that the OS scheduler gives control to the MyThread::Entry() function,
		// which in turn may return (because it completes its work) making
		// invalid the m_pThread pointer

		if ( m_pThread->Resume() != wxTHREAD_NO_ERROR )
			wxLogError("Can't resume the thread!");
		else
			m_bThrPaused = false;
	}
}

////// Events //////////////////////////////////////////////////////

void CompactFrame::OnClose(wxCloseEvent& event)
{
	if ( !CloseThread() )
	{
		event.Veto();
		return;
	}

	if ( m_DisableAllOthers )
	{
		delete m_DisableAllOthers;
		m_DisableAllOthers = NULL;
	}

	CompactEvent* c_event = new CompactEvent(CompactEvent::EventIDs::ID_exitCompacting);
	c_event->SetString(m_sSrcPath);
	wxQueueEvent(GetParent(), c_event);

	event.Skip();
}

void CompactFrame::OnCharHook(wxKeyEvent& event)
{
	if ( IsEscapeKey(event) )
	{
		if ( ThreadActive() )
		{
			PauseThread();
			if ( wxMessageBox(wxT("Do you really want to abort current process?"), wxT("Abort ?"), wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT, this) == wxYES )
				CloseThread(); // if was paused, will resume before trying to close
			else
				ResumeThread();
		}
		else
			Close();

		return;
	}
	else if ( IsPauseKey(event) )
	{
		if (m_bThrPaused)
			ResumeThread();
		else
			PauseThread();

		return;
	}
	event.Skip();
}

wxString CompactFrame::GetExitString() const
{
	return wxT("Press <Esc> to exit...");
}
void CompactFrame::OnThread(CompactEvent& event)
{
	switch ( event.GetId() )
	{
		case (int)CompactEvent::EventIDs::ID_information_Main:
		{
			if ( m_pCanvas && m_pCanvas->m_pStatTxt1 )
			{
				m_pCanvas->m_sStatTxt1 = event.GetString();
				m_pCanvas->m_pStatTxt1->TransferDataToWindow();
			}
			break;
		}
		case (int)CompactEvent::EventIDs::ID_information_Sec:
		{
			if ( m_pCanvas && m_pCanvas->m_pStatTxt2 )
			{
				m_pCanvas->m_sStatTxt2 = event.GetString();
				m_pCanvas->m_pStatTxt2->TransferDataToWindow();
			}
			break;
		}
		case (int)CompactEvent::EventIDs::ID_threadCompleted:
		{
			if ( event.GetInt() == 0 )
			{
				if ( m_pCanvas && m_pCanvas->m_pStatTxt1 )
				{
					m_pCanvas->m_sStatTxt1 = wxT("Finished successfully !");
					m_pCanvas->m_pStatTxt1->TransferDataToWindow();
				}
			}
			else
			{
				if ( m_pCanvas && m_pCanvas->m_pStatTxt1 )
				{
					wxString unknown = wxT("Unknow error");
					m_pCanvas->m_pStatTxt1->SetForegroundColour(lkCol_Red);
					if ( event.GetInt() == 1 )
					{
						wxFont f = m_pCanvas->m_pStatTxt1->GetFont();
						f = f.MakeBold();
						m_pCanvas->m_pStatTxt1->SetFont(f);
						if ( (m_pCanvas->m_sStatTxt1 = event.GetString()).IsEmpty() )
							m_pCanvas->m_sStatTxt1 = unknown;
					}
					else if ( event.GetInt() == 2 )
						m_pCanvas->m_sStatTxt1 = wxT("Canceled by user");
					else if ( event.GetInt() == 3 )
						m_pCanvas->m_sStatTxt1 = wxT("NewDB couldn't be saved (SourceDB might be locked)");
					else
						m_pCanvas->m_sStatTxt1 = unknown;

					m_pCanvas->m_pStatTxt1->TransferDataToWindow();
				}
			}

			if ( m_pCanvas && m_pCanvas->m_pStatTxt2 )
			{
				m_pCanvas->m_sStatTxt2 = GetExitString();
				m_pCanvas->m_pStatTxt2->TransferDataToWindow();
			}

			break;
		}
		case (int)CompactEvent::EventIDs::ID_progressInit:
		{
			lkProgressCtrl* progress = (m_pCanvas) ? ((event.GetInt() == 1) ? m_pCanvas->m_pProgress2 : m_pCanvas->m_pProgress1) : NULL;
			if ( progress )
			{
				progress->SetRange(0, (int)event.GetLongLong());
				progress->SetStep(1);
				progress->SetValue(0);
			}
			break;
		}
		case (int)CompactEvent::EventIDs::ID_progressStep:
		{
			if ( event.GetInt() == 1 )
			{
				if ( m_pCanvas && m_pCanvas->m_pProgress2 )
					m_pCanvas->m_pProgress2->StepIt();
			}
			else if ( event.GetInt() == 2 )
			{
				if ( m_pCanvas && m_pCanvas->m_pProgress1 )
					m_pCanvas->m_pProgress1->StepIt();
			}
			else
			{
				if ( m_pCanvas && m_pCanvas->m_pProgress1 )
					m_pCanvas->m_pProgress1->StepIt();
				if ( m_pCanvas && m_pCanvas->m_pProgress2 )
					m_pCanvas->m_pProgress2->StepIt();
			}
			break;
		}
		case (int)CompactEvent::EventIDs::ID_infoProcessing:
		{
			if ( m_pCanvas && m_pCanvas->m_pStatTxt1 )
			{
				m_pCanvas->m_sStatTxt1.Printf(wxT("Processing Requirements for [%s]"), event.GetString());
				m_pCanvas->m_pStatTxt1->TransferDataToWindow();
			}
			break;
		}
		case (int)CompactEvent::EventIDs::ID_infoRewriting:
		{
			if ( m_pCanvas && m_pCanvas->m_pStatTxt2 )
			{
				m_pCanvas->m_sStatTxt2.Printf(wxT("Rewriting table [%s]"), event.GetString());
				m_pCanvas->m_pStatTxt2->TransferDataToWindow();
			}
			break;
		}
	}
}

////// Thread //////////////////////////////////////////////////////

bool CompactFrame::DoCompact() // returning false if thread could not be started
{
	if ( ThreadActive() )
	{
		wxLogError("Thread allready active");
		return false;
	}

	// start thread in here
	m_pThread = new CompactThread(this, m_sSrcPath);
	if ( m_pThread->Run() != wxTHREAD_NO_ERROR )
	{
		if ( m_pCanvas && m_pCanvas->m_pStatTxt1 )
		{
			m_pCanvas->m_pStatTxt1->SetForegroundColour(lkCol_Red);
			wxFont f = m_pCanvas->m_pStatTxt1->GetFont();
			f = f.MakeBold();
			m_pCanvas->m_pStatTxt1->SetFont(f);
			m_pCanvas->m_sStatTxt1 = wxT("Can't create the thread!");
			m_pCanvas->m_pStatTxt1->TransferDataToWindow();
		}
		if ( m_pCanvas && m_pCanvas->m_pStatTxt2 )
		{
			m_pCanvas->m_sStatTxt2 = GetExitString();
			m_pCanvas->m_pStatTxt2->TransferDataToWindow();
		}

		delete m_pThread;
		m_pThread = NULL;
		return false;
	}

	// Thread up-and-running
	if ( !m_DisableAllOthers )
	{
		m_DisableAllOthers = new wxWindowDisabler(this);
	}
	return true;
}
