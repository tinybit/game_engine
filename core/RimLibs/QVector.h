// Vector.h: interface for the CVector class.
//////////////////////////////////////////////////////////////////////

#if !defined(VECTOR_H_)
#define VECTOR_H_

#include <math.h>

typedef float scalar_t;  

class CVector  
{
public:
  scalar_t x;                                           /* data */
  scalar_t y;
  scalar_t z;

public:
	CVector(scalar_t a, scalar_t b, scalar_t c);          /* constructors */
  CVector(const CVector &vec);
  
  const CVector &operator=(const CVector &vec);         /* operators */
  const CVector operator+(const CVector &vec) const;
  const CVector operator-(const CVector &vec) const;
  const CVector operator+() const;
  const CVector operator-() const;
  const CVector& operator+=(const CVector &vec);
  const CVector& operator-=(const CVector &vec);
  const CVector& operator*=(const scalar_t &s);
  const CVector operator*(const scalar_t &s) const;
  friend inline const CVector operator*(const scalar_t &s, const CVector &vec);
  const CVector& operator/=(const scalar_t &s);
  const CVector operator/(const scalar_t s) const;
  const CVector operator^(const CVector &vec) const;
  const scalar_t operator%(const CVector &vec) const;
  const scalar_t operator!() const;
  const CVector operator|(const scalar_t length) const;
  const CVector& operator|=(const scalar_t length);

  const CVector CrossProduct(const CVector &vec) const; /* operations */
  const scalar_t DotProduct(const CVector &vec) const;
  const scalar_t Length() const;
  const scalar_t LengthSq() const;
  const CVector UnitVector() const;
  void Normalize();
  void Flip();
  float inline Angle(CVector vec);
  const CVector inline Reflection(const CVector &normal) const;
  void MultBy4x4Matrix(float matrix[16]);
	void SetData(scalar_t a, scalar_t b, scalar_t c);

  const bool operator==(const CVector &vec) const;      /* boolean operators */
  const bool operator!=(const CVector &vec) const;

public:
	void Clear();
	
  CVector();
};

#endif // !defined(VECTOR_H_)
