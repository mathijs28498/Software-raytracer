//#include <windows.h>
//#include <wingdi.h>
//#include <cstdlib> 
//#include <cstdio> 
//#include <cmath> 
//#include <vector> 
//#include <iostream> 
//#include <cassert> 
//
//#if not defined __linux__ && not defined __APPLE__ 
//#define M_PI 3.141592653589793 
//#define INFINITY 1e8 
//#endif 
//
//#define MAX_RAY_DEPTH 10
//
//class Vec3 {
//public:
//	float x, y, z;
//	Vec3() : x(0), y(0), z(0) {}
//	Vec3(float x) : x(x), y(x), z(x) {}
//	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
//	Vec3& normalize() {
//		float nor2 = length2();
//		if (nor2 > 0) {
//			float invNor = 1 / sqrt(nor2);
//			x *= invNor, y *= invNor, z *= invNor;
//		}
//		return *this;
//	}
//	Vec3 operator * (const float& f) const { return Vec3(x * f, y * f, z * f); }
//	Vec3 operator * (const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
//	float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
//	Vec3 operator - (const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
//	Vec3 operator + (const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
//	Vec3 operator / (const float& f) const { return Vec3(x / f, y / f, z / f); }
//	Vec3& operator += (const Vec3& v) { x += v.x, y += v.y, z += v.z; return *this; }
//	Vec3& operator *= (const Vec3& v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
//	Vec3 operator - () const { return Vec3(-x, -y, -z); }
//	float length2() const { return x * x + y * y + z * z; }
//	float length() const { return sqrt(length2()); }
//};
//
//class Sphere {
//public:
//	Vec3 center;                           /// position of the sphere 
//	float radius, radius2;                  /// sphere radius and radius^2 
//	Vec3 surfaceColor;						/// surface color and emission (light) 
//	float reflection;         /// surface transparency and reflectivity 
//	Sphere(
//		const Vec3& c,
//		const float& r,
//		const Vec3& sc,
//		const float& refl = 0) :
//		center(c), radius(r), radius2(r* r), surfaceColor(sc), reflection(refl) {}
//
//	bool intersect(const Vec3& rayOrigin, const Vec3& raydir, float& t) const {
//		//Vec3 l = center - rayorig;
//		Vec3 oc = center - rayOrigin;
//		float b = oc.dot(raydir);
//		if (b < 0) return false;
//
//		float c = oc.dot(oc) - b * b;
//		if (c > radius2) return false;
//
//		float thc = sqrt(radius2 - c);
//		float t0 = b - thc;
//		float t1 = b + thc;
//		t = t0 < t1 ? t0 : t1;
//		return true;
//	}
//};
//
//float mix(const float& a, const float& b, const float& mix) {
//	return b * mix + a * (1 - mix);
//}
//
//Vec3 trace(const Vec3& rayOrigin, const Vec3& rayDir, const std::vector<Sphere>& spheres, const Sphere& light, const int& depth) {
//	float minDist = INFINITY;
//	const Sphere* sphere = nullptr;
//
//
//	for (unsigned i = 0; i < spheres.size(); ++i) {
//		float dist = INFINITY;
//		if (spheres[i].intersect(rayOrigin, rayDir, dist) && dist < minDist) {
//			minDist = dist;
//			sphere = &spheres[i];
//		}
//	}
//
//	if (!sphere) return Vec3(1);
//	Vec3 surfaceColor = 0;
//	Vec3 intersec = rayOrigin + rayDir * minDist;
//	Vec3 normal = (intersec - sphere->center).normalize();
//	float bias = 1e-4;
//
//	if (sphere->reflection > 0 && depth < MAX_RAY_DEPTH) {
//		float facingratio = -rayDir.dot(normal);
//		float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.7);
//		Vec3 refldir = rayDir - normal * 2 * rayDir.dot(normal);
//		refldir.normalize();
//		Vec3 reflection = trace(intersec + normal * bias, refldir, spheres, light, depth + 1);
//		surfaceColor = (reflection * fresneleffect) * sphere->surfaceColor;
//	} else {
//
//		Vec3 lightDirection = light.center - normal;
//		lightDirection.normalize();
//		bool isShadow = false;
//		for (unsigned j = 0; j < spheres.size(); ++j) {
//			float t;
//			if (spheres[j].intersect(intersec + normal * bias, lightDirection, t)) {
//				isShadow = true;
//				break;
//			}
//		}
//		if (!isShadow)
//			surfaceColor = sphere->surfaceColor * max(float(0), normal.dot(lightDirection));
//	}
//
//	return sphere->surfaceColor;
//}
//
//void render(const std::vector<Sphere>& spheres, const Sphere& light, HDC* hdc) {
//	unsigned width = 640, height = 480;
//	float invWidth = 1 / float(width), invHeight = 1 / float(height);
//	float fov = 30, aspectratio = width / float(height);
//	float angle = tan(M_PI * 0.5 * fov / 180.);
//
//	int amountOfSamples = 20;
//	// Trace rays
//	for (unsigned y = 0; y < height; ++y) {
//		for (unsigned x = 0; x < width; ++x) {
//			Vec3 col(0);
//			for (unsigned s = 0; s < amountOfSamples; s++) {
//				float rx = (rand() % 10 + 0.5f) / 10;
//				float ry = (rand() % 10 + 0.5f) / 10;
//
//				float xx = (2 * ((x + rx + 0.5) * invWidth) - 1) * angle * aspectratio;
//				float yy = (1 - 2 * ((y + ry + 0.5) * invHeight)) * angle;
//				Vec3 raydir(xx, yy, -1);
//				raydir.normalize();
//				col += trace(Vec3(0), raydir, spheres, light, 0) / amountOfSamples;
//			}
//
//			SetPixel(*hdc, x, y, RGB(min(float(1), col.x) * 255, min(float(1), col.y) * 255, min(float(1), col.z) * 255));
//
//		}
//	}
//
//}
//
//int maAin(int argc, char** argv) {
//	//std::srand48(13);
//
//	HDC hdc = GetDC(GetConsoleWindow());
//	std::vector<Sphere> spheres{
//		Sphere(Vec3(0.0, -10004, -20), 10000, Vec3(0.20, 0.20, 0.20), 1),
//		Sphere(Vec3(0.0, 0, -20), 4, Vec3(1.00, 0.32, 0.36), 0.1f),
//		Sphere(Vec3(5.0, -1, -15), 2, Vec3(0.90, 0.76, 0.46), 1),
//		Sphere(Vec3(5.0, 0, -25), 3, Vec3(0.65, 0.77, 0.97), 1),
//		Sphere(Vec3(-5.5, 0, -15), 3, Vec3(0.90, 0.90, 0.90), 1)
//	};
//	// light
//	Sphere light(Vec3(0.0, 20, -30), 3, Vec3(0.00, 0.00, 0.00), 0);
//
//	while (true) {
//		render(spheres, light, &hdc);
//	}
//
//	return 0;
//}