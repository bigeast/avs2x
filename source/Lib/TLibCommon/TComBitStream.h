/* ====================================================================================================================

  The copyright in this software is being made available under the License included below.
  This software may be subject to other third party and contributor rights, including patent rights, and no such
  rights are granted under this license.

  Copyright (c) 2012, SAMSUNG ELECTRONICS CO., LTD. All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted only for
  the purpose of developing standards within Audio and Video Coding Standard Workgroup of China (AVS) and for testing and
  promoting such standards. The following conditions are required to be met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
      the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
      the following disclaimer in the documentation and/or other materials provided with the distribution.
    * The name of SAMSUNG ELECTRONICS CO., LTD. may not be used to endorse or promote products derived from 
      this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 * ====================================================================================================================
*/

/** \file     TComBitStream.h
    \brief    class for handling bitstream (header)
*/

#ifndef __COMBITSTREAM__
#define __COMBITSTREAM__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <assert.h>
#include "CommonDef.h"

// ====================================================================================================================
// Class definition
// ====================================================================================================================

/// pure virtual class for basic bit handling
class TComBitIf
{
public:
  virtual Void        writeAlignOne         () {};
  virtual Void        write                 ( UInt uiBits, UInt uiNumberOfBits )  = 0;
  virtual Void        resetBits             ()                                    = 0;
  virtual UInt        getNumberOfWrittenBits()                                    = 0;
#if YQH_AEC_RDO_BUG
  virtual   UInt      getCodeBits() = 0;
  virtual  UInt      getNumberOfWrittenBytes() = 0;
#endif
#if wlq_AEC
  virtual Void put_one_bit_plus_outstanding(UInt uiBits,UInt*Ebits_to_follow)=0;
  virtual Void  flushBuffer()                                    = 0;
#endif
  
  virtual ~TComBitIf() {}
};

/// class for handling bitstream
class TComBitstream : public TComBitIf
{
protected:
  UInt*       m_apulStreamPacketBegin;
  UInt*       m_pulStreamPacket;
  UInt        m_uiBufSize;
  
  UInt        m_uiBitSize;
  Int         m_iValidBits;
  
  UInt        m_ulCurrentBits;
  UInt        m_uiBitsWritten;
#if YQH_AVS2_BITSTREAM
  Bool        b_first_bit;
#endif  
  UInt        m_uiDWordsLeft;
  UInt        m_uiBitsLeft;
  UInt        m_uiNextBits;
#if YQH_AEC_RDO_BUG
  UInt  m_uiCodeBits;
  UInt m_uiByteCounter;
#endif  
  UInt xSwap ( UInt ui )
  {
    // heiko.schwarz@hhi.fhg.de: support for BSD systems as proposed by Steffen Kamp [kamp@ient.rwth-aachen.de]
#ifdef MSYS_BIG_ENDIAN
    return ui;
#else
    UInt ul2;
    
    ul2  = ui>>24;
    ul2 |= (ui>>8) & 0x0000ff00;
    ul2 |= (ui<<8) & 0x00ff0000;
    ul2 |= ui<<24;
    
    return ul2;
#endif
  }
  
  // read one word
  __inline Void xReadNextWord ();
  
public:
  TComBitstream()             {}
  virtual ~TComBitstream()    {}
  
  // create / destroy
  Void        create          ( UInt uiSizeInBytes );
  Void        destroy         ();
  
  // interface for encoding
#if wlq_AEC
  Void put_one_bit_plus_outstanding(UInt uiBits,UInt*Ebits_to_follow); 
#endif
  Void        write           ( UInt uiBits, UInt uiNumberOfBits );
  Void        writeAlignOne   ();
  Void        writeAlignZero  ();
  Void        convertRBSPToPayload( UInt uiStartPos = 0);
  // interface for decoding
  Void        initParsingConvertPayloadToRBSP( const UInt uiBytesRead );
  Void        initParsing     ( UInt uiNumBytes );
  Void        read            ( UInt uiNumberOfBits, UInt& ruiBits );
#if AVS3_EXTENSION_DEBUG_SYC
  Void        nextnbit    ( UInt uiNumberOfBits, UInt& ruiBits );
#endif
  Void        readAlignOne    ();
  
  // reset internal status
  Void        resetBits       ()
  {
    m_uiBitSize = 0;
    m_iValidBits = 32;
    m_ulCurrentBits = 0;
    m_uiBitsWritten = 0;
#if YQH_AEC_RDO_BUG
	m_uiCodeBits = 9;
	m_uiByteCounter = 9;
#endif
  }
  
  // utility functions
  UInt*       getStartStream()          { return m_apulStreamPacketBegin;               }
  UInt*       getBuffer()               { return  m_pulStreamPacket;                    }
  Int         getBitsUntilByteAligned() { return m_iValidBits & (0x7);                  }
  Void        setModeSbac()             { m_uiBitsLeft = 8*((m_uiBitsLeft+7)/8);        } // stop bit + trailing stuffing bits
  Bool        isWordAligned()           { return  (0 == (m_iValidBits & (0x1f)));       }
  UInt        getNumberOfWrittenBits()  { return  m_uiBitsWritten;                      }
#if YQH_AEC_RDO_BUG
  UInt      getCodeBits()                                    { return m_uiCodeBits; }
  UInt      getNumberOfWrittenBytes()                                    { return m_uiByteCounter; }
#endif
  Void        flushBuffer();
  Void        rewindStreamPacket()      { m_pulStreamPacket = m_apulStreamPacketBegin;  }
  UInt        getBitsLeft()             { return  m_uiBitsLeft;                         }
};

#endif

