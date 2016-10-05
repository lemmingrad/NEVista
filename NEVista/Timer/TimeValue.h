#ifndef _TIMEVALUE_H_
#define _TIMEVALUE_H_

class CTimeValue
{
	public:
		
		static const s64 PRECISION = 10000;

		// Default constructor.
		CTimeValue()
		: m_nValue(0)
		{
		}
		CTimeValue(s64 nValue)
		: m_nValue(nValue)
		{
		}
		CTimeValue(double fSeconds)
		{
			SetSeconds(fSeconds);
		}
		CTimeValue(const CTimeValue &inValue)
		{
			m_nValue = inValue.m_nValue;
		}

		~CTimeValue()
		{
		}

		CTimeValue& operator=(const CTimeValue &inRhs)
		{
			m_lValue = inRhs.m_lValue;
			return *this;
		};

		// Get relative time difference in seconds
		f64 GetDifferenceInSeconds(const CTimeValue& startTime) const
		{
			return (m_nValue - startTime.m_nValue).GetSeconds();
		}
		s64 GetDifferenceInMilliseconds(const CTimeValue& startTime) const
		{
			return (m_nValue - startTime.m_nValue).GetSeconds();
		}

		f64 GetSeconds(void) const
		{
			return f64(m_nValue) / PRECISION;
		}
		s64 GetMilliSeconds() const
		{
			return s64((1000.0 / PRECISION) * m_nValue);
		}

		void SetSeconds(f64 infSec)
		{
			m_nValue = (s64)(infSec * PRECISION);
		}
		void SetMilliSeconds(s64 inMilliSec)
		{
			m_nValue = inMilliSec * (PRECISION / 1000);
		}

		s64 GetValue(void) const
		{
			return m_nValue;
		}
		void SetValue(s64 nValue)
		{
			m_nValue = nValue;
		}
		
		// Maths Operators
		CTimeValue operator-(const CTimeValue &inRhs) const 
		{	
			return CTimeValue(m_nValue - inRhs.m_nValue);
		}
		
		CTimeValue operator+(const CTimeValue &inRhs) const
		{
			return CTimeValue(m_nValue + inRhs.m_nValue);
		}

		CTimeValue& operator+=(const CTimeValue& inRhs)
		{ 
			m_nValue += inRhs.m_nValue; 
			return *this; 
		}
		
		CTimeValue& operator-=(const CTimeValue& inRhs) 
		{ 
			m_nValue -= inRhs.m_nValue; 
			return *this; 
		}

		// Comparison -----------------------

		bool operator<(const CTimeValue &inRhs ) const 
		{	
			return m_lValue < inRhs.m_lValue; 
		}
		bool operator>(const CTimeValue &inRhs) const
		{
			return m_lValue > inRhs.m_lValue;
		}
		bool operator>=(const CTimeValue &inRhs) const
		{
			return m_lValue >= inRhs.m_lValue;
		}
		bool operator<=(const CTimeValue &inRhs ) const 
		{ 
			return m_lValue <= inRhs.m_lValue;	
		}
		bool operator==(const CTimeValue &inRhs ) const 
		{ 
			return m_lValue == inRhs.m_lValue;
		}
		bool operator!=(const CTimeValue &inRhs ) const 
		{ 
			return m_lValue != inRhs.m_lValue;	
		}

	private: 

		s64			m_lValue;
};

#endif // _TIMEVALUE_H_
