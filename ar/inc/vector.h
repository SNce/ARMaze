////////////////////////////////////////////////////////////////////////
//
//
//	Template vector class for 2-D 3-D and 4-D vectors with operations.
//	
//  Author : Krishnakanth Mallik C
//
//
////////////////////////////////////////////////////////////////////////


#ifndef VECTOR_H
#define VECTOR_H

#include<math.h>
#include<stdio.h>

namespace a3d{

template <class T> class vector2;
template <class T> class vector3;
template <class T> class vector4;


/////////////////////////////////////////////////////
//
//  Class Template for vector2 class
//
/////////////////////////////////////////////////////


template <class T>
class vector2
{
public:

	typedef T data_type;
	int size() const { return 2;}

	/////////////////////////////////////////////////////
	//
	//  Data Members
	//
	/////////////////////////////////////////////////////


	union
	{
		struct
		{
			T x,y;
		};
		struct
		{
			T s,t;
		};
		T _array[2];
	};

	/////////////////////////////////////////////////////
    //
	//  Constructors
	//
	/////////////////////////////////////////////////////

	// Default/ scalar constructor
	vector2(const T & t = T())
	{
		for(int i = 0; i < size(); i++) _array[i] = t;
	}

	
	// Construct from array
	vector2(const T * tp)
	{
		for(int i = 0; i < size(); i++) _array[i] = tp[i];
	}


	// Construct from explicit values
	vector2(const T v0,const T v1)
	{
		x = v0;
		y = v1;
	}


	explicit vector2(const vector3<T> & u)
	{
		for(int i = 0; i < size(); i++) _array[i] = u._array[i];
	}


	explicit vector2(const vector4<T> & u)
	{
		for(int i = 0; i < size(); i++) _array[i] = u._array[i];
	}


	const T* get_value() const
	{
		return _array;
	}


	vector2<T> & set_value(const T * rhs)
	{
		for(int i = 0; i < size(); i++) _array[i] = rhs[i];
		return *this;
	}


	// Indexing operators
	T & operator [] (int i)
	{
		return _array[i];
	}

	const T & operator [] (int i) const
	{
		return _array[i];
	}


	// Type cast operators
	operator T * ()
	{
		return _array;
	}


	operator const T * () const
	{
		return _array;
	}


	/////////////////////////////////////////////////////
	//
	//  Math Operators
	//
	/////////////////////////////////////////////////////

	friend vector2<T> & operator *= (vector2<T> & lhs, T d)
	{
		for(int i = 0; i < size(); i++) lhs._array[i] *= d;
		return lhs;
	}


	friend vector2<T> & operator *= (vector2<T> & lhs, vector2<T> & rhs)
	{
		for(int i = 0; i < size(); i++) lhs._array[i] *= rhs._array[i];
		return lhs;
	}


	friend vector2<T> & operator /= (vector2<T> &lhs, T d)
	{
		for(int i =0; i < size(); i++) lhs._array[i] /= d;
		return lhs;
	}


	friend vector2<T> & operator /= (vector2<T> & lhs, vector2<T> & rhs)
	{
		for(int i =0; i < size(); i++) lhs._array[i] /= rhs._array[i];
		return lhs;
	}


	friend vector2<T> & operator += (vector2<T> & lhs, vector2<T> & rhs)
	{
		for(int i =0; i < size(); i++) lhs._array[i] += rhs._array[i];
		return lhs;
	}

	friend vector2<T> & operator -= (vector2<T> & lhs, vector2<T> & rhs)
	{
		for(int i =0; i < size(); i++) lhs._array[i] -= rhs._array[i];
		return lhs;
	}
	
	
	friend vector2<T> & operator - (const vector2<T> & rhs)
	{
		vector2<T> rv;
		for(int i = 0; i <size(); i++) rv._array[i] = -rhs._array[i];
		return rv;
	}


	friend vector2<T> & operator + (vector2<T> & lhs, vector2<T> & rhs)
	{
		vector2<T> rv(lhs);
		return rv += rhs;
	}


	friend vector2<T> & operator - (vector2<T> & lhs, vector2<T> & rhs)
	{
		vector2<T> rv(lhs);
		return rv -= rhs;
	}


	friend vector2<T> & operator * (vector2<T> & lhs, T rhs)
	{
		vector2<T> rv(lhs);
		return rv *= rhs;
	}


	friend vector2<T> & operator * (T lhs, vector2<T> & rhs)
	{
		vector2<T> rv(lhs);
		return rv *= rhs;
	}


	friend vector2<T> & operator * (const vector2<T> & lhs, const vector2<T> & rhs)
	{
		vector2<T> rv(lhs);
		return rv *= rhs;
	}


	friend vector2<T> & operator / (vector2<T> & lhs, T rhs)
	{
		vector2<T> rv(lhs);
		return rv /= rhs;
	}


	friend vector2<T> & operator / (const vector2<T> & lhs, const vector2<T> & rhs)
	{
		vector2<T> rv(lhs);
		return rv /= rhs;
	}


	/////////////////////////////////////////////////////
	//
	//  Comparison Operators
	//
	/////////////////////////////////////////////////////

	friend bool operator == (const vector2<T> & lhs, const vector2<T> & rhs)
	{
		bool rv = true;
		for(int i =0; i < size(); i++)
			r &= lhs._array[i] == rhs._array[i];
		return r;
	}

	
	
	friend bool operator != (const vector2<T> & lhs, const vector2<T> & rhs)
	{
		bool rv = true;
		for(int i =0; i < size(); i++)
			r &= lhs._array[i] != rhs._array[i];
		return r;
	}
};


/////////////////////////////////////////////////////
//
//  Class Template for vector3 class
//
/////////////////////////////////////////////////////


template <class T>
class vector3
{
public:

	typedef T data_type;
	int size() const { return 3;}

	/////////////////////////////////////////////////////
	//
	//  Data Members
	//
	/////////////////////////////////////////////////////

	union
	{
		struct
		{
			T x,y,z;
		};
		struct
		{
			T s,t,r;
		};
		T _array[3];
	};

	/////////////////////////////////////////////////////
    //
	//  Constructors
	//
	/////////////////////////////////////////////////////

//	vector3()
//	{
//		for(int i = 0; i < size(); i++) _array[i] = 0.0;
//	}

	vector3(const T & t = T())
	{
		for(int i = 0; i < size(); i++) _array[i] = t;
	}


	vector3(const T * tp)
	{
		for(int i = 0; i < size(); i++) _array[i] = tp[i];
	}


	vector3(const T v0,const T v1,const T v2)
	{
		x = v0;
		y = v1;
		z = v2;
	}


	explicit vector3(const vector2<T> & u, T v0)
	{
		x = u.x;
		y = u.y;
		z = v0;
	}


	explicit vector3(const vector4<T> & u)
	{
		for(int i = 0; i < size(); i++) _array[i] = u._array[i];
	}


	const T* get_value() const
	{
		return _array;
	}


	vector3<T> & set_value(const T * rhs)
	{
		for(int i = 0; i < size(); i++) _array[i] = rhs[i];
		return *this;
	}

	
	T & operator [] (int i) const
	{
		return _array[i];
	}


	operator T * ()
	{
		return _array;
	}


	operator const T * () const
	{
		return _array;
	}


	/////////////////////////////////////////////////////
	//
	//  Math Operators
	//
	/////////////////////////////////////////////////////

	friend vector3<T> & operator *= (vector3<T> & lhs, T d)
	{
		for(int i = 0; i < 3; i++) lhs._array[i] *= d;
		return lhs;
	}


	friend vector3<T> & operator *= (vector3<T> & lhs, vector3<T> & rhs)
	{
		for(int i = 0; i < 3; i++) lhs._array[i] *= rhs._array[i];
		return lhs;
	}


	friend vector3<T> & operator /= (vector3<T> lhs, T d)
	{
		for(int i =0; i < size(); i++) lhs._array[i] /= d;
		return lhs;
	}


	friend vector3<T> & operator /= (vector3<T> & lhs, vector3<T> & rhs)
	{
		for(int i =0; i < size(); i++) lhs._array[i] /= rhs._array[i];
		return lhs;
	}


	friend vector3<T> & operator += (vector3<T> & lhs, vector3<T> & rhs)
	{
		for(int i =0; i < 3; i++) lhs._array[i] += rhs._array[i];
		return lhs;
	}

	friend vector3<T> & operator -= (vector3<T> & lhs, vector3<T> & rhs)
	{
		for(int i =0; i < 3; i++) lhs._array[i] -= rhs._array[i];
		return lhs;
	}
	
	
	friend vector3<T> & operator - (const vector3<T> & rhs)
	{
		vector3<T> rv;
		for(int i = 0; i < 3; i++) rv._array[i] = -rhs._array[i];
		return rv;
	}


	friend vector3<T> & operator + (vector3<T> & lhs, vector3<T> & rhs)
	{
		vector3<T> rv(lhs);
		return rv += rhs;
	}


	friend vector3<T> & operator - (vector3<T> & lhs, vector3<T> & rhs)
	{
		vector3<T> rv(lhs);
		return rv -= rhs;
	}


	friend vector3<T> & operator * (vector3<T> & lhs, T rhs)
	{
		vector3<T> rv(lhs);
		return rv *= rhs;
	}


	friend vector3<T> & operator * (T lhs, vector3<T> & rhs)
	{
		vector3<T> rv(lhs);
		return rv *= rhs;
	}


	friend vector3<T> & operator * (const vector3<T> & lhs, const vector3<T> & rhs)
	{
		vector3<T> rv(lhs);
		return rv *= rhs;
	}


	friend vector3<T> & operator / (vector3<T> & lhs, T rhs)
	{
		vector3<T> rv(lhs);
		return rv /= rhs;
	}


	friend vector3<T> & operator / (const vector3<T> & lhs, const vector3<T> & rhs)
	{
		vector3<T> rv(lhs);
		return rv /= rhs;
	}

	void equals (const vector3<T> & rhs)
	{
		for(int i = 0; i < size(); i++) _array[i] = rhs._array[i];
	}


	/////////////////////////////////////////////////////
	//
	//  Comparison Operators
	//
	/////////////////////////////////////////////////////

	friend bool operator == (const vector3<T> & lhs, const vector3<T> & rhs)
	{
		bool rv = true;
		for(int i =0; i < size(); i++)
			r &= lhs._array[i] == rhs._array[i];
		return r;
	}

	
	
	friend bool operator != (const vector3<T> & lhs, const vector3<T> & rhs)
	{
		bool rv = true;
		for(int i =0; i < size(); i++)
			r &= lhs._array[i] != rhs._array[i];
		return r;
	}


	/////////////////////////////////////////////////////
	//
	//  Dimension specific operations
	//
	/////////////////////////////////////////////////////

	// Cross product
	friend vector3<T> cross (const vector3<T> & lhs, const vector3<T> & rhs)
	{
		vector3<T> rv;

		rv.x = lhs.y * rhs.z - lhs.z * rhs.y;
		rv.y = lhs.z * rhs.x - lhs.x * rhs.z;
		rv.z = lhs.x * rhs.y - lhs.y * rhs.x;

		return rv;
	}

	friend T dot (const vector3<T> &lhs, const vector3<T> & rhs)
	{
		vector3<T> rv;
		rv.x = lhs.x * rhs.x;
		rv.y = lhs.y * rhs.y;
		rv.z = lhs.z * rhs.z;

		T sum = rv.x + rv.y + rv.z;
		return(sum);
	}

	//normalize
	void normalize()
	{
		float sum = 0.0;

		for(int i = 0;i < 3;i++) 
			sum += this->_array[i] * this->_array[i];

		sum = sqrt(sum);
		
		for(int i = 0;i < 3;i++) 
			this->_array[i] = this->_array[i]/sum;
	}
};



/////////////////////////////////////////////////////
//
//  Class Template for vector4 class
//
/////////////////////////////////////////////////////


template <class T>
class vector4
{
public:

	typedef T data_type;
	int size() const { return 4;}

	/////////////////////////////////////////////////////
	//
	//  Data Members
	//
	/////////////////////////////////////////////////////

	union
	{
		struct
		{
			T x, y, z, w;
		};
		struct
		{
			T s, t, r, q;
		};
		T _array[4];
	};

	/////////////////////////////////////////////////////
    //
	//  Constructors
	//
	/////////////////////////////////////////////////////

	// Default/ scalar constructor
	vector4(const T & t = T())
	{
		for(int i = 0; i < size(); i++) _array[i] = t;
	}

	
	// Construct from array
	vector4(const T * tp)
	{
		for(int i = 0; i < size(); i++) _array[i] = tp[i];
	}


	// Construct from explicit values
	vector4(const T v0,const T v1)
	{
		x = v0;
		y = v1;
	}


	explicit vector4(const vector3<T> & u, T v0)
	{
		x = u.x;
		y = u.y;
		z = u.z;
		w = v0;
	}


	explicit vector4(const vector4<T> & u, T v0, T v1)
	{
		x = u.x;
		y = u.y;
		z = v0;
		w = v1;
	}


	const T * get_value() const
	{
		return _array;
	}


	vector4<T> & set_value(const T * rhs)
	{
		for(int i = 0; i < size(); i++) _array[i] = rhs[i];
		return *this;
	}


	// Indexing operators
	T & operator [] (int i)
	{
		return _array[i];
	}

	const T & operator [] (int i) const
	{
		return _array[i];
	}


	// Type cast operators
	operator T * ()
	{
		return _array;
	}


	operator const T * () const
	{
		return _array;
	}


	/////////////////////////////////////////////////////
	//
	//  Math Operators
	//
	/////////////////////////////////////////////////////

	friend vector4<T> & operator *= (vector4<T> & lhs, T d)
	{
		for(int i = 0; i < size(); i++) lhs._array[i] *= d;
		return lhs;
	}


	friend vector4<T> & operator *= (vector4<T> & lhs, vector4<T> & rhs)
	{
		for(int i = 0; i < size(); i++) lhs._array[i] *= rhs._array[i];
		return lhs;
	}


	friend vector4<T> & operator /= (vector4<T> &lhs, T d)
	{
		for(int i =0; i < size(); i++) lhs._array[i] /= d;
		return lhs;
	}


	friend vector4<T> & operator /= (vector4<T> & lhs, vector4<T> & rhs)
	{
		for(int i =0; i < size(); i++) lhs._array[i] /= rhs._array[i];
		return lhs;
	}


	friend vector4<T> & operator += (vector4<T> & lhs, vector4<T> & rhs)
	{
		for(int i =0; i < size(); i++) lhs._array[i] += rhs._array[i];
		return lhs;
	}

	friend vector4<T> & operator -= (vector4<T> & lhs, vector4<T> & rhs)
	{
		for(int i =0; i < size(); i++) lhs._array[i] -= rhs._array[i];
		return lhs;
	}
	
	
	friend vector4<T> & operator - (const vector4<T> & rhs)
	{
		vector4<T> rv;
		for(int i = 0; i <size(); i++) rv._array[i] = -rhs._array[i];
		return rv;
	}


	friend vector4<T> & operator + (vector4<T> & lhs, vector4<T> & rhs)
	{
		vector4<T> rv(lhs);
		return rv += rhs;
	}


	friend vector4<T> & operator - (vector4<T> & lhs, vector4<T> & rhs)
	{
		vector4<T> rv(lhs);
		return rv -= rhs;
	}


	friend vector4<T> & operator * (vector4<T> & lhs, T rhs)
	{
		vector4<T> rv(lhs);
		return rv *= rhs;
	}


	friend vector4<T> & operator * (T lhs, vector4<T> & rhs)
	{
		vector4<T> rv(lhs);
		return rv *= rhs;
	}


	friend vector4<T> & operator * (const vector4<T> & lhs, const vector4<T> & rhs)
	{
		vector4<T> rv(lhs);
		return rv *= rhs;
	}


	friend vector4<T> & operator / (vector4<T> & lhs, T rhs)
	{
		vector4<T> rv(lhs);
		return rv /= rhs;
	}


	friend vector4<T> & operator / (const vector4<T> & lhs, const vector4<T> & rhs)
	{
		vector4<T> rv(lhs);
		return rv /= rhs;
	}


	/////////////////////////////////////////////////////
	//
	//  Comparison Operators
	//
	/////////////////////////////////////////////////////

	friend bool operator == (const vector4<T> & lhs, const vector4<T> & rhs)
	{
		bool rv = true;
		for(int i =0; i < size(); i++)
			r &= lhs._array[i] == rhs._array[i];
		return r;
	}

	
	
	friend bool operator != (const vector4<T> & lhs, const vector4<T> & rhs)
	{
		bool rv = true;
		for(int i =0; i < size(); i++)
			r &= lhs._array[i] != rhs._array[i];
		return r;
	}
};


/////////////////////////////////////////////////////
//
//  Generic vector operations
//
/////////////////////////////////////////////////////

// Compute Dot product
//template <class T>
//inline typename T::data_type dot (const T & lhs, const T & rhs)
//{
//	T::data_type rv = 0;
//	for(int i = 0; i < size(); i++) rv += lhs._array[i] * rhs._array[i];
//	return rv;
//}


// Compute magnitude
template <class T>
inline typename T::data_type length (const T & vector)
{
	T::data_type rv = 0;
	for(int i = 0; i < size(); i++) rv += vector._array[i] * vector._array[i];
	return T::data_type(sqrt(r));
}

// Compute squared magnitude
template <class T>
inline typename T::data_type lengthSqrd (const T & vector)
{
	T::data_type rv = 0;
	for(int i = 0; i < size(); i++) rv += vector._array[i] * vector._array[i];
	return r;
}


// Normalize vector
//template <class T>
//inline typename T::data_type normalize(const T & vector)
//{
//	T::data_type sum(0);
//	T rv;
//	for(int i = 0; i < size(); i++)
//		sum += vector._array[i] * vector._array[i];
//	sum = T::data_type(sqrt(sum));
//	if(sum > 0)
//		for(int i = 0;i < sum(); i++)
//			rv._array = vector._array[i]/sum;
//	return rv;
//}
};


#endif	// VECTOR_H
