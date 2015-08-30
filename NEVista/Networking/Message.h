#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#pragma once

//----------------------------------------------------------//
// MESSAGE.H
//----------------------------------------------------------//
//-- Description			
// A message class. 
// Groups of messages are sent across the connection in a 
// packet. Each packet may or may not be encrypted and/or
// compressed. Packets are guaranteed to contain at least
// one message, and all messages must be complete.
//----------------------------------------------------------//


#include "Types.h"
#include "Serialized.h"
#include <map>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//


class CSerializer;


//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CMessage : public CSerialized
{
	public:

		typedef u32 Type;

		static const Type kTypeUnknown = 'xxxx';

/*		struct Type
		{
			enum Enum
			{
				Unknown											= 'xxxx',
				MsgBye											= 'bye ',
				MsgMotd											= 'motd',
				MsgClientKeyExchange							= 'ckey',
				MsgServerKeyExchange							= 'skey',
				MsgLogin										= 'logn',
				MsgChat											= 'chat'
				//-- Add new message IDs here.
			};
		};
*/
		struct Flag
		{
			//-- Note: Never flags override Always flags
			enum
			{
				NeverEncrypted			= BIT(0),
				AlwaysEncrypted			= BIT(1),
				NeverCompressed			= BIT(2),
				AlwaysCompressed		= BIT(3),
				ForcedEnd				= BIT(4)
			};
		};

		CMessage(Type nType, bitfield bitFlags);
		virtual ~CMessage();
		
		Type					GetType(void) const;
		bool					CanBeEncrypted(bool bRecommended) const;
		bool					CanBeCompressed(bool bRecommended) const;
		bool					IsForcedEnd(void) const;

		//-- CSerialized
		virtual size_t			Serialize(CSerializer& Serializer);

	protected:

		Type					m_nType;
		bitfield				m_bitFlags;
};


class CMessageFactory
{
	public:

		typedef CMessage* (*TCreateFunc)(void);
		typedef std::map<CMessage::Type, TCreateFunc> TFuncMap;
		
		CMessageFactory();
		~CMessageFactory();

		static void				RegisterType(CMessage::Type nType, TCreateFunc func);
		static CMessage*		CreateType(CMessage::Type nType);

private:

		static TFuncMap&		GetMap(void);
};


template <class T>
class CMessageRegistrar
{
	public:

		CMessageRegistrar(CMessage::Type nType, CMessageFactory::TCreateFunc func)
		{
			CMessageFactory::RegisterType(nType, func);
		}
};


#define DECLARE_MESSAGE_REGISTRAR(K, T) \
	private: \
		static const CMessageRegistrar<T> sm_reg; \
	public: \
		static const Type kType = K; \
		static CMessage* CreateFunc(void);

#define IMPLEMENT_MESSAGE_REGISTRAR(T) \
	const CMessageRegistrar<T> T::sm_reg(T::kType, &T::CreateFunc); \
	CMessage* T::CreateFunc(void)


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_MESSAGE_H_