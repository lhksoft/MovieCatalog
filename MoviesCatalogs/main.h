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
/**************************************************************************************
* \file         main.h
* \brief        Application header file.
*
****************************************************************************************/
#ifndef MAIN_H
#define MAIN_H
#pragma once

/****************************************************************************************
* Include files
****************************************************************************************/
#include <wx/app.h>

#include "MainView.h"

/****************************************************************************************
* Class definitions
****************************************************************************************/
/** \brief Application class. */
class MyApp : public wxApp
{
public:
    MyApp();

    wxFrame*                CreateChildFrame(wxView* view);

    // override some wxApp virtual methods
    virtual bool            OnInit() wxOVERRIDE;
    virtual int             OnExit() wxOVERRIDE;

    MainCanvas*           GetMainCanvas(void);

protected:
    // override base class method to use GUI traits
    virtual wxAppTraits*    CreateTraits() wxOVERRIDE;

private:
    MainCanvas*           m_pCanvas;
};

wxDECLARE_APP(MyApp);

#endif /* MAIN_H */
/*********************************** end of main.h *************************************/

