#ifndef VECTOR_4F_H
#define VECTOR_4F_H

class Vector2f;
class Vector3f;

class Vector4f
{
public:

	Vector4f( double f = 0.f );
	Vector4f( double fx, double fy, double fz, double fw );
	Vector4f( double buffer[ 4 ] );

	Vector4f( const Vector2f& xy, double z, double w );
	Vector4f( double x, const Vector2f& yz, double w );
	Vector4f( double x, double y, const Vector2f& zw );
	Vector4f( const Vector2f& xy, const Vector2f& zw );

	Vector4f( const Vector3f& xyz, double w );
	Vector4f( double x, const Vector3f& yzw );

	// copy constructors
	Vector4f( const Vector4f& rv );

	// assignment operators
	Vector4f& operator = ( const Vector4f& rv );

	// no destructor necessary

	// returns the ith element
	const double& operator [] ( int i ) const;
	double& operator [] ( int i );

	double& x();
	double& y();
	double& z();
	double& w();

	double x() const;
	double y() const;
	double z() const;
	double w() const;

	Vector2f xy() const;
	Vector2f yz() const;
	Vector2f zw() const;
	Vector2f wx() const;

	Vector3f xyz() const;
	Vector3f yzw() const;
	Vector3f zwx() const;
	Vector3f wxy() const;

	Vector3f xyw() const;
	Vector3f yzx() const;
	Vector3f zwy() const;
	Vector3f wxz() const;

	double abs() const;
	double absSquared() const;
	void normalize();
	Vector4f normalized() const;

	// if v.z != 0, v = v / v.w
	void homogenize();
	Vector4f homogenized() const;

	void negate();

	// ---- Utility ----
	operator const double* () const; // automatic type conversion for OpenGL
	operator double* (); // automatic type conversion for OpenG
	void print() const; 

	static double dot( const Vector4f& v0, const Vector4f& v1 );
	static Vector4f lerp( const Vector4f& v0, const Vector4f& v1, double alpha );

private:

	double m_elements[ 4 ];

};

// component-wise operators
Vector4f operator + ( const Vector4f& v0, const Vector4f& v1 );
Vector4f operator - ( const Vector4f& v0, const Vector4f& v1 );
Vector4f operator * ( const Vector4f& v0, const Vector4f& v1 );
Vector4f operator / ( const Vector4f& v0, const Vector4f& v1 );

// unary negation
Vector4f operator - ( const Vector4f& v );

// multiply and divide by scalar
Vector4f operator * ( double f, const Vector4f& v );
Vector4f operator * ( const Vector4f& v, double f );
Vector4f operator / ( const Vector4f& v, double f );

bool operator == ( const Vector4f& v0, const Vector4f& v1 );
bool operator != ( const Vector4f& v0, const Vector4f& v1 );

#endif // VECTOR_4F_H
