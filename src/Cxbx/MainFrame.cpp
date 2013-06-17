// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *
// *  This file is part of the Cxbx-Reloaded project.
// *
// *  Cxbx-Reloaded and it's components are free software; you can 
// *  redistribute them and/or modify them under the terms of the 
// *  GNU General Public License as published by the Free Software 
// *  Foundation; either version 2 of the license, or (at your option)
// *  any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  Cxbx-Reloaded
// *  (c) 2013 Luke Usher <luke.usher@xohho.co.uk>
// *
// *  Loosely based on the Cxbx project
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// ******************************************************************

#include "MainFrame.h"

// Declare Event IDs
enum
{
    ID_FILE_OPEN = 1,
	ID_FILE_CLOSE,
    ID_FILE_EXIT,

	ID_CONFIG_CORE,
	ID_CONFIG_PLUGINS,

	ID_EMU_START,
	ID_EMU_STOP,

	ID_HELP_HOMEPAGE,
	ID_HELP_ABOUT,
};

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	// Menu Events

	EVT_MENU(ID_FILE_OPEN, MainFrame::OnFileOpen)
	EVT_MENU(ID_FILE_CLOSE, MainFrame::OnFileClose)
	EVT_MENU(ID_FILE_EXIT, MainFrame::OnFileExit)
	EVT_MENU(ID_CONFIG_CORE, MainFrame::OnConfigCore)
	EVT_MENU(ID_CONFIG_PLUGINS, MainFrame::OnConfigPlugins)
	EVT_MENU(ID_HELP_HOMEPAGE, MainFrame::OnHelpHomepage)
	EVT_MENU(ID_HELP_ABOUT, MainFrame::OnHelpAbout)

	// Window Events
	EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()


MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, -1, title, pos, size, wxMINIMIZE_BOX | wxSYSTEM_MENU |wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN)
{
	// Set application icon
	SetIcon(wxIcon("mainicon"));

	// Generate File Menu
    wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_FILE_OPEN, _("&Open XBE"));
	menuFile->Append(ID_FILE_CLOSE, _("&Close XBE"));
	menuFile->AppendSeparator();
    menuFile->Append(ID_FILE_EXIT, _("E&xit"));

	// Generate Config Menu
	wxMenu *menuConfig = new wxMenu;
	menuConfig->Append(ID_CONFIG_CORE, _("&Core"));
	menuConfig->Append(ID_CONFIG_PLUGINS, _("&Plugins"));
	
	// Generate Emulation Menu
	wxMenu* menuEmu = new wxMenu;
	menuEmu->Append(ID_EMU_START, _("&Start"));
	menuEmu->Append(ID_EMU_STOP, _("Sto&p"));

	// Generate Help Menu
	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(ID_HELP_HOMEPAGE, _("&Home Page"));
	menuHelp->AppendSeparator();
	menuHelp->Append(ID_HELP_ABOUT, _("&About"));

	// Add Menus to menu bar
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _("&File"));
	menuBar->Append(menuConfig, _("&Config"));
	menuBar->Append(menuEmu, _("&Emulation"));
	menuBar->Append(menuHelp, _("&Help"));

	// Attach menu bar
    SetMenuBar(menuBar);

	// Disable unusable menu options
	menuBar->Enable(ID_FILE_CLOSE, false);
	menuBar->Enable(ID_EMU_STOP, false);

}

void MainFrame::OnFileOpen(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, 
								_("Open XBE file"), 
								"", 
								"",
								"XBE files (*.xbe)|*.xbe", 
								wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
	{
		return;
	}
	else
	{

	}
}

void MainFrame::OnFileClose(wxCommandEvent& event)
{

}

void MainFrame::OnFileExit(wxCommandEvent& event)
{
	Close();
}

void MainFrame::OnConfigCore(wxCommandEvent& event)
{

}

void MainFrame::OnConfigPlugins(wxCommandEvent& event)
{

}

void MainFrame::OnHelpHomepage(wxCommandEvent& event)
{
	wxLaunchDefaultBrowser("http://github.com/SoullessSentinel/Cxbx-Reloaded");
}

void MainFrame::OnHelpAbout(wxCommandEvent& event)
{
	wxMessageBox("Cxbx-Reloaded is a new Xbox emulator loosely based on the Cxbx project", "About Cxbx-Reloaded");
}

void MainFrame::OnClose(wxCloseEvent& event)
{
	// TODO: Only ask for confirmation if emulator is running
    if (event.CanVeto())
    {
        if (wxMessageBox("Are you sure you wish to quit?", "Confirm", wxICON_QUESTION | wxYES_NO) != wxYES )
        {
            event.Veto();
            return;
        }
    }
	// TODO: Kill emulator proccess (Either HLEStub or LLEStub depending on settings
    Destroy(); 
}