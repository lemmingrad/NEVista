#ifndef _PACKET_HEADER_H_
#define _PACKET_HEADER_H_
#pragma once


#include "Types.h"
#include "Serialized.h"


class CSerializer;


class CPacketHeader : public CSerialized
{
	public:
	
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
			u8							m_nVersion;						//-- version number
			u8							m_nFlags;						//-- flags
			u16							m_nSize;						//-- size (bytes) of packet data, not including the header
			u16							m_nMessages;					//-- number of messages in packet
		};


		CPacketHeader() : CSerialized() {};
		virtual ~CPacketHeader() {};

		static size_t				GetMaxHeaderSize(void)
		{
			return sizeof(HeaderV1);
		}

		size_t							GetHeaderSize(void) const
		{
			switch (m_Header.m_nVersion)
			{
				case Version::V1:
				{
					return sizeof(HeaderV1);
				}
				break;

				default:
				break;
			}
			
			return 0;
		}

		Version::Enum					Validate(void) const
		{
			switch (m_Header.m_nVersion)
			{
				case Version::V1:
				{
					if ( (GetDataSize() > 0)
						&& (GetMessageCount() > 0) )
					{
						return (Version::Enum)m_Header.m_nVersion;
					}
				}
				break;

				default:
				break;
			}

			return Version::Unknown;
		}

		bool							IsEncrypted(void) const
		{
			switch (m_Header.m_nVersion)
			{
				case Version::V1:
				{
					return TEST_FLAG(m_Header.m_nFlags, Flag::Encrypted);
				}
				break;

				default:
				break;
			}

			return false;
		}

		bool							IsCompressed(void) const
		{
			switch (m_Header.m_nVersion)
			{
				case Version::V1:
				{
					return TEST_FLAG(m_Header.m_nFlags, Flag::Compressed);
				}
				break;

				default:
				break;
			}

			return false;
		}

		u16								GetMessageCount(void) const
		{
			switch (m_Header.m_nVersion)
			{
				case Version::V1:
				{
					return m_Header.m_nMessages;
				}
				break;

				default:
				break;
			}

			return 0;
		}
		void							SetMessageCount(u16 nCount)
		{
			//-- Version::V1
			m_Header.m_nMessages = nCount;
		}

		size_t							GetDataSize(void) const
		{
			switch (m_Header.m_nVersion)
			{
				case Version::V1:
				{
					return m_Header.m_nSize;
				}
				break;

				default:
				break;
			}

			return 0;
		}

		void							SetDataSize(size_t nSize)
		{
			//-- Version::V1
			m_Header.m_nSize = nSize;
		}

		virtual void					Serialize(CSerializer& Serializer)
		{
			m_Header.m_nFlags;
		}

	private:

		HeaderV1						m_Header;
};




#endif //_PACKET_HEADER_H_