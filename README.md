Cxbx-Reloaded
=============

A branch of the Cxbx source code with support for 64-bit operating systems. 
This branch has been renamed to Cxbx-Reloaded and started from version 0.0.1 as I plan to completely rewrite large portions of the emulator.

Main Features:
- 64-bit support!
- Able to boot and get ingame in Turok Evolution, and Futurama.  
- May work with other titles, I have not yet tested much outside of these.

Differences from mainline Cxbx:
- Corrected generation of SizeOfImage field in converted executables (Fixes 0xC000007b on Windows 64-bit)
- Partial emulation of the FS register without using the LDT (Allows Xbox games to execute on Windows 64-bit) 

Known Issues:
- Included project file only known to compile with Visual Studio 2010, can be compiled with other versions by using project files from a different branch.
- Code to work around enhanced memory protection in 64-bit assumes executable is named "default.exe" and will break on games with multiple XBE files.
- FS register emulation is incomplete, only essential FS register accesses are hooked.
- Debug builds have very low compatibility, release builds are more compatible, try to avoid running games in Debug mode
- All titles crash if console output is enabled, disabling it allows games to boot.

