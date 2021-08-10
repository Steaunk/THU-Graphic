#ifndef MATRIX3F_H
#define MATRIX3F_H

#include <cstdio>

class Matrix2f;
class Quat4f;
class Vector3f;

// 3x3 Matrix, stored in column major order (OpenGL style)
class Matrix3f
{
public:

    // Fill a 3x3 matrix with "fill", default to 0.
	Matrix3f( double fill = 0.f );
	Matrix3f( double m00, double m01, double m02,
		double m10, double m11, double m12,
		double m20, double m21, double m22 );

	// setColumns = true ==> sets the columns of the matrix to be [v0 v1 v2]
	// otherwise, sets the rows
	Matrix3f( const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, bool setColumns = true );

	Matrix3f( const Matrix3f& rm ); // copy constructor
	Matrix3f& operator = ( const Matrix3f& rm ); // assignment operator
	// no destructor necessary

	const double& operator () ( int i, int j ) const;
	double& operator () ( int i, int j );

	Vector3f getRow( int i ) const;
	void setRow( int i, const Vector3f& v );

	Vector3f getCol( int j ) const;
	void setCol( int j, const Vector3f& v );

	// gets the 2x2 submatrix of this matrix to m
	// starting with upper left corner at (i0, j0)
	Matrix2f getSubmatrix2x2( int i0, int j0 ) const;

	// sets a 2x2 submatrix of this matrix to m
	// starting with upper left corner at (i0, j0)
	void setSubmatrix2x2( int i0, int j0, const Matrix2f& m );

	double determinant() const;
	Matrix3f inverse( bool* pbIsSingular = NULL, double epsilon = 0.f ) const; // TODO: invert in place as well

	void transpose();
	Matrix3f transposed() const;

	// ---- Utility ----
	operator double* (); // automatic type conversion for GL
	void print();

	static double determinant3x3( double m00, double m01, double m02,
		double m10, double m11, double m12,
		double m20, double m21, double m22 );

	static Matrix3f ones();
	static Matrix3f identity();
	static Matrix3f rotateX( double radians );
	static Matrix3f rotateY( double radians );
	static Matrix3f rotateZ( double radians );
	static Matrix3f scaling( double sx, double sy, double sz );
	static Matrix3f uniformScaling( double s );
	static Matrix3f rotation( const Vector3f& rDirection, double radians );

	// Returns the rotation matrix represented by a unit quaternion
	// if q is not normalized, it it normalized first
	static Matrix3f rotation( const Quat4f& rq );

private:

	double m_elements[ 9 ];

};

// Matrix-Vector multiplication
// 3x3 * 3x1 ==> 3x1
Vector3f operator * ( const Matrix3f& m, const Vector3f& v );

// Matrix-Matrix multiplication
Matrix3f operator * ( const Matrix3f& x, const Matrix3f& y );

#endif // MATRIX3F_H
