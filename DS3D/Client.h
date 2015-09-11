//----------------------------------------------------------//
// CLIENT.H
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//


#include "SysSmartPtr.h"
#include "TCPConnection.h"
#include "TCPNonBlockingConnector.h"
#include "Network.h"
#include "Message.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


class CClient
{
	public:

		struct Error
		{
			enum Enum
			{
				AlreadyRegistered	= 0x80000000,
				NotFound			= 0x80000001,

				BadServerAddress	= 0x80001000,
				ConnectFailed		= 0x80001001,
				LoginFailed			= 0x80001002,

				WrongState			= 0x80002001,

				ConnectionClosed	= 0x80003000,
				ConnectionError		= 0x80003001,

				//-- Badfail
				BadFail				= -1,

				//-- Success
				Ok					= 0,

				// In progress
				InProgress			= 1
			};
		};

		struct State
		{
			enum Enum
			{
				Disconnected = 0,
				Connecting,
				Connected,
				LoggingIn,
				LoggedIn,
				Disconnecting
			};
		};

		typedef bool (*TRecvMessageHandlerFunc)(CMessage::Type nType, void* pData, bool bAlreadyProcessed);

		CClient();
		~CClient();

		Error::Enum		Initialise(void);
		Error::Enum		Shutdown(void);
		Error::Enum		Update(void);

		Error::Enum		Connect(s8* strServer);
		Error::Enum		Connect(const s8* strIP, const s8* strPort);
		Error::Enum		Disconnect(void);

		Error::Enum		AddRecvMessageHandler(CMessage::Type nType, TRecvMessageHandlerFunc handlerFunc, void* pData);
		Error::Enum		RemoveRecvMessageHandler(CMessage::Type nType, TRecvMessageHandlerFunc handlerFunc);

		Error::Enum		SendMessage(SysSmartPtr<CMessage> pMessage);

	private:

		struct RecvMessageHandlerRecord
		{
			RecvMessageHandlerRecord(TRecvMessageHandlerFunc pCB, void* pData)
			: m_pCB(pCB)
			, m_pData(pData)
			{
			}

			TRecvMessageHandlerFunc			m_pCB;
			void*							m_pData;
		};

		typedef std::list<RecvMessageHandlerRecord> TRecvMessageHandlerRecordList;
		typedef std::map<CMessage::Type, TRecvMessageHandlerRecordList>	TRecvMessageHandlerArray;

		CTCPNonblockingConnector			m_connector;
		SysSmartPtr<CTCPConnection>			m_pConnection;

		TMessageList						m_recvMessageList;
		TMessageList						m_sendMessageList;

		TRecvMessageHandlerArray			m_RecvMessageHandlerArray;

		State::Enum							m_eState;

		Error::Enum							Reset(Error::Enum eError);

		Error::Enum							NotifyRecvMessageHandlers(void);

		static bool							ClientStateChangeHandler(CMessage::Type nType, void* pData, bool bAlreadyHandled);
		static bool							ByeHandler(CMessage::Type nType, void* pData, bool bAlreadyHandled);
};


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
