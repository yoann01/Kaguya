//
//  Matrix3D.h
//
//  Created by Shenyao Ke on 1/21/15.
//  Copyright (c) 2015 AKIKA. All rights reserved.
//
#pragma once
#ifndef __Matrix3x3__
#define __Matrix3x3__

#include "Math/MathUtil.h"
#include "Math/Geometry.h"
#include "Core/MemoryControl.h"

class Matrix3x3
{
public:
	// Column Major
	Float mtx[3][3];
	//Float det = 0;

	Matrix3x3()
		: mtx{	1, 0, 0,
				0, 1, 0,
				0, 0, 1 }
	{}
	Matrix3x3(Float val) : mtx{ val }
	{}
	Matrix3x3(const Float mat[3][3])
	{
		memcpy(mtx, mat, sizeof(mtx));
	}
	Matrix3x3(const Matrix3x3 &mat)
	{
		memcpy(mtx, mat.mtx, sizeof(mtx));
	}
	Matrix3x3(Float t00, Float t01, Float t02,
		Float t10, Float t11, Float t12,
		Float t20, Float t21, Float t22)
	{
		mtx[0][0] = t00; mtx[0][1] = t01; mtx[0][2] = t02;
		mtx[1][0] = t10; mtx[1][1] = t11; mtx[1][2] = t12;
		mtx[2][0] = t20; mtx[2][1] = t21; mtx[2][2] = t22;
	}
	Matrix3x3(Point3f &col0, Point3f &col1, Point3f &col2)
	{
		mtx[0][0] = col0.x;	mtx[0][1] = col0.y;	mtx[0][2] = col0.z;
		mtx[1][0] = col1.x;	mtx[1][1] = col1.y;	mtx[1][2] = col1.z;
		mtx[2][0] = col2.x;	mtx[2][1] = col2.y;	mtx[2][2] = col2.z;
		//Determinant();
	}
	~Matrix3x3()
	{
		//delete_2DArray(mtx, 3, 3);
	}
	Float* operator [] (int i);
	const Float* operator [] (int i) const;
	Matrix3x3 operator + (const Matrix3x3 &mat) const;
	Matrix3x3 operator - (const Matrix3x3 &mat) const;
	Matrix3x3 operator * (const Matrix3x3 &mat) const;
	Point3f operator * (Point3f &p) const;
	Matrix3x3 operator = (const Matrix3x3 &mat) const;
	Matrix3x3 operator == (const Matrix3x3 &mat) const;
	Matrix3x3 operator != (const Matrix3x3 &mat) const;
	//Vector2D operator * (Vector2D& p) const;
	//Matrix3D operator / (const Matrix3D &) const;

	void setIdentity() { mtx[0][0] = mtx[1][1] = mtx[2][2] = 1.0; }
	void printMat();
	Float determinant() const;
	Matrix3x3 transposeMat() const;
	Matrix3x3 adjointMat() const;
	Matrix3x3 inverseMat() const;

	//Set transformation matrix
	void setTranslation(const Point2f& vec);
	void setRotation(Float theta);
	void setRotation(Float sinth, Float costh);
	void setScale(Float sx, Float sy);
	void setScale(Float scale);
	void setShear(const Point2f& vec);
	void setReflection(const Point2f& vec);
	void setPerspective(const Point2f& vPnt);
};
inline Float* Matrix3x3::operator[](int i)
{
	return mtx[i];
}
inline const Float* Matrix3x3::operator[](int i)const
{
	return mtx[i];
}
inline Matrix3x3 Matrix3x3::operator + (const Matrix3x3 &mat) const
{
	Matrix3x3 buffer;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			buffer.mtx[i][j] = mtx[i][j] + mat.mtx[i][j];
		}
	}

	return buffer;
}
inline Matrix3x3 Matrix3x3::operator - (const Matrix3x3 &mat) const
{
	Matrix3x3 buffer;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			buffer.mtx[i][j] = mtx[i][j] - mat.mtx[i][j];
		}
	}

	return buffer;
}
inline Matrix3x3 Matrix3x3::operator * (const Matrix3x3 &mat) const
{
	Matrix3x3 buffer(0.);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				buffer.mtx[i][j] += mtx[i][k] * mat.mtx[k][j];
			}
		}
	}
	//buffer.Determinant();
	return buffer;
}
inline Point3f Matrix3x3::operator * (Point3f &p) const
{
	return Point3f(p.x * mtx[0][0] + p.y * mtx[0][1] + p.z * mtx[0][2],
		p.x * mtx[1][0] + p.y * mtx[1][1] + p.z * mtx[1][2],
		p.x * mtx[2][0] + p.y * mtx[2][1] + p.z * mtx[2][2]);
}
/*
inline Vector2D Matrix3D::operator * (Vector2D& p) const
{
	return Vector2D(p.x * mtx[0][0] + p.y * mtx[0][1] + mtx[0][2],
		p.x * mtx[1][0] + p.y * mtx[1][1] + mtx[1][2]);
}*/
inline void Matrix3x3::printMat() {
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			cout << mtx[i][j] << "\t";
		}
		cout << endl;
	}
}
inline Float Matrix3x3::determinant() const
{
	Float det = 0;
	for (int i = 0; i < 3; i++)
	{
		det += mtx[0][i] * mtx[1][(i + 1) % 3] * mtx[2][(i + 2) % 3];
		det -= mtx[2][i] * mtx[1][(i + 1) % 3] * mtx[0][(i + 2) % 3];
	}
	return det;
}
inline Matrix3x3 Matrix3x3::transposeMat() const
{
	return Matrix3x3(
		mtx[0][0], mtx[1][0], mtx[2][0],
		mtx[0][1], mtx[1][1], mtx[2][1],
		mtx[0][2], mtx[1][2], mtx[2][2]);

}
inline Matrix3x3 Matrix3x3::adjointMat() const
{
	Matrix3x3 buffer, trpM;// trpM is transposed matrix of origin matrix.

	trpM = transposeMat();
	int coeff = -1;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			coeff *= -1;
			buffer.mtx[i][j] = coeff * (trpM.mtx[(i + 1) % 3][(j + 1) % 3] * trpM.mtx[(i + 2) % 3][(j + 2) % 3]
									- trpM.mtx[(i + 1) % 3][(j + 2) % 3] * trpM.mtx[(i + 2) % 3][(j + 1) % 3]);
		}
	}

	return buffer;
}
inline Matrix3x3 Matrix3x3::inverseMat() const
{
	Matrix3x3 buffer, adjM;
	Float det = this->determinant();
	if (det == 0)
	{
		cout << "The matrix is non-inversable!" << endl;
		return Matrix3x3();
	}
	adjM = adjointMat();

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			buffer.mtx[i][j] = adjM.mtx[i][j] / det;
		}
	}

	return buffer;
}
inline void Matrix3x3::setTranslation(const Point2f &vec)
{
	setIdentity();
	mtx[0][2] = vec.x;
	mtx[1][2] = vec.y;
}
inline void Matrix3x3::setRotation(Float theta)
{
	if (cos(theta) ==  1)
	{
		setIdentity();
		return;
	}
	setRotation(sin(DegToRad(theta)), cos(DegToRad(theta)));
}
inline void Matrix3x3::setRotation(Float sinth, Float costh)
{
	mtx[0][0] = costh;	mtx[0][1] = -sinth;
	mtx[1][0] = sinth;	mtx[1][1] = costh;
	mtx[2][2] = 1;
}
inline void Matrix3x3::setScale(Float sx, Float sy)
{
	mtx[0][0] = sx;	mtx[1][1] = sy; mtx[2][2] = 1;
}
inline void Matrix3x3::setScale(Float scale)
{
	mtx[0][0] = scale;	mtx[1][1] = scale; mtx[2][2] = 1;
}
inline void Matrix3x3::setShear(const Point2f& vec)
{
	setIdentity();
	mtx[0][1] = vec.x;
	mtx[1][0] = vec.y;
}
inline void Matrix3x3::setReflection(const Point2f& vec)
{
	// vec is a vector in the direction of the line
	mtx[0][0] = vec.x * vec.x - vec.y * vec.y;	mtx[0][1] = 2 * vec.x * vec.y;
	mtx[1][0] = 2 * vec.x * vec.y;	mtx[1][1] = vec.y * vec.y - vec.x * vec.x;
	mtx[2][2] = 1;
}
inline void Matrix3x3::setPerspective(const Point2f& vPnt)
{
	setIdentity();
	mtx[2][0] = 1.0 / vPnt.x; mtx[2][1] = 1.0 / vPnt.y;
}
#endif
