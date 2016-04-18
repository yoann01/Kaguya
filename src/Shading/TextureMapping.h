//
//  TextureMapping.h
//
//  Created by Shenyao Ke on 3/6/15.
//  Copyright (c) 2015 AKIKA. All rights reserved.
//
#pragma once
#ifndef __TextureMapping__
#define __TextureMapping__
//#include "Core/rtdef.h"
#include "Math/MathUtil.h"
#include "Math/Geometry.h"

class TextureMapping
{
protected:
	Point3f c;
	Vector3f nx, ny, nz;
public:

	TextureMapping();
	TextureMapping(const Point3f &center);
	TextureMapping(const Point3f &center, const Vector3f &view, const Vector3f &upVec);
	TextureMapping(const Point3f &center, const Vector3f &xDir, const Vector3f &yDir, const Vector3f &zDir);
	virtual ~TextureMapping();

	virtual void printInfo() const;
	virtual void setPos(const Point3f &pos);
	virtual void setDir(const Vector3f &view, const Vector3f &upVec);
	virtual Point2f posToUV(const Point3f &pos) const;
	//virtual void mapToUV(const DifferentialGeometry* queryPoint) const = 0;
	//virtual void getUVDir(const DifferentialGeometry* queryPoint) const = 0;
	//virtual Point2f mapToXY(const Float& x, const Float& y) const;
};
/************************************************************************/
/* Spherical Mapping                                                    */
/************************************************************************/
class SphericalMapping2D :public TextureMapping
{
public:
	SphericalMapping2D();
	SphericalMapping2D(const Point3f &center);
	SphericalMapping2D(const Point3f &center, const Vector3f &view, const Vector3f &upVec);
	SphericalMapping2D(const Point3f &center, const Vector3f &xDir, const Vector3f &yDir, const Vector3f &zDir);
	~SphericalMapping2D();

	Point2f posToUV(const Point3f &pos) const;
	//void mapToUV(const DifferentialGeometry* queryPoint) const;
	//void getUVDir(const DifferentialGeometry* queryPoint) const;
protected:
private:
};
/************************************************************************/
/* Planar Mapping                                                       */
/************************************************************************/
class PlanarMapping2D :public TextureMapping
{
	Float uSize = 1, vSize = 1;
public:
	PlanarMapping2D();
	PlanarMapping2D(const Float& us, const Float& vs);
	PlanarMapping2D(const Point3f &center);
	PlanarMapping2D(const Point3f &center, const Float& us, const Float& vs);
	PlanarMapping2D(const Point3f &center, const Vector3f &view, const Vector3f &upVec);
	PlanarMapping2D(const Point3f &center, const Vector3f &xDir, const Vector3f &yDir, const Vector3f &zDir);
	~PlanarMapping2D();

	void setSize(const Float& us, const Float& vs);
	//void mapToUV(const DifferentialGeometry* queryPoint) const;
protected:

private:
};

#endif