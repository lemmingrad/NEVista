#ifndef _SERIALIZED_H_
#define _SERIALIZED_H_
#pragma once

//----------------------------------------------------------//
// SERIALIZED.H
//----------------------------------------------------------//
//-- Description			
// Base class definition for a serialised class
//----------------------------------------------------------//


#include "Types.h"


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


class CSerialized
{
	public:

		CSerialized() {};
		virtual ~CSerialized() {};

		virtual void Serialize(CSerializer& Serializer) = 0;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_SERIALIZER_H_