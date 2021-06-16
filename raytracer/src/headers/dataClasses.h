#pragma once

#include <cmath>

struct Vec3 {
	float x, y, z;
	Vec3(float value) : x(value), y(value), z(value) {}
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	float length2() { return x * x + y * y + z * z; };
	float length() { return sqrt(length2()); };
	Vec3 normalize() {
		float invL = 1 / length();
		x *= invL, y *= invL, z *= invL;
		return *this;
	};

	Vec3 operator- (Vec3 v) const { return Vec3(x - v.x, y - v.y, z - v.z); };
	Vec3 operator+ (Vec3 v) const { return Vec3(x + v.x, y + v.y, z + v.z); };
	Vec3 operator* (Vec3 v) const { return Vec3(x * v.x, y * v.y, z * v.z); };
	Vec3 operator/ (float f) const { return Vec3(x / f, y / f, z / f); };
};
float dot(Vec3& a, Vec3& b) {
	return (a.x * b.x + a.y * b.y + a.z * b.z);
};

struct Ray {
	Vec3 origin;
	Vec3 dir;

	Ray(Vec3 origin, Vec3 dir) : origin(origin), dir(dir) {}
};

struct Sphere {
	Vec3 center;
	float radius, radius2;
	Vec3 color;

	Sphere(Vec3 center, float radius, Vec3 color) :
		center(center), radius(radius), radius2(radius* radius), color(color) {}

	Vec3 getNormal(Vec3 intersec) {
		return ((intersec - center) / radius).normalize();
	}

	bool intersect(Ray& ray, float& dist) {
		Vec3 oc = center - ray.origin;
		float b = dot(oc, ray.dir);
		if (b < 0)
			return false;

		float c = dot(oc, oc) - b * b;
		if (c > radius2)
			return false;

		float thc = sqrt(radius2 - c);
		float t0 = b - thc;
		float t1 = b + thc;
		dist = t0 < t1 ? t0 : t1;
		return true;
	}
};

float mix(const float& a, const float& b, const float& mix) {
	return b * mix + a * (1 - mix);
}