#include "Math/Transform.h"

namespace Kaguya
{

Bounds3f Transform::operator()(const Bounds3f &bbox) const
{
    const Transform &M = *this;
    Bounds3f ret(M(bbox.pMin));
    ret = Union(ret, M(Point3f(bbox.pMax.x, bbox.pMin.y, bbox.pMin.z)));
    ret = Union(ret, M(Point3f(bbox.pMin.x, bbox.pMax.y, bbox.pMin.z)));
    ret = Union(ret, M(Point3f(bbox.pMin.x, bbox.pMin.y, bbox.pMax.z)));
    ret = Union(ret, M(Point3f(bbox.pMin.x, bbox.pMax.y, bbox.pMax.z)));
    ret = Union(ret, M(Point3f(bbox.pMax.x, bbox.pMax.y, bbox.pMin.z)));
    ret = Union(ret, M(Point3f(bbox.pMax.x, bbox.pMin.y, bbox.pMax.z)));
    ret = Union(ret, M(Point3f(bbox.pMax.x, bbox.pMax.y, bbox.pMax.z)));
    return ret;
}

Ray Transform::operator()(const Ray &ray) const
{
    return m(ray);
}

void Transform::operator()(const Ray &ray, Ray* ret) const
{
    ret->d = m(ray.d);
    ret->o = m(ray.o);
    if (&ray != ret)
    {
        ret->tMin = ray.tMin;
        ret->tMax = ray.tMax;
        ret->time = ray.time;
        ret->dp = ray.dp;
    }
}

Point3f Transform::operator()(const Point3f &p) const
{
    return m(p);
}

Vector3f Transform::operator()(const Vector3f &v) const
{
    return m(v);
}

Normal3f Transform::operator()(const Normal3f &n) const
{
    Float x = n.x, y = n.y, z = n.z;
    return Normal3f(
        mInv.mtx[0][0] * x + mInv.mtx[0][1] * x + mInv.mtx[0][2] * x,
        mInv.mtx[1][0] * y + mInv.mtx[1][1] * y + mInv.mtx[1][2] * y,
        mInv.mtx[2][0] * z + mInv.mtx[2][1] * z + mInv.mtx[2][2] * z);
}

void Transform::setMat(const Matrix4x4 &mat)
{
    m = mat;
    mInv = m.inverse();
}

void Transform::setInvMat(const Matrix4x4 &matInv)
{
    mInv = matInv;
    m = mInv.inverse();
}

Transform xformTRS(Float tx, Float ty, Float tz,
                   Float rx, Float ry, Float rz,
                   Float sx, Float sy, Float sz)
{
    Matrix4x4 T = Matrix4x4::translate(tx, ty, tz);
    Matrix4x4 R = Matrix4x4::rotate(rx, ry, rz);
    Matrix4x4 S = Matrix4x4::scale(sx, sy, sz);
    return Transform(T * R * S);
}

Transform Rotate(const Vector3f &axis, Float theta)
{
    Vector3f u = normalize(axis);
    Float rad = degreeToRadian(theta);
    Float c = cos(rad);
    Float s = sin(rad);
    Float t = 1 - c;

    Float mat[4][4];

    mat[0][0] = t * sqr(u.x) + c;
    mat[0][1] = t * u.x * u.y - s * u.x;
    mat[0][2] = t * u.x * u.z + s * u.y;
    mat[0][3] = 0.0;

    mat[1][0] = t * u.x * u.y + s * u.z;
    mat[1][1] = t * sqr(u.y) + c;
    mat[1][2] = t * u.y * u.z - s * u.x;
    mat[1][3] = 0.0;

    mat[2][0] = t * u.x * u.z - s * u.y;
    mat[2][1] = t * u.y * u.z + s * u.x;
    mat[2][2] = t * sqr(u.z) + c;
    mat[2][3] = 0.0;

    mat[3][0] = 0.0;
    mat[3][1] = 0.0;
    mat[3][2] = 0.0;
    mat[3][3] = 1.0;

    return Transform(mat);
}

Transform lookAt(const Point3f &pos = Point3f(0, 0, 0),
                 const Point3f &target = Point3f(0, 0, 1),
                 const Vector3f &up = Vector3f(0, 1, 0))
{
    //Camera to World
    Vector3f nz = target - pos;
    // distance between target and camera position is too small
    if (isFuzzyNull(nz.x) && isFuzzyNull(nz.y) && isFuzzyNull(nz.z))
    {
        return Transform();
    }

    nz.normalize();
#ifdef RIGHT_HAND_ORDER // OpenGL style
    Vector3f nx = normalize(cross(up, nz));
    Vector3f ny = cross(nz, nx);
    Float mat[4][4] = { nx.x,  nx.y,  nx.z,  0.0,
                         ny.x,  ny.y,  ny.z,  0.0,
                        -nz.x, -nz.y, -nz.z,  0.0,
                         pos.x, pos.y, pos.z, 1.0 };
#else // DirectX style
    nz = -nz;
    Vector3f nx = normalize(cross(nz, up));//left dir
    Vector3f ny = cross(nx, nz);
    Float mat[4][4] = { nx.x, nx.y, nx.z, 0.0,
                        ny.x, ny.y, ny.z, 0.0,
                        nz.x, nz.y, nz.z, 0.0,
                        pos.x, pos.y, pos.z, 1.0 };
#endif

    return Transform(mat);
}

bool solveLinearSystem2x2(const Float A[2][2], const Float b[2],
                          Float* x0, Float* x1)
{
    // x = A^-1 * b
    // A^-1 = 1/det * | +A11  -A01 |
    //				  | -A10  +A00 |
    Float det = A[0][0] * A[1][1] - A[0][1] * A[1][0];
    if (std::abs(det) < 1e-10f) return false;
    *x0 = (A[1][1] * b[0] - A[0][1] * b[1]) / det;
    *x1 = (A[0][0] * b[1] - A[1][0] * b[0]) / det;

    if (std::isnan(*x0) || std::isnan(*x1)) return false;
    return true;
}

}
