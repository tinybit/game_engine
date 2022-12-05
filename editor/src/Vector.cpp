// Vector.cpp: implementation of the CVector class.
//////////////////////////////////////////////////////////////////////

#include "Vector.h"

//--------------------------- construction --------------------
//default
CVector::CVector()
{
	Clear();
}

//scalar values
CVector::CVector(scalar_t a, scalar_t b, scalar_t c)
{
	x = a; y = b; z = c;
}

//vector
CVector::CVector( const CVector &vec )
{
	x = vec.x; y = vec.y; z = vec.z;
}

//--------------------------- operators --------------------

//assignment
const CVector& CVector::operator=(const CVector &vec)
{
  x = vec.x; y = vec.y; z = vec.z;
  return *this;
}

const CVector& CVector::operator=(const D3DXVECTOR3 &vec)
{
  x = vec.x; y = vec.y; z = vec.z;
  return *this;
}

void CVector::SetData(scalar_t a, scalar_t b, scalar_t c)
{
  x = a; y = b; z = c;
}

//addition
const CVector CVector::operator+(const CVector &vec) const
{
  return CVector(x + vec.x, y + vec.y, z + vec.z);
}

//substraction
const CVector CVector::operator-(const CVector &vec) const
{
  return CVector(x - vec.x, y - vec.y, z - vec.z);
}

//add
const CVector CVector::operator+() const
{
  return CVector(*this);
}

//negation
const CVector CVector::operator-() const
{
  return CVector(-x, -y, -z);
}

//increment
const CVector& CVector::operator+=(const CVector &vec)
{
  x += vec.x; y += vec.y; z += vec.z;
  return *this;
}

//decrement
const CVector& CVector::operator-=(const CVector &vec)
{
  x -= vec.x; y -= vec.y; z -= vec.z;
  return *this;
}

//scalar multiplication
const CVector& CVector::operator*=(const scalar_t &s)
{
  x *= s; y *= s; z *= s;
  return *this;
}

//scalar post multiplication
const CVector CVector::operator*(const scalar_t &s) const
{
  return CVector(x * s, y * s, z * s);
}

//scalar pre multiplication
inline const CVector operator*(const scalar_t &s, const CVector &vec)
{
  return vec * s;
}

//scalar division
const CVector& CVector::operator/=(const scalar_t &s)
{
  const float recip = 1 / s; // speed-up

  x *= recip; y *= recip; z *= recip;
  return *this;
}

//divide by scalar
const CVector CVector::operator/(scalar_t s) const
{
  s = 1 / s; // speed-up

  return CVector(x * s, y * s, z * s);
}

//cross product2
const CVector CVector::operator^(const CVector &vec) const
{
  return CVector(y * vec.z - z * vec.y, 
                 z * vec.x - x * vec.z,
                 x * vec.y - y * vec.x);
}

//dot product 2
const scalar_t CVector::operator%(const CVector &vec) const
{
  return (x * vec.x + y * vec.y + z * vec.z);
}

//length 2
const scalar_t CVector::operator!() const
{
  return sqrtf(x * x + y * y + z * z);
}

//--------------------------- operations --------------------

//cross product
const CVector CVector::CrossProduct(const CVector &vec) const
{
  return CVector(y * vec.z - z * vec.y, 
                 z * vec.x - x * vec.z,
                 x * vec.y - y * vec.x);
}

//dot product
const scalar_t CVector::DotProduct(const CVector &vec) const
{
  return (x * vec.x + y * vec.y + z * vec.z);
}

//length
const scalar_t CVector::Length() const
{
  return (scalar_t)sqrtf((float)(x * x + y * y + z * z));
}

//length sq
const scalar_t CVector::LengthSq() const
{
  return (scalar_t)(x * x + y * y + z * z);
}


//unit vector
const CVector CVector::UnitVector() const
{
  return (*this) / Length();
}

//normalize
void CVector::Normalize()
{
  (*this) /= Length();
}

void CVector::Flip()
{
  (*this) *= -1.0f;
}

//return vector with specified length
const CVector CVector::operator|(const scalar_t length) const
{
	return (*this) * (length / !(*this));
}

//set vector length
const CVector& CVector::operator|=(const scalar_t length)
{
	return (*this) = (*this) | length;
}

//get angle between vectors
float inline CVector::Angle(CVector vec)
{
	return acosf(*this % vec);
}

//reflect about normal
const CVector inline CVector::Reflection(const CVector &normal) const
{
  const CVector vec(*this | 1); // unit vector

  return (vec - normal * 2.0 * (vec % normal)) * !(*this);
}

void CVector::MultBy4x4Matrix(float m[])
{
  scalar_t four = 1.0;
  scalar_t temp_x, temp_y, temp_z;

  temp_x    = m[0] * x + m[4] * y + m[8]  * z + m[12] * four;
  temp_y    = m[1] * x + m[5] * y + m[9]  * z + m[13] * four;
  temp_z    = m[2] * x + m[6] * y + m[10] * z + m[14] * four;
  four      = m[3] * x + m[7] * y + m[11] * z + m[15] * four;

  if (four != 0.0)
  {
    temp_x /= four;
    temp_y /= four;
    temp_z /= four;
  }  

  x = temp_x;
  y = temp_y;
  z = temp_z;
}

//--------------------------- boolean operators --------------------

//equality
const bool CVector::operator==(const CVector &vec) const
{
  return ((x == vec.x) && (y == vec.y) && (z == vec.z));
}

//inequality
const bool CVector::operator!=(const CVector &vec) const
{
  return !(*this == vec);
}


//--------------------------- consruction --------------------------
void CVector::Clear()
{
  x = y = z = 0.0;
}
