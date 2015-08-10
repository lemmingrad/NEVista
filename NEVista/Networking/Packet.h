#ifndef _PACKET_H_
#define _PACKET_H_
#pragma once


//----------------------------------------------------------//
// PACKET.H
//----------------------------------------------------------//
//-- Description			
// A packet class. 
// Groups of messages are sent across the connection in a 
// packet. Each packet may or may not be encrypted and/or
// compressed. Packets are guaranteed to contain at least
// one message, and all messages must be complete.
//----------------------------------------------------------//


#include "Types.h"
#include "SysSmartPtr.h"
#include "SimpleBuffer.h"
#include "Network.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


//-- This is the maximum size of the data section of an unencrypted packet.
#define PACKET_DATA_BUFFER_MAX_SIZE			(1024)


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//


class CSerializer;
class CPacketSerializer;
class CMessage;


//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CPacket
{
	public:
	
		struct Error
		{
			enum Enum
			{
				//-- Not so bad fails
				SerializerFull				= 0x80000000,
				SerializerEmpty				= 0x80000001,
				DataBufferFull				= 0x80000002,
				DataBufferEmpty				= 0x80000003,

				//-- Bad fails
				UnknownVersion				= 0x80000010,
				ProtocolMismatch			= 0x80000011,
				EncryptionFailed			= 0x80000012,
				CopyFailed					= 0x80000013,
				SanityFail					= 0x80000014,
				
				BadFail						= -1,

				//-- Success
				Ok							= 0
			};
		};

		struct Version
		{
			enum Enum
			{
				Unknown = 0,
				V1,
				Current = V1
			};
		};

		struct Flag
		{
			enum Enum
			{
				Encrypted	= BIT(0),
				Compressed	= BIT(1)
			};
		};

		struct HeaderV1
		{
			u8							m_nFlags;						//-- flags
			u16							m_nMessages;					//-- number of messages in packet
			u16							m_nDataSize;					//-- size (bytes) of packet payload data, not including the header
		};

		CPacket(u8 nFlags = 0);
		virtual ~CPacket();

		static size_t					GetMinimumReadSize(void);

		size_t							GetHeaderSize(void) const;
		Version::Enum					Validate(void) const;

		bool							IsEncrypted(void) const;
		bool							IsCompressed(void) const;
		u16								GetMessageCount(void) const;
		size_t							GetDataSize(void) const;

		Error::Enum						Serialize(CPacketSerializer& serializer);

		Error::Enum						AddMessage(SysSmartPtr<CMessage> message);	
		Error::Enum						ConvertToMessages(TMessageList& pushList);		

	private:

		CSimpleBuffer<PACKET_DATA_BUFFER_MAX_SIZE>	m_DataBuffer;
		TMessageList					m_MessageList;

		HeaderV1						m_HeaderV1;
		u8								m_nVersion;						//-- version number
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_PACKET_H_