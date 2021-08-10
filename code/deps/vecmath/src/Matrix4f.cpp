#include "Matrix4f.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>

#include "Matrix2f.h"
#include "Matrix3f.h"
#include "Quat4f.h"
#include "Vector3f.h"
#include "Vector4f.h"

Matrix4f::Matrix4f( double fill )
{
	for( int i = 0; i < 16; ++i )
	{
		m_elements[ i ] = fill;
	}
}

Matrix4f::Matrix4f( double m00, double m01, double m02, double m03,
				   double m10, double m11, double m12, double m13,
				   double m20, double m21, double m22, double m23,
				   double m30, double m31, double m32, double m33 )
{
	m_elements[ 0 ] = m00;
	m_elements[ 1 ] = m10;
	m_elements[ 2 ] = m20;
	m_elements[ 3 ] = m30;
	
	m_elements[ 4 ] = m01;
	m_elements[ 5 ] = m11;
	m_elements[ 6 ] = m21;
	m_elements[ 7 ] = m31;

	m_elements[ 8 ] = m02;
	m_elements[ 9 ] = m12;
	m_elements[ 10 ] = m22;
	m_elements[ 11 ] = m32;

	m_elements[ 12 ] = m03;
	m_elements[ 13 ] = m13;
	m_elements[ 14 ] = m23;
	m_elements[ 15 ] = m33;
}

Matrix4f& Matrix4f::operator/=(double d)
{
	for(int ii=0;ii<16;ii++){
		m_elements[ii]/=d;
	}
	return *this;
}

Matrix4f::Matrix4f( const Vector4f& v0, const Vector4f& v1, const Vector4f& v2, const Vector4f& v3, bool setColumns )
{
	if( setColumns )
	{
		setCol( 0, v0 );
		setCol( 1, v1 );
		setCol( 2, v2 );
		setCol( 3, v3 );
	}
	else
	{
		setRow( 0, v0 );
		setRow( 1, v1 );
		setRow( 2, v2 );
		setRow( 3, v3 );
	}
}

Matrix4f::Matrix4f( const Matrix4f& rm )
{
	memcpy( m_elements, rm.m_elements, 16 * sizeof( double ) );
}

Matrix4f& Matrix4f::operator = ( const Matrix4f& rm )
{
	if( this != &rm )
	{
		memcpy( m_elements, rm.m_elements, 16 * sizeof( double ) );
	}
	return *this;
}

const double& Matrix4f::operator () ( int i, int j ) const
{
	return m_elements[ j * 4 + i ];
}

double& Matrix4f::operator () ( int i, int j )
{
	return m_elements[ j * 4 + i ];
}

Vector4f Matrix4f::getRow( int i ) const
{
	return Vector4f
	(
		m_elements[ i ],
		m_elements[ i + 4 ],
		m_elements[ i + 8 ],
		m_elements[ i + 12 ]
	);
}

void Matrix4f::setRow( int i, const Vector4f& v )
{
	m_elements[ i ] = v.x();
	m_elements[ i + 4 ] = v.y();
	m_elements[ i + 8 ] = v.z();
	m_elements[ i + 12 ] = v.w();
}

Vector4f Matrix4f::getCol( int j ) const
{
	int colStart = 4 * j;

	return Vector4f
	(
		m_elements[ colStart ],
		m_elements[ colStart + 1 ],
		m_elements[ colStart + 2 ],
		m_elements[ colStart + 3 ]
	);
}

void Matrix4f::setCol( int j, const Vector4f& v )
{
	int colStart = 4 * j;

	m_elements[ colStart ] = v.x();
	m_elements[ colStart + 1 ] = v.y();
	m_elements[ colStart + 2 ] = v.z();
	m_elements[ colStart + 3 ] = v.w();
}

Matrix2f Matrix4f::getSubmatrix2x2( int i0, int j0 ) const
{
	Matrix2f out;

	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < 2; ++j )
		{
			out( i, j ) = ( *this )( i + i0, j + j0 );
		}
	}

	return out;
}

Matrix3f Matrix4f::getSubmatrix3x3( int i0, int j0 ) const
{
	Matrix3f out;

	for( int i = 0; i < 3; ++i )
	{
		for( int j = 0; j < 3; ++j )
		{
			out( i, j ) = ( *this )( i + i0, j + j0 );
		}
	}

	return out;
}

void Matrix4f::setSubmatrix2x2( int i0, int j0, const Matrix2f& m )
{
	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < 2; ++j )
		{
			( *this )( i + i0, j + j0 ) = m( i, j );
		}
	}
}

void Matrix4f::setSubmatrix3x3( int i0, int j0, const Matrix3f& m )
{
	for( int i = 0; i < 3; ++i )
	{
		for( int j = 0; j < 3; ++j )
		{
			( *this )( i + i0, j + j0 ) = m( i, j );
		}
	}
}

double Matrix4f::determinant() const
{
	double m00 = m_elements[ 0 ];
	double m10 = m_elements[ 1 ];
	double m20 = m_elements[ 2 ];
	double m30 = m_elements[ 3 ];

	double m01 = m_elements[ 4 ];
	double m11 = m_elements[ 5 ];
	double m21 = m_elements[ 6 ];
	double m31 = m_elements[ 7 ];

	double m02 = m_elements[ 8 ];
	double m12 = m_elements[ 9 ];
	double m22 = m_elements[ 10 ];
	double m32 = m_elements[ 11 ];

	double m03 = m_elements[ 12 ];
	double m13 = m_elements[ 13 ];
	double m23 = m_elements[ 14 ];
	double m33 = m_elements[ 15 ];

	double cofactor00 =  Matrix3f::determinant3x3( m11, m12, m13, m21, m22, m23, m31, m32, m33 );
	double cofactor01 = -Matrix3f::determinant3x3( m12, m13, m10, m22, m23, m20, m32, m33, m30 );
	double cofactor02 =  Matrix3f::determinant3x3( m13, m10, m11, m23, m20, m21, m33, m30, m31 );
	double cofactor03 = -Matrix3f::determinant3x3( m10, m11, m12, m20, m21, m22, m30, m31, m32 );

	return( m00 * cofactor00 + m01 * cofactor01 + m02 * cofactor02 + m03 * cofactor03 );
}

Matrix4f Matrix4f::inverse( bool* pbIsSingular, double epsilon ) const
{
	double m00 = m_elements[ 0 ];
	double m10 = m_elements[ 1 ];
	double m20 = m_elements[ 2 ];
	double m30 = m_elements[ 3 ];

	double m01 = m_elements[ 4 ];
	double m11 = m_elements[ 5 ];
	double m21 = m_elements[ 6 ];
	double m31 = m_elements[ 7 ];

	double m02 = m_elements[ 8 ];
	double m12 = m_elements[ 9 ];
	double m22 = m_elements[ 10 ];
	double m32 = m_elements[ 11 ];

	double m03 = m_elements[ 12 ];
	double m13 = m_elements[ 13 ];
	double m23 = m_elements[ 14 ];
	double m33 = m_elements[ 15 ];

    double cofactor00 =  Matrix3f::determinant3x3( m11, m12, m13, m21, m22, m23, m31, m32, m33 );
    double cofactor01 = -Matrix3f::determinant3x3( m12, m13, m10, m22, m23, m20, m32, m33, m30 );
    double cofactor02 =  Matrix3f::determinant3x3( m13, m10, m11, m23, m20, m21, m33, m30, m31 );
    double cofactor03 = -Matrix3f::determinant3x3( m10, m11, m12, m20, m21, m22, m30, m31, m32 );
    
    double cofactor10 = -Matrix3f::determinant3x3( m21, m22, m23, m31, m32, m33, m01, m02, m03 );
    double cofactor11 =  Matrix3f::determinant3x3( m22, m23, m20, m32, m33, m30, m02, m03, m00 );
    double cofactor12 = -Matrix3f::determinant3x3( m23, m20, m21, m33, m30, m31, m03, m00, m01 );
    double cofactor13 =  Matrix3f::determinant3x3( m20, m21, m22, m30, m31, m32, m00, m01, m02 );
    
    double cofactor20 =  Matrix3f::determinant3x3( m31, m32, m33, m01, m02, m03, m11, m12, m13 );
    double cofactor21 = -Matrix3f::determinant3x3( m32, m33, m30, m02, m03, m00, m12, m13, m10 );
    double cofactor22 =  Matrix3f::determinant3x3( m33, m30, m31, m03, m00, m01, m13, m10, m11 );
    double cofactor23 = -Matrix3f::determinant3x3( m30, m31, m32, m00, m01, m02, m10, m11, m12 );
    
    double cofactor30 = -Matrix3f::determinant3x3( m01, m02, m03, m11, m12, m13, m21, m22, m23 );
    double cofactor31 =  Matrix3f::determinant3x3( m02, m03, m00, m12, m13, m10, m22, m23, m20 );
    double cofactor32 = -Matrix3f::determinant3x3( m03, m00, m01, m13, m10, m11, m23, m20, m21 );
    double cofactor33 =  Matrix3f::determinant3x3( m00, m01, m02, m10, m11, m12, m20, m21, m22 );

	double determinant = m00 * cofactor00 + m01 * cofactor01 + m02 * cofactor02 + m03 * cofactor03;

	bool isSingular = ( fabs( determinant ) < epsilon );
	if( isSingular )
	{
		if( pbIsSingular != NULL )
		{
			*pbIsSingular = true;
		}
		return Matrix4f();
	}
	else
	{
		if( pbIsSingular != NULL )
		{
			*pbIsSingular = false;
		}

		double reciprocalDeterminant = 1.0f / determinant;

		return Matrix4f
			(
				cofactor00 * reciprocalDeterminant, cofactor10 * reciprocalDeterminant, cofactor20 * reciprocalDeterminant, cofactor30 * reciprocalDeterminant,
				cofactor01 * reciprocalDeterminant, cofactor11 * reciprocalDeterminant, cofactor21 * reciprocalDeterminant, cofactor31 * reciprocalDeterminant,
				cofactor02 * reciprocalDeterminant, cofactor12 * reciprocalDeterminant, cofactor22 * reciprocalDeterminant, cofactor32 * reciprocalDeterminant,
				cofactor03 * reciprocalDeterminant, cofactor13 * reciprocalDeterminant, cofactor23 * reciprocalDeterminant, cofactor33 * reciprocalDeterminant
			);
	}
}

void Matrix4f::transpose()
{
	double temp;

	for( int i = 0; i < 3; ++i )
	{
		for( int j = i + 1; j < 4; ++j )
		{
			temp = ( *this )( i, j );
			( * this )( i, j ) = ( *this )( j, i );
			( *this )( j, i ) = temp;
		}
	}
}

Matrix4f Matrix4f::transposed() const
{
	Matrix4f out;
	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			out( j, i ) = ( *this )( i, j );
		}
	}

	return out;
}

Matrix4f::operator double* ()
{
	return m_elements;
}

Matrix4f::operator const double* ()const
{
	return m_elements;
}


void Matrix4f::print()
{
	printf( "[ %.4f %.4f %.4f %.4f ]\n[ %.4f %.4f %.4f %.4f ]\n[ %.4f %.4f %.4f %.4f ]\n[ %.4f %.4f %.4f %.4f ]\n",
		m_elements[ 0 ], m_elements[ 4 ], m_elements[ 8 ], m_elements[ 12 ],
		m_elements[ 1 ], m_elements[ 5 ], m_elements[ 9 ], m_elements[ 13 ],
		m_elements[ 2 ], m_elements[ 6 ], m_elements[ 10], m_elements[ 14 ],
		m_elements[ 3 ], m_elements[ 7 ], m_elements[ 11], m_elements[ 15 ] );
}

// static
Matrix4f Matrix4f::ones()
{
	Matrix4f m;
	for( int i = 0; i < 16; ++i )
	{
		m.m_elements[ i ] = 1;
	}

	return m;
}

// static
Matrix4f Matrix4f::identity()
{
	Matrix4f m;
	
	m( 0, 0 ) = 1;
	m( 1, 1 ) = 1;
	m( 2, 2 ) = 1;
	m( 3, 3 ) = 1;

	return m;
}

// static
Matrix4f Matrix4f::translation( double x, double y, double z )
{
	return Matrix4f
	(
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1
	);
}

// static
Matrix4f Matrix4f::translation( const Vector3f& rTranslation )
{
	return Matrix4f
	(
		1, 0, 0, rTranslation.x(),
		0, 1, 0, rTranslation.y(),
		0, 0, 1, rTranslation.z(),
		0, 0, 0, 1
	);
}

// static
Matrix4f Matrix4f::rotateX( double radians )
{
	double c = cos( radians );
	double s = sin( radians );

	return Matrix4f
	(
		1, 0, 0, 0,
		0, c, -s, 0,
		0, s, c, 0,
		0, 0, 0, 1
	);
}

// static
Matrix4f Matrix4f::rotateY( double radians )
{
	double c = cos( radians );
	double s = sin( radians );

	return Matrix4f
	(
		c, 0, s, 0,
		0, 1, 0, 0,
		-s, 0, c, 0,
		0, 0, 0, 1
	);
}

// static
Matrix4f Matrix4f::rotateZ( double radians )
{
	double c = cos( radians );
	double s = sin( radians );

	return Matrix4f
	(
		c, -s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

// static
Matrix4f Matrix4f::rotation( const Vector3f& rDirection, double radians )
{
	Vector3f normalizedDirection = rDirection.normalized();
	
	double cosTheta = cos( radians );
	double sinTheta = sin( radians );

	double x = normalizedDirection.x();
	double y = normalizedDirection.y();
	double z = normalizedDirection.z();

	return Matrix4f
	(
		x * x * ( 1.0f - cosTheta ) + cosTheta,			y * x * ( 1.0f - cosTheta ) - z * sinTheta,		z * x * ( 1.0f - cosTheta ) + y * sinTheta,		0.0f,
		x * y * ( 1.0f - cosTheta ) + z * sinTheta,		y * y * ( 1.0f - cosTheta ) + cosTheta,			z * y * ( 1.0f - cosTheta ) - x * sinTheta,		0.0f,
		x * z * ( 1.0f - cosTheta ) - y * sinTheta,		y * z * ( 1.0f - cosTheta ) + x * sinTheta,		z * z * ( 1.0f - cosTheta ) + cosTheta,			0.0f,
		0.0f,											0.0f,											0.0f,											1.0f
	);
}

// static
Matrix4f Matrix4f::rotation( const Quat4f& q )
{
	Quat4f qq = q.normalized();

	double xx = qq.x() * qq.x();
	double yy = qq.y() * qq.y();
	double zz = qq.z() * qq.z();

	double xy = qq.x() * qq.y();
	double zw = qq.z() * qq.w();

	double xz = qq.x() * qq.z();
	double yw = qq.y() * qq.w();

	double yz = qq.y() * qq.z();
	double xw = qq.x() * qq.w();

	return Matrix4f
	(
		1.0f - 2.0f * ( yy + zz ),		2.0f * ( xy - zw ),				2.0f * ( xz + yw ),				0.0f,
		2.0f * ( xy + zw ),				1.0f - 2.0f * ( xx + zz ),		2.0f * ( yz - xw ),				0.0f,
		2.0f * ( xz - yw ),				2.0f * ( yz + xw ),				1.0f - 2.0f * ( xx + yy ),		0.0f,
		0.0f,							0.0f,							0.0f,							1.0f
	);
}

// static
Matrix4f Matrix4f::scaling( double sx, double sy, double sz )
{
	return Matrix4f
	(
		sx, 0, 0, 0,
		0, sy, 0, 0,
		0, 0, sz, 0,
		0, 0, 0, 1
	);
}

// static
Matrix4f Matrix4f::uniformScaling( double s )
{
	return Matrix4f
	(
		s, 0, 0, 0,
		0, s, 0, 0,
		0, 0, s, 0,
		0, 0, 0, 1
	);
}

// static
Matrix4f Matrix4f::randomRotation( double u0, double u1, double u2 )
{
	return Matrix4f::rotation( Quat4f::randomRotation( u0, u1, u2 ) );
}

// static
Matrix4f Matrix4f::lookAt( const Vector3f& eye, const Vector3f& center, const Vector3f& up )
{
	// z is negative forward
	Vector3f z = ( eye - center ).normalized();
	Vector3f y = up;
	Vector3f x = Vector3f::cross( y, z );

	// the x, y, and z vectors define the orthonormal coordinate system
	// the affine part defines the overall translation
	Matrix4f view;

	view.setRow( 0, Vector4f( x, -Vector3f::dot( x, eye ) ) );
	view.setRow( 1, Vector4f( y, -Vector3f::dot( y, eye ) ) );
	view.setRow( 2, Vector4f( z, -Vector3f::dot( z, eye ) ) );
	view.setRow( 3, Vector4f( 0, 0, 0, 1 ) );

	return view;
}

// static
Matrix4f Matrix4f::orthographicProjection( double width, double height, double zNear, double zFar, bool directX )
{
	Matrix4f m;

	m( 0, 0 ) = 2.0f / width;
	m( 1, 1 ) = 2.0f / height;
	m( 3, 3 ) = 1.0f;

	m( 0, 3 ) = -1;
	m( 1, 3 ) = -1;

	if( directX )
	{
		m( 2, 2 ) = 1.0f / ( zNear - zFar );
		m( 2, 3 ) = zNear / ( zNear - zFar );
	}
	else
	{
		m( 2, 2 ) = 2.0f / ( zNear - zFar );
		m( 2, 3 ) = ( zNear + zFar ) / ( zNear - zFar );
	}

	return m;
}

// static
Matrix4f Matrix4f::orthographicProjection( double left, double right, double bottom, double top, double zNear, double zFar, bool directX )
{
	Matrix4f m;

	m( 0, 0 ) = 2.0f / ( right - left );
	m( 1, 1 ) = 2.0f / ( top - bottom );
	m( 3, 3 ) = 1.0f;

	m( 0, 3 ) = ( left + right ) / ( left - right );
	m( 1, 3 ) = ( top + bottom ) / ( bottom - top );

	if( directX )
	{
		m( 2, 2 ) = 1.0f / ( zNear - zFar );
		m( 2, 3 ) = zNear / ( zNear - zFar );
	}
	else
	{
		m( 2, 2 ) = 2.0f / ( zNear - zFar );
		m( 2, 3 ) = ( zNear + zFar ) / ( zNear - zFar );
	}

	return m;
}

// static
Matrix4f Matrix4f::perspectiveProjection( double fLeft, double fRight,
										 double fBottom, double fTop,
										 double fZNear, double fZFar,
										 bool directX )
{
	Matrix4f projection; // zero matrix

	projection( 0, 0 ) = ( 2.0f * fZNear ) / ( fRight - fLeft );
	projection( 1, 1 ) = ( 2.0f * fZNear ) / ( fTop - fBottom );
	projection( 0, 2 ) = ( fRight + fLeft ) / ( fRight - fLeft );
	projection( 1, 2 ) = ( fTop + fBottom ) / ( fTop - fBottom );
	projection( 3, 2 ) = -1;

	if( directX )
	{
		projection( 2, 2 ) = fZFar / ( fZNear - fZFar);
		projection( 2, 3 ) = ( fZNear * fZFar ) / ( fZNear - fZFar );
	}
	else
	{
		projection( 2, 2 ) = ( fZNear + fZFar ) / ( fZNear - fZFar );
		projection( 2, 3 ) = ( 2.0f * fZNear * fZFar ) / ( fZNear - fZFar );
	}

	return projection;
}

// static
Matrix4f Matrix4f::perspectiveProjection( double fovYRadians, double aspect, double zNear, double zFar, bool directX )
{
	Matrix4f m; // zero matrix

	double yScale = 1.f / tanf( 0.5f * fovYRadians );
	double xScale = yScale / aspect;

	m( 0, 0 ) = xScale;
	m( 1, 1 ) = yScale;
	m( 3, 2 ) = -1;

	if( directX )
	{
		m( 2, 2 ) = zFar / ( zNear - zFar );
		m( 2, 3 ) = zNear * zFar / ( zNear - zFar );
	}
	else
	{
		m( 2, 2 ) = ( zFar + zNear ) / ( zNear - zFar );
		m( 2, 3 ) = 2.f * zFar * zNear / ( zNear - zFar );
	}

	return m;
}

// static
Matrix4f Matrix4f::infinitePerspectiveProjection( double fLeft, double fRight,
												 double fBottom, double fTop,
												 double fZNear, bool directX )
{
	Matrix4f projection;

	projection( 0, 0 ) = ( 2.0f * fZNear ) / ( fRight - fLeft );
	projection( 1, 1 ) = ( 2.0f * fZNear ) / ( fTop - fBottom );
	projection( 0, 2 ) = ( fRight + fLeft ) / ( fRight - fLeft );
	projection( 1, 2 ) = ( fTop + fBottom ) / ( fTop - fBottom );
	projection( 3, 2 ) = -1;

	// infinite view frustum
	// just take the limit as far --> inf of the regular frustum
	if( directX )
	{
		projection( 2, 2 ) = -1.0f;
		projection( 2, 3 ) = -fZNear;		
	}
	else
	{
		projection( 2, 2 ) = -1.0f;
		projection( 2, 3 ) = -2.0f * fZNear;
	}

	return projection;
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

Vector4f operator * ( const Matrix4f& m, const Vector4f& v )
{
	Vector4f output( 0, 0, 0, 0 );

	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			output[ i ] += m( i, j ) * v[ j ];
		}
	}

	return output;
}

Matrix4f operator * ( const Matrix4f& x, const Matrix4f& y )
{
	Matrix4f product; // zeroes

	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			for( int k = 0; k < 4; ++k )
			{
				product( i, k ) += x( i, j ) * y( j, k );
			}
		}
	}

	return product;
}
