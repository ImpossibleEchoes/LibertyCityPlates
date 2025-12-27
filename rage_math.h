#pragma once
#include <cmath>

#define RAGE_PI 3.1415926535f

struct Rect {
	float minX;
	float maxX;
	float minY;
	float maxY;

	//Rect(float _minX, float _minX, float _minX, float _minX)
};

struct Vector3 {
	float x, y, z, w;
	
	void normalize();
};

struct Vector4 {
	float x, y, z, w;
};

struct Vec3 {
	float x, y, z;
};

struct Quaternion {
	float x, y, z, w;

	void normalize();

	Quaternion operator*(const Quaternion& other) const;

	void fromAxisAngle(const Vector3& axis, float angle_rad);
};

struct Matrix34 {
	Vector3 a, b, c, d;

	void identity();

	void identityRot();

	void fromQuaternion(const Quaternion& quat);

	void setYRot(float rot);

	void multBy(Matrix34* pMtx);


	__forceinline float getZAngle() const { return atan2f(-a.y, a.x); }

	Matrix34* mult(Matrix34* other) {
		Matrix34 temp;

		temp.a.x = (a.x * other->a.x) + (a.y * other->b.x) + (a.z * other->c.x);
		temp.a.y = (a.x * other->a.y) + (a.y * other->b.y) + (a.z * other->c.y);
		temp.a.z = (a.x * other->a.z) + (a.y * other->b.z) + (a.z * other->c.z);

		temp.b.x = (b.x * other->a.x) + (b.y * other->b.x) + (b.z * other->c.x);
		temp.b.y = (b.x * other->a.y) + (b.y * other->b.y) + (b.z * other->c.y);
		temp.b.z = (b.x * other->a.z) + (b.y * other->b.z) + (b.z * other->c.z);

		temp.c.x = (c.x * other->a.x) + (c.y * other->b.x) + (c.z * other->c.x);
		temp.c.y = (c.x * other->a.y) + (c.y * other->b.y) + (c.z * other->c.y);
		temp.c.z = (c.x * other->a.z) + (c.y * other->b.z) + (c.z * other->c.z);

		temp.d.x = (d.x * other->a.x) + (d.y * other->b.x) + (d.z * other->c.x) + other->d.x;
		temp.d.y = (d.x * other->a.y) + (d.y * other->b.y) + (d.z * other->c.y) + other->d.y;
		temp.d.z = (d.x * other->a.z) + (d.y * other->b.z) + (d.z * other->c.z) + other->d.z;

		*this = temp;

		return this;
	}
};

float normalizeAngleSafeWithClamp25(float angle);

float sign(float a1);