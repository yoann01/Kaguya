#include "Sphere.h"
#include "Geometry/DifferentialGeometry.h"
#include "Shading/Texture.h"
#include "Shading/TextureMapping.h"

geoSphere::geoSphere(const Transform* o2w, const Transform* w2o,
	Float radius, Float phi, Float th0, Float th1)
	: Shape(o2w, w2o), r(radius), phiMax(phi)
	, thetaMin(th0), thetaMax(th1)
{
	zMin = clamp(cos(thetaMax), -radius, radius);
	zMax = clamp(cos(thetaMin), -radius, radius);

	bounding();
}
void geoSphere::bounding()
{
	ObjBound = Bounds3f(c);
	ObjBound.expand(r);
	//return false;
}

Bounds3f geoSphere::getWorldBounding() const
{
	Bounds3f ret(c);
	ret.expand(r);
	return ret;
}
bool geoSphere::intersect(const Ray& inRay,
	DifferentialGeometry* queryPoint, Float* tHit, Float* rayEpsilon) const
{
	Float phi, theta;
	Point3f pHit;
	Ray ray = (*WorldToObject)(inRay);
	// (ox + t*dx)^2 + (oy + t*dy)^2 + (oz + t*dz)^2 = r^2
	// A * t^2 + 2B * t + C = 0
	// A = dx^2 + dy^2 + dz^2
	// B = dx*ox + dy*oy + dz*oz
	// C = ox^2 + oy^2 + oz^2 - r^2
	// t = (-B - sqrt(B^2 - A*C)) / A
	//  or (-B + sqrt(B^2 - A*C)) / A
	Float coeA = ray.d.lengthSquared();
	Float coeB = 2 * (ray.d.x * ray.o.x + ray.d.y * ray.o.y + ray.d.z * ray.o.z);
	Float coeC = ray.o.x * ray.o.x + ray.o.y * ray.o.y + ray.o.z * ray.o.z - sqr(r);
	Float t1, t2;

	//delta > 0
	if (!quadratic(coeA, coeB, coeC, t1, t2)) return false;

	// (tmin, tmax) doesn't overlap (t1, t2)
	if (t1 > ray.tmax || t2 < ray.tmin) return false;

	// Test against ray length
	Float tHitLoc = t1;
	if (tHitLoc < ray.tmin)
	{
		tHitLoc = t2; // Always use the nearest intersection
		// (tmin, tmax) inside (t1, t2)
		if (tHitLoc > ray.tmax) return false;
	}

	// Compute intersection position
	pHit = ray(tHitLoc);
	if (pHit.x == 0. && pHit.y == 0) pHit.x = 1e-5f * r;
	phi = atan2(pHit.y, pHit.x);
	if (phi < 0.) phi += M_TWOPI;

	// Test against clipping range
	if (pHit.z < zMin || pHit.z > zMax || phi > phiMax)
	{
		// Closer intersection doesn't hit in clipping range

		// Farther intersection doesn't hit in clipping range
		// Fail to intersect
		if (tHitLoc == t1) return false;
		// Farther intersection hits but out of ray range
		if (t2 > ray.tmax) return false;

		// Re-Compute new intersection point
		tHitLoc = t2;
		pHit = ray(tHitLoc);
		if (pHit.x == 0. && pHit.y == 0) pHit.x = 1e-5f * r;
		phi = atan2(pHit.y, pHit.x);
		if (phi < 0.) phi += M_TWOPI;
		if (pHit.z < zMin || pHit.z > zMax || phi > phiMax)
		{
			return false;
		}
	}

	// Compute parametric representation
	theta = acos(clamp(pHit.z / r, (Float)-1, (Float)1));
	Point2f uv(phi / phiMax,
		(theta - thetaMin) / (thetaMax - thetaMin));

	// Compute dpdu and dpdv
	Float xyRadius = sqrt(pHit.x * pHit.x + pHit.y * pHit.y);
	Float invXYRadius = 1. / xyRadius;
	Float cosPhi = pHit.x * invXYRadius;
	Float sinPhi = pHit.y * invXYRadius;
	// r*sinTheta == xyRadius
	Vector3f dpdu(-phiMax * pHit.y, phiMax * pHit.x, 0.);
	Vector3f dpdv = (thetaMax - thetaMin)
		* Vector3f(pHit.z * cosPhi, pHit.z * sinPhi, -xyRadius);


	// Compute dndu, dndv

	*tHit = tHitLoc;
	*rayEpsilon = reCE * *tHit;

	const Transform &o2w = *ObjectToWorld;
	*queryPoint = DifferentialGeometry(o2w(pHit), o2w(Normal3f()), o2w(dpdu), o2w(dpdv), o2w(Normal3f()), o2w(Normal3f()), uv, this);
	return true;
}
bool geoSphere::isInside(const Point3f &p) const
{
	if ((p - c).lengthSquared() <= sqr(r))
	{
		return true;
	}
	else
	{
		return false;
	}
}