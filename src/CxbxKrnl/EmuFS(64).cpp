// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuFS.cpp
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include <cmath>
#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuShared.h"
#include "EmuFS.h"
#include "EmuLDT.h"
#include "EmuAlloc.h"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>
#include <cstdio>
#include <vector>

uint32 EmuAutoSleepRate = -1;

// Pointer to emulated FS register structure
xboxkrnl::KPCR* EmuFS;

// Base TLS Structure
Xbe::TLS *g_pTLS;
void *g_pTLSData;

// EThread vector
std::vector<xboxkrnl::ETHREAD*>  EThreads;

xboxkrnl::ETHREAD* EmuGenerateTLS();

// __declspec(naked)
// TODO: Fix this up a little 
void EmuSetupFSAccess()
{
	// Ptr to found thread
	xboxkrnl::ETHREAD* EThread;
	EThread = NULL;

	// Loop through threads
	for(uint32 i = 0; i < EThreads.size(); i++)
	{
		if(EThreads[i]->UniqueThread == GetCurrentThreadId())
		{
			EThread = EThreads[i];
		}
	}

	// If thread was not found, make one
	if(EThread == NULL)
	{
		EThread = EmuGenerateTLS();
	}

	// Set FS Values
	EmuFS->PrcbData.CurrentThread = (xboxkrnl::KTHREAD*)EThread;
	EmuFS->NtTib.StackBase = EThread->Tcb.TlsData; 
}

__declspec(naked) void EmuMEaxFs28()
{
	__asm
	{	
		push EBX 
		push ECX 
		push EDX
	}
	EmuSetupFSAccess();

	__asm
	{
		mov eax, EmuFS
		mov eax, [eax+28h]

		pop EDX
		pop ECX
		pop EBX
		
		ret
	}
}

__declspec(naked) void EmuMEaxFs20()
{
	__asm
	{	
		push EBX 
		push ECX 
		push EDX
	}

	EmuSetupFSAccess();
	__asm
	{
		//mov eax, dword ptr fs:[0x20]
		mov eax, EmuFS
		mov eax, [eax+20h]

		pop EDX
		pop ECX
		pop EBX

		ret
	}
}
__declspec(naked) void EmuMEcxFs04()
{
	__asm
	{
		push EAX
		push EBX 
		push EDX
	}

	EmuSetupFSAccess();
	__asm
	{
		//mov ecx, dword ptr fs:[0x04]
		mov ecx, EmuFS
		mov ecx, [ecx+04h]

		pop EDX
		pop EBX
		pop EAX

		ret
	}
}

__declspec(naked) void EmuMEdiFs04()
{
	__asm
	{
		push EAX
		push EBX 
		push ECX 
		push EDX
	}

	EmuSetupFSAccess();
	__asm
	{
		mov edi, EmuFS
		mov edi, [edi+04h]

		pop EDX
		pop ECX
		pop EBX
		pop EAX

		ret
	}
}

uint08 EmuMzEaxFS24Val;
__declspec(naked) void EmuMZEaxFs24()
{
	__asm
	{
		push EBX 
		push ECX 
		push EDX
	}

	EmuSetupFSAccess();
	EmuMzEaxFS24Val = EmuFS->Irql;
	__asm
	{
		movzx eax, EmuMzEaxFS24Val

		pop EDX
		pop ECX
		pop EBX

		ret
	}
}

xboxkrnl::ETHREAD* EmuGenerateTLS()
{
	uint08* pNewTLS = NULL;

    // copy global TLS to the current thread
    {
        uint32 dwCopySize = g_pTLS->dwDataEndAddr - g_pTLS->dwDataStartAddr;
        uint32 dwZeroSize = g_pTLS->dwSizeofZeroFill;

        pNewTLS = (uint08*)CxbxMalloc(dwCopySize + dwZeroSize + 0x100 /* + HACK: extra safety padding 0x100*/);

        memset(pNewTLS, 0, dwCopySize + dwZeroSize + 0x100);
        memcpy(pNewTLS, g_pTLSData, dwCopySize);
    }

    // prepare TLS
    {
        // dword @ pTLSData := pTLSData
        if(pNewTLS != 0)
            *(void**)pNewTLS = pNewTLS;
    }

	// Generate EThread structure
	xboxkrnl::ETHREAD* EThread = (xboxkrnl::ETHREAD*)CxbxMalloc(sizeof(xboxkrnl::ETHREAD));
    EThread->Tcb.TlsData  = (void*)pNewTLS;
    EThread->UniqueThread = GetCurrentThreadId();

	// Add to EThread vector
	EThreads.push_back(EThread);

	// Return pointer
	return EThread;
}

// generate fs segment selector
void EmuGenerateFS(Xbe::TLS *pTLS, void *pTLSData)
{
	NT_TIB         *OrgNtTib;

    __asm
    {
		push eax
        // Obtain "OrgNtTib"
        mov eax, fs:[0x18]
		mov OrgNtTib, eax
		pop eax
	}

	// Generate KPCR structure at EmuFS
	uint32 dwSize = sizeof(xboxkrnl::KPCR);
	EmuFS = (xboxkrnl::KPCR*)CxbxMalloc(dwSize);
    memset(EmuFS, 0, sizeof(*EmuFS));

    memcpy(&EmuFS->NtTib, OrgNtTib, sizeof(NT_TIB));
	EmuFS->NtTib.Self = &EmuFS->NtTib;
	EmuFS->Prcb = &EmuFS->PrcbData;

	// Setup global TLS
	g_pTLS = pTLS;
	g_pTLSData = pTLSData;

	// Create default EThread
	EmuGenerateTLS();

	// Perform Patches to FS register accesses
	DbgPrintf("EmuFS (0x%X): Searching for FS register usage..\n", GetCurrentThreadId());

	for(uint32 addr = (uint32)g_XbeBuffer; addr < (uint32)g_XbeBuffer + g_XbeHeader->dwSizeofImage; addr++)
	{
		uint32 realAddr = addr - (uint32)g_XbeBuffer + g_XbeHeader->dwBaseAddr;
		switch(*(uint32*)addr)
		{
		case 0x0028a164:
			DbgPrintf("EmuFS: 0x%X = Found 'mov eax, large fs:28h'\n", addr - (uint32)g_XbeBuffer + g_XbeHeader->dwBaseAddr);
			// Call 
			*(uint08*)addr = 0xE8;
			*(uint32*)(addr + 1) = (uint32)EmuMEaxFs28 - realAddr - 5;
			// Ret
			*(uint08*)(addr + 5) = 0x90;
			break;
		case 0x0020a164:
			DbgPrintf("EmuFS: 0x%X = 'Found mov eax, large fs:20h'\n", addr - (uint32)g_XbeBuffer + g_XbeHeader->dwBaseAddr);
			// Call 
			*(uint08*)addr = 0xE8;
			*(uint32*)(addr + 1) = (uint32)EmuMEaxFs20 - realAddr - 5;
			// Ret
			*(uint08*)(addr + 5) = 0x90;
			break;
		case 0x040d8b64:
			DbgPrintf("EmuFS: 0x%X = 'Found mov ecx, large fs:4h'\n", addr - (uint32)g_XbeBuffer + g_XbeHeader->dwBaseAddr);
			// Call 
			*(uint08*)addr = 0xE8;
			*(uint32*)(addr + 1) = (uint32)EmuMEcxFs04 - realAddr - 5;
			// Ret
			*(uint08*)(addr + 5) = 0x90;
			*(uint08*)(addr + 6) = 0x90;
			break;
		case 0x043d8b64:
			DbgPrintf("EmuFS: 0x%X = 'Found mov edi, large fs:4h'\n", addr - (uint32)g_XbeBuffer + g_XbeHeader->dwBaseAddr);
			// Call 
			*(uint08*)addr = 0xE8;
			*(uint32*)(addr + 1) = (uint32)EmuMEdiFs04 - realAddr - 5;
			// Ret
			*(uint08*)(addr + 5) = 0x90;
			*(uint08*)(addr + 6) = 0x90;
			break;
		case 0x05b60f64:
			DbgPrintf("EmuFS: 0x%X = 'Found movzx eax, large byte ptr fs:24h'\n", addr - (uint32)g_XbeBuffer + g_XbeHeader->dwBaseAddr);
			// Call 
			*(uint08*)addr = 0xE8;
			*(uint32*)(addr + 1) = (uint32)EmuMZEaxFs24 - realAddr - 5;
			// Ret
			*(uint08*)(addr + 5) = 0x90;
			*(uint08*)(addr + 6) = 0x90;
			*(uint08*)(addr + 7) = 0x90;
			break;
		}
	}
}

	