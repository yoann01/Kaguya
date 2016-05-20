#include "Camera/Camera.h"

Camera::Camera(const Point3f &eye,
	const Point3f &targ, const Vector3f &up,
	Float asp, Float lr, Float fd, const Film &fm)
	: CameraToWorld(Matrix4x4::LookAt(eye, target, up))
	, target(targ)
	, viewportRatio(1), focLen(35)
	, lensRadius(lr), focalDistance(fd)
	, nearPlane(0.1), farPlane(100)
	, film(fm)
{
}

void Camera::setFilm(const Film &f)
{
	film = f;
	updateMatrices();
}

void Camera::setResolution(int resX, int resY)
{
	//film.resize(resX, resY);
	//buffer = renderBuffer(resX, resY);
}
void Camera::setSample(int aaSample)
{
	sample = aaSample;
}
void Camera::setFocLen(Float fl)
{
	focLen = fl;
}
void Camera::setFilmType(FILM_TYPE filmType)
{
	film.setFilmType(filmType);
}
void Camera::updateMatrices()
{
	// Raster to camera
	updateRasterToCam();

	// Camera to screen
	updateCamToScreen();
	
	// Update RasterToScreen
	updateRasterToScreen();

}

/*
void Camera::setBuffer(int x, int y, const bufferData tmpBuff)
{
	buffer.data[x][y] = tmpBuff;
}
bufferData Camera::getBufferData(int x, int y) const
{
	return buffer.data[x][y];
}*/

Point3f Camera::getTarget() const
{
	return target;
}

void Camera::setCamToWorld(const Matrix4x4 &cam2wMat)
{
	CameraToWorld = Transform(cam2wMat);
}

void Camera::setProjection(const Matrix4x4 &perspMat)
{
	CameraToScreen = Transform(perspMat);
}

void Camera::exportVBO(float *view, float *proj, float *raster) const
{
	if (view != nullptr)
	{
		CameraToWorld.mInv.exportVBO(view);
	}
	if (proj != nullptr)
	{
		CameraToScreen.m.exportVBO(proj);
	}
	if (raster != nullptr)
	{
		RasterToScreen.m.exportVBO(raster);
	}
}

void Camera::updateRasterToCam()
{
	Matrix4x4 raster2camMat = film.rasterToFilm();
#ifdef RIGHT_HAND_ORDER
	raster2camMat[3][2] = -focLen;
#else
	raster2camMat[3][2] = focLen;
#endif
	RasterToCamera = Transform(raster2camMat);
}

void Camera::updateRasterToScreen()
{
	RasterToScreen.setMat(CameraToScreen.getMat() * RasterToCamera.getMat());
}

void Camera::zoom(Float x_val, Float y_val, Float z_val)
{
	Matrix4x4 cam2w = CameraToWorld.getMat();
	
	// Pw: world space position, Pc: Camera space position
	// Pw = c2w1 * Pc1 = c2w1 * T^-1 * Pc = c2w * Pc
	// c2w1 = T * c2w
	Matrix4x4 newLookAt = cam2w * Matrix4x4::Translate(x_val, y_val, z_val);
	//cout << "befor: " << target << endl;

	Vector3f _nx(cam2w[0]), _ny(cam2w[1]);
	target += _nx * x_val + _ny * y_val;
	//cout << "after: " << target << endl;
	CameraToWorld.setMat(newLookAt);
}

void Camera::rotate(Float x_rot, Float y_rot, Float z_rot)
{
	//pitch, yaw, roll
	Matrix4x4 lookAtMat = CameraToWorld.getMat();

	Point3f _pos(lookAtMat[3]);
	Vector3f vt = _pos - target;
	Float vt_len = vt.length();
	Float upCoef = lookAtMat[1][1] < 0 ? -1 : 1;
	
	Float phi = atan2(vt.x, vt.z) + DegToRad(y_rot) * upCoef;
	Float old_theta = asin(vt.y / vt_len);
	Float theta = old_theta + DegToRad(x_rot) * upCoef;
	
	if ((old_theta < M_HALFPI && theta > M_HALFPI) || (old_theta > -M_HALFPI && theta < -M_HALFPI))
	{
		upCoef *= -1;
	}
	Vector3f newVt(sin(phi) * cos(theta), sin(theta), cos(phi) * cos(theta));
	CameraToWorld.setMat(Matrix4x4::LookAt(target + newVt * vt_len, target, Vector3f(0, upCoef, 0)));
}

void Camera::rotatePYR(Float pitchAngle, Float yawAngle, Float rollAngle)
{

	/*Matrix4x4 lookAtMat = CameraToWorld.getMat();
	Vector4D _nx(lookAtMat[0]), _ny(lookAtMat[1]), _nz(lookAtMat[2]);
	Vector4D _pos(lookAtMat[3]);
	Vector4D vt(_pos - Vector4D(target, 1));
	Float vt_len = vt.getLength();

	printf("Original VT: %lf\n", vt.getLength());
	// Rotate pitch
	Matrix4x4 pitchMat = Matrix4x4::Rotate(_nx.toVector3D(), pitchAngle);//RotateX(pitchAngle);
	_ny = pitchMat * _ny;
	_nz = pitchMat * _nz;
	vt = pitchMat * vt;

	printf("\tafter pitch: %lf\n", vt.getLength());
	Matrix4x4 yawMat = Matrix4x4::Rotate(_ny.toVector3D(), yawAngle);//RotateY(yawAngle);
	_nx = yawMat * _nx;
	_nz = yawMat * _nz;
	vt = yawMat * vt;

	printf("\tafter yaw: %lf\n", vt.getLength());
	_nx.normalize();
	_ny.normalize();
	_nz.normalize();
	vt.normalize();
	_pos = Vector4D(target, 1.0) + vt * vt_len;

	CameraToWorld.setMat(Matrix4x4::LookAt(_pos.toVector3D(), target, _ny.toVector3D()));*/
}

void Camera::resizeViewport(Float aspr)
{
	Matrix4x4 newProj= CameraToScreen.getMat();
	newProj[0][0] = -newProj[1][1] / aspr;
	CameraToScreen.setMat(newProj);
}