#ifndef VECTOR_3F_H
#define VECTOR_3F_H

class Vector2f;

class Vector3f
{
public:

	static const Vector3f ZERO;
	static const Vector3f UP;
	static const Vector3f RIGHT;
	static const Vector3f FORWARD;

    Vector3f( double f = 0.f );
    Vector3f( double x, double y, double z );

	Vector3f( const Vector2f& xy, double z );
	Vector3f( double x, const Vector2f& yz );

	// copy constructors
    Vector3f( const Vector3f& rv );

	// assignment operators
    Vector3f& operator = ( const Vector3f& rv );

	// no destructor necessary

	// returns the ith element
    const double& operator [] ( int i ) const;
    double& operator [] ( int i );

    double& x();
	double& y();
	double& z();

	double x() const;
	double y() const;
	double z() const;

	Vector2f xy() const;
	Vector2f xz() const;
	Vector2f yz() const;

	Vector3f xyz() const;
	Vector3f yzx() const;
	Vector3f zxy() const;

	double length() const;
    double squaredLength() const;

	void normalize();
	Vector3f normalized() const;

	Vector3f clamp() const;

	Vector2f homogenized() const;

	Vector3f pow( const float & k) const;

	void negate();

	// ---- Utility ----
    operator const double* () const; // automatic type conversion for OpenGL
    operator double* (); // automatic type conversion for OpenGL 
	void print() const;	

	Vector3f& operator += ( const Vector3f& v );
	Vector3f& operator -= ( const Vector3f& v );
    Vector3f& operator *= ( double f );
	Vector3f& operator /= ( double f );

    static double dot( const Vector3f& v0, const Vector3f& v1 );
	static Vector3f cross( const Vector3f& v0, const Vector3f& v1 );
    
    // computes the linear interpolation between v0 and v1 by alpha \in [0,1]
	// returns v0 * ( 1 - alpha ) * v1 * alpha
	static Vector3f lerp( const Vector3f& v0, const Vector3f& v1, double alpha );

	// computes the cubic catmull-rom interpolation between p0, p1, p2, p3
    // by t \in [0,1].  Guarantees that at t = 0, the result is p0 and
    // at p1, the result is p2.
	static Vector3f cubicInterpolate( const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, double t );

private:

	double m_elements[ 3 ];

};

// component-wise operators
Vector3f operator + ( const Vector3f& v0, const Vector3f& v1 );
Vector3f operator - ( const Vector3f& v0, const Vector3f& v1 );
Vector3f operator * ( const Vector3f& v0, const Vector3f& v1 );
Vector3f operator / ( const Vector3f& v0, const Vector3f& v1 );

// unary negation
Vector3f operator - ( const Vector3f& v );

// multiply and divide by scalar
Vector3f operator * ( double f, const Vector3f& v );
Vector3f operator * ( const Vector3f& v, double f );
Vector3f operator / ( const Vector3f& v, double f );

bool operator == ( const Vector3f& v0, const Vector3f& v1 );
bool operator != ( const Vector3f& v0, const Vector3f& v1 );

#endif // VECTOR_3F_H
