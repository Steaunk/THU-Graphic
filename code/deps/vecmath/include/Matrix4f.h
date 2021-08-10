#ifndef MATRIX4F_H
#define MATRIX4F_H

#include <cstdio>

class Matrix2f;
class Matrix3f;
class Quat4f;
class Vector3f;
class Vector4f;

// 4x4 Matrix, stored in column major order (OpenGL style)
class Matrix4f
{
public:

    // Fill a 4x4 matrix with "fill".  Default to 0.
	Matrix4f( double fill = 0.f );
	Matrix4f( double m00, double m01, double m02, double m03,
		double m10, double m11, double m12, double m13,
		double m20, double m21, double m22, double m23,
		double m30, double m31, double m32, double m33 );
	
	// setColumns = true ==> sets the columns of the matrix to be [v0 v1 v2 v3]
	// otherwise, sets the rows
	Matrix4f( const Vector4f& v0, const Vector4f& v1, const Vector4f& v2, const Vector4f& v3, bool setColumns = true );
	
	Matrix4f( const Matrix4f& rm ); // copy constructor
	Matrix4f& operator = ( const Matrix4f& rm ); // assignment operator
	Matrix4f& operator/=(double d);
	// no destructor necessary

	const double& operator () ( int i, int j ) const;
	double& operator () ( int i, int j );

	Vector4f getRow( int i ) const;
	void setRow( int i, const Vector4f& v );

	// get column j (mod 4)
	Vector4f getCol( int j ) const;
	void setCol( int j, const Vector4f& v );

	// gets the 2x2 submatrix of this matrix to m
	// starting with upper left corner at (i0, j0)
	Matrix2f getSubmatrix2x2( int i0, int j0 ) const;

	// gets the 3x3 submatrix of this matrix to m
	// starting with upper left corner at (i0, j0)
	Matrix3f getSubmatrix3x3( int i0, int j0 ) const;

	// sets a 2x2 submatrix of this matrix to m
	// starting with upper left corner at (i0, j0)
	void setSubmatrix2x2( int i0, int j0, const Matrix2f& m );

	// sets a 3x3 submatrix of this matrix to m
	// starting with upper left corner at (i0, j0)
	void setSubmatrix3x3( int i0, int j0, const Matrix3f& m );

	double determinant() const;
	Matrix4f inverse( bool* pbIsSingular = NULL, double epsilon = 0.f ) const;

	void transpose();
	Matrix4f transposed() const;

	// ---- Utility ----
	operator double* (); // automatic type conversion for GL
	operator const double* () const; // automatic type conversion for GL
	
	void print();

	static Matrix4f ones();
	static Matrix4f identity();
	static Matrix4f translation( double x, double y, double z );
	static Matrix4f translation( const Vector3f& rTranslation );
	static Matrix4f rotateX( double radians );
	static Matrix4f rotateY( double radians );
	static Matrix4f rotateZ( double radians );
	static Matrix4f rotation( const Vector3f& rDirection, double radians );
	static Matrix4f scaling( double sx, double sy, double sz );
	static Matrix4f uniformScaling( double s );
	static Matrix4f lookAt( const Vector3f& eye, const Vector3f& center, const Vector3f& up );
	static Matrix4f orthographicProjection( double width, double height, double zNear, double zFar, bool directX );
	static Matrix4f orthographicProjection( double left, double right, double bottom, double top, double zNear, double zFar, bool directX );
	static Matrix4f perspectiveProjection( double fLeft, double fRight, double fBottom, double fTop, double fZNear, double fZFar, bool directX );
	static Matrix4f perspectiveProjection( double fovYRadians, double aspect, double zNear, double zFar, bool directX );
	static Matrix4f infinitePerspectiveProjection( double fLeft, double fRight, double fBottom, double fTop, double fZNear, bool directX );

	// Returns the rotation matrix represented by a quaternion
	// uses a normalized version of q
	static Matrix4f rotation( const Quat4f& q );

	// returns an orthogonal matrix that's a uniformly distributed rotation
	// given u[i] is a uniformly distributed random number in [0,1]
	static Matrix4f randomRotation( double u0, double u1, double u2 );

private:

	double m_elements[ 16 ];

};

// Matrix-Vector multiplication
// 4x4 * 4x1 ==> 4x1
Vector4f operator * ( const Matrix4f& m, const Vector4f& v );

// Matrix-Matrix multiplication
Matrix4f operator * ( const Matrix4f& x, const Matrix4f& y );

#endif // MATRIX4F_H
