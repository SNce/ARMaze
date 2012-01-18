 /* ========================================================================
  * Copyright (C) 2001  John Adcock (http://deinterlace.sourceforge.net)
  * Modified by Thomas Pintaric (pintaric@ims.tuwien.ac.at)
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

#ifndef __CPU_H__
#define __CPU_H__

// Symbolic constants for CpuFeatureFlags TRB 12/00
#define FEATURE_CPUID           0x00000001
#define FEATURE_STD_FEATURES    0x00000002
#define FEATURE_EXT_FEATURES    0x00000004
#define FEATURE_TSC             0x00000010
#define FEATURE_MMX             0x00000020
#define FEATURE_CMOV            0x00000040
#define FEATURE_3DNOW           0x00000080
#define FEATURE_3DNOWEXT        0x00000100
#define FEATURE_MMXEXT          0x00000200
#define FEATURE_SSEFP           0x00000400
#define FEATURE_K6_MTRR         0x00000800
#define FEATURE_P6_MTRR         0x00001000
#define FEATURE_SSE             0x00002000
#define FEATURE_SSE2            0x00004000

void  CPU_SetupFeatureFlag(void);
extern UINT CpuFeatureFlags;        // TRB 12/20/00 Processor capability flags

#endif
