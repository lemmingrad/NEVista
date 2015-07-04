//----------------------------------------------------------//
// LOG.CPP
//----------------------------------------------------------//
//-- Description			
// Creates a log file for writing out useful information.
//----------------------------------------------------------//


#include "Log.h"
#include "Types.h"
#include "SysTime.h"
#include "SysString.h"
#include "File/FileData.h"
#include "File/FileDirectTextWriter.h"
#include "Win32/Game.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLog::CLog
//----------------------------------------------------------//
//-- Description			
// Constructor for log object.
//----------------------------------------------------------//
CLog::CLog()
: m_FileData("", CFileData::Type::Unknown, CFileData::AccessMethod::Unknown)
, m_FileAccessor(NULL)
, m_FileProcessor(NULL)
, m_nMarkupHistoryIndex(0)
, m_nNumericListCounter(0)
{
	m_strMarkupHistory[0] = '\0';
	m_strWorkingBuffer.Clear();
}


//----------------------------------------------------------//
// CLog::~CLog
//----------------------------------------------------------//
//-- Description			
// Destructor for log object.
//----------------------------------------------------------//
CLog::~CLog()
{
	Shutdown();
}


//----------------------------------------------------------//
// CLog::Initialise
//----------------------------------------------------------//
//-- Description			
// Opens a log file.
// Returns true if file was opened, otherwise false;
//----------------------------------------------------------//
bool CLog::Initialise(const s8* strFileName)	
{
	//-- The Debug Log is a rare case where we use a 
	//-- CFile-type that hasn't been added to the File Manager.
	//-- 2 reasons:
	//-- 1: Because the File Manager may or may not be initialised 
	//-- before the log is created or required for the first time
	//-- (likely if the filemanager wants to write to the log itself!)
	//-- 2: We want fine control over when to flush it.
	//-- 
	//-- For now we'll use a DirectTextWriter, but in future we may
	//-- choose to use a buffered writer for performance, with a
	//-- manual processor flush.
	
	m_nMarkupHistoryIndex = 0;
	m_nNumericListCounter = 0;

	m_FileData = CFileData(strFileName, CFileData::Type::Text, CFileData::AccessMethod::DirectWrite);
	m_FileAccessor = CFileAccessorDirectTextWriter(&m_FileData);
	m_FileProcessor = CFileProcessorDirectTextWriter(&m_FileData);

	if (CFileProcessor::Error::Ok != m_FileProcessor.Open())
	{
		return false;
	}

	time(&m_nStartTime);
	FixedString<SYSTIME_BUFFER_SIZE> strTime;
	SysTime::Ctime(strTime.Buffer(), strTime.Size(), &m_nStartTime);

	m_FileAccessor.Printf("%s ver %s log created %s\n", Game_Title(), Game_Version(), strTime.ConstBuffer());

	return true;
}


//----------------------------------------------------------//
// CLog::Shutdown
//----------------------------------------------------------//
//-- Description			
// Closes an open log file. 
//----------------------------------------------------------//
bool CLog::Shutdown(void)
{
	time_t nLogTime;
	time(&nLogTime);
	FixedString<SYSTIME_BUFFER_SIZE> strTime;
	SysTime::Ctime(strTime.Buffer(), strTime.Size(), &nLogTime);

	if (IS_TRUE(m_FileAccessor.IsOpen()))
	{
		m_FileAccessor.Printf("\n\nLog closed %s", strTime.ConstBuffer());
		m_FileProcessor.Close();
	}

	return true;
}


//----------------------------------------------------------//
// CLog::PushMarkup
//----------------------------------------------------------//
//-- Description			
// Adds a new markup block to the history.
// Valid Markup:
// >	Indent.
// [	Start a Bracket block.
// {	Start a Curly block.
// !	Start a Warning block.
// *	Start a '*' list block.
// #	Start a numbered list block.
//
// |	End current markup.
// <	Roll back to last Indent.
// ]	Roll back to last Bracket block.
// }	Roll back to last Curly block.
// =	End all markups.
//
// \\ Ignore first character. That means the printed string
//		will begin at the second character, and if the second
//		character is markup, it will be printed instead.
//		eg: "\\|" will be printed as "|hi" with no markup.
//----------------------------------------------------------//
s8 CLog::PushMarkup(s8 cMarkup)
{
	if (m_nMarkupHistoryIndex < (LOG_MARKUP_HISTORY_SIZE - 2))
	{
		m_strMarkupHistory[m_nMarkupHistoryIndex++] = cMarkup;
		m_strMarkupHistory[m_nMarkupHistoryIndex] = '\0';

		if (cMarkup == '#')
		{
			m_nNumericListCounter = 1;
		}
	}

	printf("Markup: push '%c' ... history is \"%s\"\n", cMarkup, m_strMarkupHistory);

	return cMarkup;
}


//----------------------------------------------------------//
// CLog::PopMarkup
//----------------------------------------------------------//
//-- Description			
// Pop a markup off the stack.
//----------------------------------------------------------//
s8 CLog::PopMarkup(void)
{
	if (m_nMarkupHistoryIndex > 0)
	{
		if (m_strMarkupHistory[m_nMarkupHistoryIndex] == '#')
		{
			m_nNumericListCounter = 0;
		}

		m_strMarkupHistory[--m_nMarkupHistoryIndex] = '\0';
	}
	
	printf("Markup: pop ... history is \"%s\"\n", m_strMarkupHistory);

	return GetCurrentMarkup();
}


//----------------------------------------------------------//
// CLog::MultiPopMarkup
//----------------------------------------------------------//
//-- Description			
// Pop markup repeatedly and rollback to a previous state
// Valid options:
// < pops until previous > is top of the stack.
// ] pops until previous [ is top of ths stack.
// } pops until previous { is top of the stack.
// = pops to the very first item on the stack.
//----------------------------------------------------------//
s8 CLog::MultiPopMarkup(s8 cMarkup)
{
	if (m_nMarkupHistoryIndex > 0)
	{
		switch (cMarkup)
		{
			case '<':
			{
				//-- pop to last Indent markup
				while ( (GetCurrentMarkup() != '>')
					&& (m_nMarkupHistoryIndex > 0) )
				{
					EndFormatted(GetCurrentMarkup(), "");
					PopMarkup();
				}
			}
			break;

			case ']':
			{
				//-- pop to last bracket
				while ( (GetCurrentMarkup() != '[')
					&& (m_nMarkupHistoryIndex > 0) )
				{
					EndFormatted(GetCurrentMarkup(), "");
					PopMarkup();
				}
			}
			break;

			case '}':
			{
				//-- pop to last curly
				while ( (GetCurrentMarkup() != '{')
					&& (m_nMarkupHistoryIndex > 0) )
				{
					EndFormatted(GetCurrentMarkup(), "");
					PopMarkup();
				}
			}
			break;

			case '=':
			{
				//-- pop to first markup in history, NOT EMPTY STACK
				//-- (because there will be a PopMarkup() later)
				while (m_nMarkupHistoryIndex > 0)
				{
					EndFormatted(GetCurrentMarkup(), "");
					PopMarkup();
				}
			}
			break;

			default:
			break;
		}

		printf("Markup: multipop '%c' ... history is \"%s\"\n", cMarkup, m_strMarkupHistory);
	}

	return GetCurrentMarkup();
}


//----------------------------------------------------------//
// CLog::GetCurrentMarkup
//----------------------------------------------------------//
//-- Description			
// Returns the current markup mode symbol.
//----------------------------------------------------------//
s8 CLog::GetCurrentMarkup(void) const
{
	if (m_nMarkupHistoryIndex > 0)
	{
		u32 nIndex = m_nMarkupHistoryIndex;
		return m_strMarkupHistory[nIndex - 1];
	}

	return '\0';
}


//----------------------------------------------------------//
// CLog::PrintLineFront
//----------------------------------------------------------//
//-- Description			
// Print the line front, including date, indent and markup
//----------------------------------------------------------//
void CLog::PrintLineFront(s32 nBack)
{
	if (IS_TRUE(m_FileAccessor.IsOpen()))
	{
		//-- Add time since start to each line
		time_t nCurTime;
		time(&nCurTime);
		f64 fTotalSeconds = difftime(nCurTime, m_nStartTime);
		u32 nMinutes = (u32)(fTotalSeconds / 60.0f);
		u32 nSeconds = (u32)fTotalSeconds % 60;

		m_FileAccessor.Printf("<%d:%02d> ", nMinutes, nSeconds);

		s32 nHistoryLoops = m_nMarkupHistoryIndex + nBack;

		//-- print indent
		for (s32 nIndex = 0; nIndex < nHistoryLoops; ++nIndex)
		{
			switch (m_strMarkupHistory[nIndex])
			{
				case '>':
				case '[':
				case '{':
				{
					//-- Need an extra indent
					m_FileAccessor.Printf("  ");
				}
				break;

				case '!':
				{
					//-- Add a !
					m_FileAccessor.Printf("! ");
				}
				break;

				case '*':
				{
					//-- Add a *
					if (nIndex == (nHistoryLoops - 1))
					{
						m_FileAccessor.Printf("* ");
					}
					else
					{
						m_FileAccessor.Printf("  ");
					}
				}
				break;

				case '#':
				{
					//-- Add a number
					if (nIndex == (nHistoryLoops - 1))
					{
						m_FileAccessor.Printf("%02d. ", m_nNumericListCounter++);
					}
					else
					{
						m_FileAccessor.Printf("    ");
					}
				}
				break;

				default:
				break;
			}
		}
	}
}


//----------------------------------------------------------//
// CLog::StartFormatted
//----------------------------------------------------------//
//-- Description			
// Prints anything required to start a markup block.
//----------------------------------------------------------//
void CLog::StartFormatted(s8 cMarkup, const s8* strText)
{
	if (IS_TRUE(m_FileAccessor.IsOpen()))
	{
		switch (cMarkup)
		{
			case '>':
			case '*':
			case '#':
			{
				//-- Start of Indent block.
				PrintLineFront();
				m_FileAccessor.Printf("%s\n", strText);
			}
			break;

			case '!':
			{
				//-- Start of Warning block.
				PrintLineFront(-1);
				m_FileAccessor.Printf("!!! %s !!!\n", strText);
			}
			break;

			case '[':
			{
				//-- Start of Bracket block.
				PrintLineFront(-1);
				m_FileAccessor.Printf("%s [\n", strText);
			}
			break;

			case '{':
			{
				//-- Start of Curly block.
				PrintLineFront(-1);
				m_FileAccessor.Printf("%s {\n", strText);
			}
			break;

			default:
			break;
		}
	}
}


//----------------------------------------------------------//
// CLog::EndFormatted
//----------------------------------------------------------//
//-- Description			
// Prints anything required to end a markup block.
//----------------------------------------------------------//
void CLog::EndFormatted(s8 cMarkup, const s8* strText)
{
	if (IS_TRUE(m_FileAccessor.IsOpen()))
	{
		switch (cMarkup)
		{
			case '>':
			case '*':
			case '#':
			case '!':
			{
				//-- End of block.
				if (IS_FALSE(SysString::IsEmpty(strText)))
				{
					PrintLineFront(-1);
					m_FileAccessor.Printf("%s\n", strText);
				}
			}
			break;

			case '[':
			{
				//-- End of Bracket block.
				PrintLineFront(-1);
				m_FileAccessor.Printf("] %s\n", strText);
			}
			break;

			case '{':
			{
				//-- End of Curly block.
				PrintLineFront(-1);
				m_FileAccessor.Printf("} %s\n", strText);
			}
			break;

			default:
			{
				PrintLineFront(-1);
				m_FileAccessor.Printf("%s\n", strText);
			}
			break;
		}
	}
}


//----------------------------------------------------------//
// CLog::PrintFormatted
//----------------------------------------------------------//
//-- Description			
// Prints a line of text using the current markup style.
//----------------------------------------------------------//
void CLog::PrintFormatted(const s8* strText)
{
	if (IS_TRUE(m_FileAccessor.IsOpen()))
	{
		s8* next_token1 = NULL;
		s8* strLine = SysString::Strtok((s8*)strText, "\n", next_token1);
		while (IS_PTR(strLine))
		{
			if (IS_FALSE(SysString::IsEmpty(strLine)))
			{
				PrintLineFront();
				m_FileAccessor.Printf("%s\n", strLine);
			}

			strLine = SysString::Strtok(NULL, "\n", next_token1);
		}
	}
}


//----------------------------------------------------------//
// CLog::Printf 
//----------------------------------------------------------//
//-- Description			
// Sends a string to the output file. String can be in a
// printf() sort of style. Newlines are stripped out if
// present to maintain formatted layout.
//----------------------------------------------------------//
void CLog::Printf(const s8 *strText, ...)
{
	if (IS_TRUE(m_FileAccessor.IsOpen()))
	{
		m_strWorkingBuffer.Clear();

		s8 *pBuffer = m_strWorkingBuffer.Buffer();

		va_list ArgList;
		va_start(ArgList, strText);
		SysString::Vsprintf(pBuffer, LOG_WORKING_BUFFER_SIZE, strText, ArgList);
		va_end(ArgList);

		if (IS_FALSE(m_strWorkingBuffer.IsEmpty()))
		{
			switch (pBuffer[0])
			{
				case '\\':
				{
					if (m_strWorkingBuffer.Length() > 1)
					{
						PrintFormatted(pBuffer + 1);
					}
				}
				break;

				case '>':
				case '[':
				case '{':
				case '!':
				case '#':
				case '*':
				{
					PushMarkup(pBuffer[0]);
					if (m_strWorkingBuffer.Length() > 1)
					{
						StartFormatted(GetCurrentMarkup(), pBuffer + 1);
					}
				}
				break;

				case '|':
				case '<':
				case ']':
				case '}':
				case '=':
				{
					MultiPopMarkup(pBuffer[0]);
					if (m_strWorkingBuffer.Length() > 1)
					{
						EndFormatted(GetCurrentMarkup(), pBuffer + 1);
					}
					PopMarkup();
				}
				break;

				default:
				{
					PrintFormatted(pBuffer);
				}
				break;
			}
		}
	}
}


//----------------------------------------------------------//
// CLog::AddTimeStamp
//----------------------------------------------------------//
//-- Description			
// Adds a time stamp without any indentation.
//----------------------------------------------------------//
void CLog::AddTimeStamp(void)
{
	if (IS_TRUE(m_FileAccessor.IsOpen()))
	{
		time_t nLogTime;
		time(&nLogTime);
		FixedString<SYSTIME_BUFFER_SIZE> strTime;
		SysTime::Ctime(strTime.Buffer(), strTime.Size(), &nLogTime);
		
		m_FileAccessor.Printf("[%s]\n", strTime.ConstBuffer());
	}
}


//----------------------------------------------------------//
// CLog::AddBreakLine
//----------------------------------------------------------//
//-- Description			
// Adds a break line and 2 empty lines into the log.
//----------------------------------------------------------//
void CLog::AddBreakLine(void)
{
	if (IS_TRUE(m_FileAccessor.IsOpen()))
	{
		m_FileAccessor.Printf("--------------------------------------------------------------------------------------\n");
	}
}	


//----------------------------------------------------------//
// CLog::AddBlankLine
//----------------------------------------------------------//
//-- Description			
// Adds a blank line into the log.
//----------------------------------------------------------//
void CLog::AddBlankLine(void)
{
	if (IS_TRUE(m_FileAccessor.IsOpen()))
	{
		m_FileAccessor.Printf("\n");
	}
}


//----------------------------------------------------------//
// CLog::PushIndent
//----------------------------------------------------------//
//-- Description			
// Increments the tab indent
//----------------------------------------------------------//
void CLog::PushIndent(void)
{
	PushMarkup('>');
}


//----------------------------------------------------------//
// CLog::PopIndent
//----------------------------------------------------------//
//-- Description			
// Decrements the tab indent
//----------------------------------------------------------//
void CLog::PopIndent(void)
{
	MultiPopMarkup('<');
	PopMarkup();
}
	

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
