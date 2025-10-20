#include "rage_math.h"

void Vector3::normalize() {
	float y; // xmm2_4
	float x; // xmm3_4
	float z; // xmm1_4
	float v6; // xmm0_4
	float v7; // [esp+8h] [ebp-10h]
	float v8; // [esp+8h] [ebp-10h]

	y = this->y;
	x = this->x;
	z = this->z;
	v7 = (float)((float)(x * x) + (float)(y * y)) + (float)(z * z);
	if (v7 == 0.0)
		v6 = 0.0;
	else {
		y = this->y;
		x = this->x;
		z = this->z;
		v8 = 1.0 / sqrtf(v7);
		v6 = v8;
	}
	this->x = x * v6;
	this->y = y * v6;
	this->z = z * v6;
}

void Quaternion::normalize() {
	float magnitude = sqrtf(x * x + y * y + z * z + w * w);
	if (magnitude > 0.0f) {
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
		w /= magnitude;
	}
}

Quaternion Quaternion::operator*(const Quaternion& other) const {
	Quaternion result;
	result.w = w * other.w - x * other.x - y * other.y - z * other.z;
	result.x = w * other.x + x * other.w + y * other.z - z * other.y;
	result.y = w * other.y - x * other.z + y * other.w + z * other.x;
	result.z = w * other.z + x * other.y - y * other.x + z * other.w;
	result.normalize();
	return result;
}

void Quaternion::fromAxisAngle(const Vector3& axis, float angle_rad) {
	Quaternion q;
	float halfAngle = angle_rad * 0.5f;
	float sinHalfAngle = sinf(halfAngle);
	x = axis.x * sinHalfAngle;
	y = axis.y * sinHalfAngle;
	z = axis.z * sinHalfAngle;
	w = cosf(halfAngle);
	normalize(); // Важно нормализовать кватернион
}

void Matrix34::identity() {
	a.y = a.z = b.x = b.z = c.x = c.y = d.x = d.y = d.z = 0.f;
	a.x = b.y = c.z = 1.f;
}

void Matrix34::identityRot() {
	a.y = a.z = b.x = b.z = c.x = c.y = 0.f;
	a.x = b.y = c.z = 1.f;
}

void Matrix34::fromQuaternion(const Quaternion& quat) {
	float x2 = quat.x * quat.x;
	float y2 = quat.y * quat.y;
	float z2 = quat.z * quat.z;
	float w2 = quat.w * quat.w;

	float xy = quat.x * quat.y;
	float xz = quat.x * quat.z;
	float xw = quat.x * quat.w;
	float yz = quat.y * quat.z;
	float yw = quat.y * quat.w;
	float zw = quat.z * quat.w;

	a.x = 1.0f - 2.0f * (y2 + z2);
	a.y = 2.0f * (xy + zw);
	a.z = 2.0f * (xz - yw);
	a.w = 0.0f;

	b.x = 2.0f * (xy - zw);
	b.y = 1.0f - 2.0f * (x2 + z2);
	b.z = 2.0f * (yz + xw);
	b.w = 0.0f;

	c.x = 2.0f * (xz + yw);
	c.y = 2.0f * (yz - xw);
	c.z = 1.0f - 2.0f * (x2 + y2);
	c.w = 0.0f;

}

void Matrix34::setYRot(float rot) {
	float cosAngle = cosf(rot);
	float sinAngle = sinf(rot);

	a.x = cosAngle;
	a.z = sinAngle;

	c.x = -sinAngle;
	c.z = cosAngle;
}

void Matrix34::multBy(Matrix34* pMtx) {
	//a.x = a.x * pMtx->a.x + a.y * pMtx->b.x + a.z * pMtx->c.x + a.w * pMtx->d.x;
	//a.y = a.x * pMtx->a.y + a.y * pMtx->b.y + a.z * pMtx->c.y + a.w * pMtx->d.y;
	//a.z = a.x * pMtx->a.z + a.y * pMtx->b.z + a.z * pMtx->c.z + a.w * pMtx->d.z;
	//a.w = a.x * pMtx->a.w + a.y * pMtx->b.w + a.z * pMtx->c.w + a.w * pMtx->d.w;

	//b.x = b.x * pMtx->a.x + b.y * pMtx->b.x + b.z * pMtx->c.x + b.w * pMtx->d.x;
	//b.y = b.x * pMtx->a.y + b.y * pMtx->b.y + b.z * pMtx->c.y + b.w * pMtx->d.y;
	//b.z = b.x * pMtx->a.z + b.y * pMtx->b.z + b.z * pMtx->c.z + b.w * pMtx->d.z;
	//b.w = b.x * pMtx->a.w + b.y * pMtx->b.w + b.z * pMtx->c.w + b.w * pMtx->d.w;

	//c.x = c.x * pMtx->a.x + c.y * pMtx->b.x + c.z * pMtx->c.x + c.w * pMtx->d.x;
	//c.y = c.x * pMtx->a.y + c.y * pMtx->b.y + c.z * pMtx->c.y + c.w * pMtx->d.y;
	//c.z = c.x * pMtx->a.z + c.y * pMtx->b.z + c.z * pMtx->c.z + c.w * pMtx->d.z;
	//c.w = c.x * pMtx->a.w + c.y * pMtx->b.w + c.z * pMtx->c.w + c.w * pMtx->d.w;

	//a.x = ((a.x * pMtx->a.x) + (a.y * pMtx->b.x)) + (a.z * pMtx->c.x);
	//a.y = ((a.y * pMtx->b.y) + (pMtx->a.y * a.x)) + (pMtx->c.y * a.z);
	//a.z = ((a.y * pMtx->b.z) + (pMtx->a.z * a.x)) + (pMtx->c.z * a.z);
	//b.x = ((b.z * pMtx->c.x) + (b.y * pMtx->b.x)) + (b.x * pMtx->a.x);
	//b.y = ((b.z * pMtx->c.y) + (b.y * pMtx->b.y)) + (b.x * pMtx->a.y);
	//b.z = ((b.z * pMtx->c.z) + (b.y * pMtx->b.z)) + (b.x * pMtx->a.z);
	//c.x = ((c.x * pMtx->a.x) + (c.z * pMtx->c.x)) + (c.y * pMtx->b.x);
	//c.y = ((pMtx->c.y * c.z) + (pMtx->b.y * c.y)) + (pMtx->a.y * c.x);
	//c.z = ((pMtx->c.z * c.z) + (pMtx->b.z * c.y)) + (pMtx->a.z * c.x);

	a.x = a.x * pMtx->a.x + b.x * pMtx->a.y + c.x * pMtx->a.z;
	b.x = a.x * pMtx->b.x + b.x * pMtx->b.y + c.x * pMtx->b.z;
	c.x = a.x * pMtx->c.x + b.x * pMtx->c.y + c.x * pMtx->c.z;

	a.y = a.y * pMtx->a.x + b.y * pMtx->a.y + c.y * pMtx->a.z;
	b.y = a.y * pMtx->b.x + b.y * pMtx->b.y + c.y * pMtx->b.z;
	c.y = a.y * pMtx->c.x + b.y * pMtx->c.y + c.y * pMtx->c.z;

	a.z = a.z * pMtx->a.x + b.z * pMtx->a.y + c.z * pMtx->a.z;
	b.z = a.z * pMtx->b.x + b.z * pMtx->b.y + c.z * pMtx->b.z;
	c.z = a.z * pMtx->c.x + b.z * pMtx->c.y + c.z * pMtx->c.z;

}

float normalizeAngleSafeWithClamp25(float angle) {

	if (angle > 25.0)
		angle = 25.0;
	else if (angle < -25.0)
		angle = -25.0;

	angle = fmodf(angle + RAGE_PI, RAGE_PI * 2);

	if (angle <= 0.0)
		angle += RAGE_PI;
	else
		angle -= RAGE_PI;

	if (angle == -RAGE_PI)
		angle = RAGE_PI;

	return angle;
}

float sign(float a1) {
	if (a1 < 0.f)
		return -1.f;
	if (a1 != 0.f)
		return 1.f;
	return 0.f;
}
