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
#include "lkGUIAppTraits.h"
#include <wx/app.h>
#include <wx/filename.h>
//#include <wx/fileconf.h>
#include "../lkControls/lkFileConfig.h"

class lkStandardPaths : public wxStandardPaths
{
protected:
	lkStandardPaths();

public:
	// return the directory for the documents files used by this application:
	// it's a subdirectory of GetDocumentsDir() constructed using the
	// application name/vendor if it NOT exists it will be created
	virtual wxString						GetAppDocumentsDir() const wxOVERRIDE;

private:
	wxDECLARE_NO_COPY_CLASS(lkStandardPaths);
};

lkStandardPaths::lkStandardPaths() : wxStandardPaths()
{
}

wxString lkStandardPaths::GetAppDocumentsDir() const
{
	const wxString docsDir = GetDocumentsDir();
	wxString appDocsDir = AppendAppInfo(docsDir);

	if ( !wxDirExists(appDocsDir) )
		wxFileName::Mkdir(appDocsDir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

	return wxDirExists(appDocsDir) ? appDocsDir : docsDir;
}


namespace
{

	// Derive a class just to be able to create it: wxStandardPaths ctor is
	// protected to prevent its misuse, but it also means we can't create an object
	// of this class directly.
	class lkStandardPathsDefault : public lkStandardPaths
	{
	public:
		lkStandardPathsDefault()
		{
		}
	};

	static lkStandardPathsDefault lkgs_stdPaths;

} // anonymous namespace



wxConfigBase* lkGUIAppTraits::CreateConfig()
{
	return new lkFileConfig(wxTheApp->GetAppName(), wxTheApp->GetVendorName(), wxEmptyString, wxEmptyString, wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_SUBDIR);
}

wxStandardPaths& lkGUIAppTraits::GetStandardPaths()
{
	return lkgs_stdPaths;
}

