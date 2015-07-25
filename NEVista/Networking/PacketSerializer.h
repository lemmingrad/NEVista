#ifndef _PACKET_SERIALIZER_H_
#define _PACKET_SERIALIZER_H_
#pragma once

//----------------------------------------------------------//
// PACKETSERIALIZER.H
//----------------------------------------------------------//
//-- Description			
// Packet Serializer. Converts a packet class to and from
// a linear byte buffer, taking into account system endian.
//----------------------------------------------------------//


#include "Types.h"
#include "Serializer.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define PACKET_SERIALIZER_USES_DEBUG_FOURCC


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CPacketSerializer : public CSerializer
{
	public:

		struct Error
		{
			enum Enum
			{
				CopyFailed				= -5,
				BadParameter			= -4,
				FourCCMismatch			= -3,
				EndReached				= -2,
				Fail					= -1,
				Ok						= 0
			};
		};

		CPacketSerializer(Mode::Enum eMode, u8* pBuffer, size_t nBufferSize, size_t nOffset = 0);
		virtual ~CPacketSerializer();

		u8*						GetBuffer(void);
		size_t					GetSize(void) const;
		size_t					GetOffset(void) const;
		Error::Enum				GetError(void) const;

		u8*						SerializeReserve(size_t nReservedSize);

		//-- CSerializer
		virtual	size_t			SerializeF32(f32& fValue, u32 nFourCC = 'f32 '); 
		virtual	size_t			SerializeF64(f64& fValue, u32 nFourCC = 'f64 '); 
		virtual	size_t			SerializeS32(s32& nValue, u32 nFourCC = 's32 '); 
		virtual	size_t			SerializeU32(u32& nValue, u32 nFourCC = 'u32 '); 
		virtual	size_t			SerializeS8(s8& nValue, u32 nFourCC = 's8  '); 
		virtual	size_t			SerializeU8(u8& nValue, u32 nFourCC = 'u8  '); 
		virtual	size_t			SerializeS16(s16& nValue, u32 nFourCC = 's16 '); 
		virtual	size_t			SerializeU16(u16& nValue, u32 nFourCC = 'u16 '); 
		virtual	size_t			SerializeS64(s64& nValue, u32 nFourCC = 's64 '); 
		virtual	size_t			SerializeU64(u64& nValue, u32 nFourCC = 'u64 '); 
		virtual	size_t			SerializeBitfield(bitfield& nFlags, u32 nFourCC = 'bits'); 
		virtual size_t			SerializeBytes(u8* pData, size_t nDataSize, u32 nFourCC = 'data');

	protected:

		union UBAf32
		{
			f32		m_fValue;
			u8		m_Bytes[sizeof(f32)];
		};
		union UBAf64
		{
			f64		m_fValue;
			u8		m_Bytes[sizeof(f64)];
		};
		union UBAs32
		{
			s32		m_nValue;
			u8		m_Bytes[sizeof(s32)];
		};
		union UBAs16
		{
			s16		m_nValue;
			u8		m_Bytes[sizeof(s16)];
		};
		union UBAs64
		{
			s64		m_nValue;
			u8		m_Bytes[sizeof(s64)];
		};
		union UBAu32
		{
			u32		m_nValue;
			u8		m_Bytes[sizeof(u32)];
		};
		union UBAu16
		{
			u16		m_nValue;
			u8		m_Bytes[sizeof(u16)];
		};		
		union UBAu64
		{
			u64		m_nValue;
			u8		m_Bytes[sizeof(u64)];
		};		
		union UBAbitfield
		{
			bitfield m_nValue;
			u8		m_Bytes[sizeof(bitfield)];
		};	
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
		union UBAFourCC
		{
			u32		m_nValue;
			u8		m_Bytes[sizeof(u32)];
		};
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

		u8*						m_pBuffer;
		size_t					m_nSize;
		size_t					m_nOffset;
		Error::Enum				m_eError;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_PACKET_SERIALIZER_H_