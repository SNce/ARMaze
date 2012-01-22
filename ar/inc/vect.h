#ifndef _VECT
#define _VECT

template<class T>
class Vect3
{
public:
	union
	{
		struct
		{
			T x;
			T y;
			T z;
		};
		T _array[3];
	};

	Vect3(const T& t = T())
	{
		for(int i = 0; i < 3; i++)
			_array[i] = t;
	}

	Vect3(const Vect3<T>& rhs)
	{
		for(int i = 0; i < 3; i++)
			_array[i] = rhs._array[i];
	}

	Vect3(const T* t)
	{
		for(int i = 0; i < 3; i++)
			_array[i] = t[i];
	}

	Vect3(const T x, const T y, const T z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	//LHS and RHS indexing operator.
	T& operator[] (int i)
	{
		return _array[i];
	}

	const T& operator[] (const int i) const
	{
		return _array[i];
	}

	//get and set.
	T* getValue()
	{
		return _array;
	}

	Vect3& setValue(const T* t)
	{
		for(int i = 0; i < 3; i++)
			_array[i] = t[i];
		return *this;
	}
	
	//math operators.
	friend Vect3<T>& operator *= (Vect3<T>& lhs, T v)
	{
		for(int i = 0; i < 3; i++)
			lhs._array[i] *= v;
		return lhs;
	}

	friend Vect3<T>& operator *= (Vect3<T>& lhs, Vect3<T>& rhs)
	{
		for(int i = 0; i < 3; i++)
			lhs._array[i] *= rhs._array[i];
		return lhs;
	}

	friend Vect3<T>& operator /= (Vect3<T>& lhs, T v)
	{
		for(int i = 0; i < 3; i++)
			lhs._array[i] /= v;
		return lhs;
	}

	friend Vect3<T>& operator += (Vect3<T>& lhs, T v)
	{
		for(int i = 0; i < 3; i++)
			lhs._array[i] += v;
		return lhs;
	}

	friend Vect3<T>& operator += (Vect3<T>& lhs, Vect3<T>& rhs)
	{
		for(int i = 0; i < 3; i++)
			lhs._array[i] += rhs._array[i];
		return lhs;
	}

	friend Vect3<T>& operator -= (Vect3<T>& lhs, T v)
	{
		for(int i = 0; i < 3; i++)
			lhs._array[i] -= v;
		return lhs;
	}

	friend Vect3<T>& operator -= (Vect3<T>&  lhs, Vect3<T>& rhs)
	{
		for(int i = 0; i < 3; i++)
			lhs._array[i] -= rhs._array[i];
		return lhs;
	}

	friend Vect3<T>& operator - (const Vect3<T>& rhs)
	{
		Vect3<T> rv;
		for(int i = 0; i < 3; i++)
			rv._array[i] = -rhs._array[i];
		return rv;
	}

	friend Vect3<T>& operator + (Vect3<T>& lhs, Vect3<T>& rhs)
	{
		Vect3<T> rv(lhs);
		rv += rhs;
		return rv;
	}

	friend Vect3<T>& operator - (Vect3<T>& lhs, Vect3<T>& rhs)
	{
		Vect3<T> rv(lhs);
		rv -= rhs;
		return rv;
	}

	friend Vect3<T>& operator * (Vect3<T>& lhs, T v)
	{
		Vect3<T> rv(lhs);
		rv *= v;
		return rv; 
	}

	friend Vect3<T>& operator * (T v, Vect3<T>& rhs)
	{
		Vect3<T> rv(rhs);
		rv *= v;
		return rv;
	}

	friend Vect3<T>& operator * (const Vect3<T>& lhs, const Vect3<T>& rhs)
	{
		Vect3<T> rv(lhs);
		rv *= rhs;
		return rv;
	}

	friend Vect3<T>& operator / (Vect3<T>& lhs, T v)
	{
		Vect3<T> rv(lhs);
		rv /= v;
		return rv;
	}



	// Comparison operators.
	friend bool operator == (const Vect3<T>& lhs, const Vect3<T>& rhs)
	{
		bool rv = true;
		for(int i = 0; i < 3; i++)
			rv &= lhs._array[i] == rhs._array[i];
		return rv;
	}

	friend bool operator != (const Vect3<T>& lhs, const Vect3<T>& rhs)
	{
		bool rv = true;
		for(int i = 0; i < 3; i++)
			rv &= lhs._array[i] != rhs._array[i];
		return rv;
	}

	Vect3<T>& normalize()
	{
		float sum = _array[0] * _array[0];
		sum += _array[1] * _array[1];
		sum += _array[2] * _array[2];

		sum = sqrt(sum);
		*this = *this / sum;

		return *this;
	}

	Vect3<T>& setMagnitude(float v)
	{
		this->normalize();
		*this *= v;
		return *this;
	}

};

typedef Vect3<float> Vector3;

#endif