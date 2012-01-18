 /* ========================================================================
  * Copyright (C) 2000  John Adcock (http://deinterlace.sourceforge.net)
  * Refinements made by Mark Rejhon and Steve Grimm
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 2
  * of the License, or (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  *
  * ========================================================================*/

#include "MemCopy.h"

#pragma warning(disable : 4799)
// warning C4799: No EMMS at end of function '...'

/////////////////////////////////////////////////////////////////////////////
// memcpyMMX
// Uses MMX instructions to move memory around
// does as much as we can in 64 byte chunks (128-byte on SSE machines)
// using MMX instructions
// then copies any extra bytes
// assumes there will be at least 64 bytes to copy
// This code was originally from Borg's bTV plugin SDK 
/////////////////////////////////////////////////////////////////////////////
void __cdecl memcpyMMX(void *Dest, void *Src, size_t nBytes)
{
    __asm 
    {
        mov     esi, dword ptr[Src]
        mov     edi, dword ptr[Dest]
        mov     ecx, nBytes
        shr     ecx, 6                      // nBytes / 64
align 8
CopyLoop:
        movq    mm0, qword ptr[esi]
        movq    mm1, qword ptr[esi+8*1]
        movq    mm2, qword ptr[esi+8*2]
        movq    mm3, qword ptr[esi+8*3]
        movq    mm4, qword ptr[esi+8*4]
        movq    mm5, qword ptr[esi+8*5]
        movq    mm6, qword ptr[esi+8*6]
        movq    mm7, qword ptr[esi+8*7]
        movq    qword ptr[edi], mm0
        movq    qword ptr[edi+8*1], mm1
        movq    qword ptr[edi+8*2], mm2
        movq    qword ptr[edi+8*3], mm3
        movq    qword ptr[edi+8*4], mm4
        movq    qword ptr[edi+8*5], mm5
        movq    qword ptr[edi+8*6], mm6
        movq    qword ptr[edi+8*7], mm7
        add     esi, 64
        add     edi, 64
        loop CopyLoop
        mov     ecx, nBytes
        and     ecx, 63
        cmp     ecx, 0
        je EndCopyLoop
align 8
CopyLoop2:
        mov dl, byte ptr[esi] 
        mov byte ptr[edi], dl
        inc esi
        inc edi
        dec ecx
        jne CopyLoop2
EndCopyLoop:
    }
}

/////////////////////////////////////////////////////////////////////////////
// memcpySSE
// On SSE machines, we can use the 128-bit floating-point registers and
// bypass write caching to copy a bit faster.  The destination has to be
// 16-byte aligned.  
/////////////////////////////////////////////////////////////////////////////
void __cdecl memcpySSE(void *Dest, void *Src, size_t nBytes)
{
    __asm 
    {
        mov     esi, dword ptr[Src]
        mov     edi, dword ptr[Dest]
        mov     ecx, nBytes
        shr     ecx, 7                      // nBytes / 128
align 8
CopyLoopSSE:
        // movaps should be slightly more efficient
        // as the data is 16 bit aligned
        movaps  xmm0, xmmword ptr[esi]
        movaps  xmm1, xmmword ptr[esi+16*1]
        movaps  xmm2, xmmword ptr[esi+16*2]
        movaps  xmm3, xmmword ptr[esi+16*3]
        movaps  xmm4, xmmword ptr[esi+16*4]
        movaps  xmm5, xmmword ptr[esi+16*5]
        movaps  xmm6, xmmword ptr[esi+16*6]
        movaps  xmm7, xmmword ptr[esi+16*7]
        movntps xmmword ptr[edi], xmm0
        movntps xmmword ptr[edi+16*1], xmm1
        movntps xmmword ptr[edi+16*2], xmm2
        movntps xmmword ptr[edi+16*3], xmm3
        movntps xmmword ptr[edi+16*4], xmm4
        movntps xmmword ptr[edi+16*5], xmm5
        movntps xmmword ptr[edi+16*6], xmm6
        movntps xmmword ptr[edi+16*7], xmm7
        add     esi, 128
        add     edi, 128
        loop CopyLoopSSE
        mov     ecx, nBytes
        and     ecx, 127
        cmp     ecx, 0
        je EndCopyLoopSSE
align 8
CopyLoop2SSE:
        mov dl, byte ptr[esi] 
        mov byte ptr[edi], dl
        inc esi
        inc edi
        dec ecx
        jne CopyLoop2SSE
EndCopyLoopSSE:
    }
}
