#ifndef _LOG_H_
#define _LOG_H_
#pragma once

//----------------------------------------------------------//
// LOG.H
//----------------------------------------------------------//
//-- Description			
// Creates a log file for writing out useful information.
//----------------------------------------------------------//


#include "Types.h"
#include "SysString.h"
#include "File/FileData.h"
#include "File/FileDirectTextWriter.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define LOG_WORKING_BUFFER_SIZE				(16 * 1024)
#define LOG_MARKUP_HISTORY_SIZE				(64)

#define SCOPED_LOG_INDENT(x)				CScopedLogIndent scopedLogIndent(&(x)) 


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CLog
{
	public:

		CLog();
		~CLog();

		bool										Initialise(const s8* strFileName);						
		bool										Shutdown(void);					 
		
		void										Printf(const s8* strText, ...);
		void										AddTimeStamp(void);
		void										AddBreakLine(void);
		void										AddBlankLine(void);

		void										PushIndent(void);
		void										PopIndent(void);

	private:

		s8											PushMarkup(s8 cMarkup);
		s8											PopMarkup(void);
		s8											MultiPopMarkup(s8 cMarkup);
		s8											GetCurrentMarkup(void) const;

		void										StartFormatted(s8 cMarkup, const s8* strText);
		void										EndFormatted(s8 cMarkup, const s8* strText);
		void										PrintFormatted(const s8* strText);

		void										PrintLineFront(s32 nBack = 0);
		
		FixedString<LOG_WORKING_BUFFER_SIZE>		m_strWorkingBuffer;

		CFileData									m_FileData;
		CFileAccessorDirectTextWriter				m_FileAccessor;
		CFileProcessorDirectTextWriter				m_FileProcessor;
	
		s8											m_strMarkupHistory[LOG_MARKUP_HISTORY_SIZE];
		u32											m_nMarkupHistoryIndex;

		u32											m_nNumericListCounter;

		time_t										m_nStartTime;
};


class CScopedLogIndent
{
	private:
		CLog*										m_pLog;

	public:
		CScopedLogIndent(CLog* pLog)
		{
			m_pLog = pLog;
			m_pLog->PushIndent();
		}

		~CScopedLogIndent()
		{
			m_pLog->PopIndent();
		}
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_LOG_H_