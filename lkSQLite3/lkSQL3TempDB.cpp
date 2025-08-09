/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#include "lkSQL3TempDB.h"
#include <wx/filename.h>
#include <wx/filefn.h> 
#include "lkSQL3Exception.h"
#include <wx/log.h>

lkSQL3TempDB::lkSQL3TempDB() : lkSQL3Database(), m_strTemp(), m_strPath()
{
}
lkSQL3TempDB::~lkSQL3TempDB()
{
	Discard();
}

bool lkSQL3TempDB::OpenAsTemp(const wxString& dbPath)
{
	// we must have an absolute filename because otherwise CreateTempFileName()
	// would create the temp file in $TMP (i.e. the system standard location
	// for the temp files) which might be on another volume/drive/mount and
	// wxRename()ing it later to m_strName from Commit() would then fail
	//
	// with the absolute filename, the temp file is created in the same
	// directory as this one which ensures that wxRename() may work later
	wxFileName fn(dbPath);
	if ( !fn.IsAbsolute() )
	{
		fn.Normalize(wxPATH_NORM_ABSOLUTE);
	}

	m_strPath = fn.GetFullPath();
	m_strTemp = wxFileName::CreateTempFileName(m_strPath, (wxFile*)NULL);

	if ( m_strTemp.IsEmpty() )
	{
		// CreateTempFileName() failed
		m_strPath = wxT("");
		return false;
	}

	try
	{
		lkSQL3Database::Open(m_strTemp, lkSQL3Database::ofCreate);
	}
	catch ( const lkSQL3Exception& )
	{
		// should not throw because we'r also called from our constructor
//		((lkSQL3Exception*)&e)->ReportError();
		Close(); // close is required even in case of error on opening
		m_strPath = wxT("");
		m_strTemp = wxT("");
		wxLogSysError(GetErrorMsg());
		throw;
		return false;
	}

	Close();

#ifdef __UNIX__
	// the temp file should have the same permissions as the original one
	mode_t mode;

	wxStructStat st;
	if ( stat((const char*)m_strPath.fn_str(), &st) == 0 )
	{
		mode = st.st_mode;
	}
	else
	{
		// file probably didn't exist, just give it the default mode _using_
		// user's umask (new files creation should respect umask)
		mode_t mask = umask(0777);
		mode = 0666 & ~mask;
		umask(mask);
	}

	if ( chmod((const char*)m_strTemp.fn_str(), mode) == -1 )
	{
		wxLogSysError(wxT("Failed to set temporary file permissions"));
	}
#endif // Unix

	try
	{
		lkSQL3Database::Open(m_strTemp);
	}
	catch ( const lkSQL3Exception& )
	{
		// should not throw because we'r also called from our constructor
//		((lkSQL3Exception*)&e)->ReportError();
		Close(); // close is required even in case of error on opening
		m_strPath = wxT("");
		m_strTemp = wxT("");
		wxLogSysError(GetErrorMsg());
		throw;
		return false;
	}

	return true;
}

bool lkSQL3TempDB::Commit()
{
	if ( !m_strTemp.IsEmpty() && wxFile::Exists(m_strTemp)) // cause if the file doesn't exist (for some reason), don't remove the original!
	{
		Close();
		if ( wxFile::Exists(m_strPath) && wxRemove(m_strPath) != 0 )
		{
			wxLogSysError(wxT("can't remove file '%s'"), m_strPath.c_str());
			return false;
		}

		if ( !wxRenameFile(m_strTemp, m_strPath) )
		{
			wxLogSysError(wxT("can't commit changes to file '%s'"), m_strPath.c_str());
			return false;
		}

		m_strTemp = wxT("");
		m_strPath = wxT("");
		return true;
	}
	return false;
}

void lkSQL3TempDB::Discard()
{
	if ( !m_strTemp.IsEmpty() )
	{
		Close();
		if ( wxRemove(m_strTemp) != 0 )
		{
			wxLogSysError(wxT("can't remove temporary file '%s'"), m_strTemp.c_str());
		}
	}
	m_strTemp = wxT("");
	m_strPath = wxT("");
}

