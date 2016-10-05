#ifndef _BESTFIT_H_
#define _BESTFIT_H_

#pragma once


template <class T>
class CLineOfBestFit
{
	public:

		CLineOfBestFit() 
		: m_fSlope(0.0f)
		, m_intercept()
		{
		}
		CLineOfBestFit(double fSlope, T intercept) 
		: m_fScope(fSlope)
		, m_intercept(intercept)
		{
		}

		GetY(T x) const
		{
			return (x * m_slope) + m_intercept;
		}

	private:

		T		m_intercept;
		double	m_fSlope;
};


template <class X, class Y, int N>
class CBestFitPlotter
{
	public:

		CBestFitPlotter()
		: m_nCount(0)
		{
		}

		void AddPoint(X x, Y y)
		{
			size_t nIndex = m_nCount % N;
			m_x[nIndex] = x;
			m_y[nIndex] = y;
			++m_nCount;
		}

		bool Empty() const
		{
			return IS_ZERO(m_nCount);
		}

		size_t Size() const
		{
			if (m_count > N)
			{
				return N;
			}
			else
			{
				return m_nCount;
			}
		}

		size_t Capacity() const
		{
			return N;
		}

		bool GetLineOfBestFit(CLineOfBestFit<X,Y>& result) const
		{
			//-- Calculate line of best fit using Least Square method

			//-- Only generate a line if we have at least 3 plotted points.
			if (m_nCount >= 3)
			{
				X averageX = X();
				Y averageY = Y();

				for (size_t i = 0; i < m_nCount; ++i)
				{
					averageX += m_x[i];
					averageY += m_y[i];
				}
				averageX /= m_nCount;
				averageY /= m_nCount;

				f64 fSlopeNumer = 0.0;
				f64 fSlopeDenom = 0.0;

				for (size_t i = 0; i < m_nCount; ++i)
				{
					f64 fXFactor = m_x[i] - averageX;
					f64 fYFactor = m_y[i] - averageY;

					fSlopeNumer += fXFactor * fYFactor;							
					fSlopeDenom += fXFactor * fXFactor;
				}

				if (fabsf(fSlopeDenom) >= DBL_EPSILON)
				{
					f64 fSlope = fSlopeNumer / fSlopeDenom;
						
					Y intercept = averageY - (fSlope * averageX);
						
					result = CResult(fSlope, intercept);
					return true;
				}
			}

			return false;
		}

	private:
		
		X		m_x[N];
		Y		m_y[N];
		size_t	m_nCount;
};



#endif //_BESTFIT_H_