#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <limits>
#include <chrono>
#include <windows.h>
#include <wingdi.h>


# define PI           3.14159265358979323846  /* pi */

#define MAX_RAY_DEPTH 0

constexpr int WIDTH = 500;
constexpr int HEIGHT = 500;

float invWidth = 1 / float(WIDTH), invHeight = 1 / float(HEIGHT);
float fov = 90, aspectratio = WIDTH / float(HEIGHT);
float angle = fov * PI / 180;
float imagePlaneWidth = 100;
float imagePlaneHeight = aspectratio * imagePlaneWidth;

struct Vec3 {
	float x, y, z;
	Vec3() : x(0), y(0), z(0) {};
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {};
	float length() { return sqrt(x * x + y * y + z * z); };
	float sqlength() { return x * x + y * y + z * z; };
	Vec3 normalize() {
		float l = length();
		return Vec3(x / l, y / l, z / l);
	};

	Vec3 rotate(float angle) {
		return Vec3();
	}

	Vec3 operator- (Vec3 v) { return Vec3(x - v.x, y - v.y, z - v.z); }
	Vec3 operator+ (Vec3 v) { return Vec3(x + v.x, y + v.y, z + v.z); }
	Vec3 operator* (float a) { return Vec3(x * a, y * a, z * a); }
	Vec3 operator* (Vec3 v) { return Vec3(x * v.x, y * v.y, z * v.z); }
	Vec3 operator/ (float a) { return Vec3(x / a, y / a, z / a); }
	Vec3 operator- () { return Vec3{ -x, -y, -z }; }
	bool operator== (Vec3 v) { return abs(x - v.x) < 0.001f && abs(y - v.y) < 0.001f && abs(z - v.z) < 0.001f; }
};

float dot(Vec3 a, Vec3 b) {
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

struct Color {
	Vec3 rgb;
	Color() : rgb({ 0, 0, 0 }) {};
	Color(Vec3 rgb) : rgb(rgb) {};

	Color operator* (float a) { return Color(rgb * a); };
	Color operator* (Color c) { return Color(rgb * c.rgb); };
	Color operator+ (Color c) { return Color((rgb + c.rgb) / 2); };
	bool operator== (Color c) { return rgb == c.rgb; };
};

struct Ray {
	Vec3 origin;
	Vec3 dir;

	Ray(Vec3 origin, Vec3 dir) : origin(origin), dir(dir) {};
};

class Sphere {
public:
	Vec3 center;
	float radius;
	Color color;
	Sphere(Vec3 center, float radius, Color color) : center(center), radius(radius), color(color) {};

	Vec3 getNormal(Vec3 intersec) {
		return ((intersec - center) / radius).normalize();
	}

	bool intersect(Ray ray, float& t) {
		Vec3 oc = ray.origin - center;
		float b = 2 * dot(oc, ray.dir);
		float c = dot(oc, oc) - radius * radius;
		float disc = b * b - c * 4;
		if (disc < 0) return false;

		disc = sqrt(disc);
		float t0 = -b - disc;
		float t1 = -b + disc;
		t = t0 < t1 ? t0 : t1;
		return t > 0;
	}
};

Color traceMine(std::vector<Sphere>* spheres, Sphere* light, Ray& ray, int depth) {
	float minDist = 2000000;
	Color endColor;

	Sphere* hitSphere = nullptr;
	Vec3 intersec;
	Vec3 lightDir;
	Vec3 normal;
	size_t sphereIndex;

	for (size_t i = 0; i < spheres->size(); i++) {
		Sphere* sphere = &(*spheres)[i];
		float dist;
		if (sphere->intersect(ray, dist) && dist < minDist) {
			minDist = dist;

			intersec = ray.origin + ray.dir * dist;
			lightDir = (light->center - intersec).normalize();
			normal = sphere->getNormal(intersec);

			hitSphere = sphere;
			sphereIndex = i;
		}
	}

	if (hitSphere == nullptr)
		return Color();



	Ray shadowRay{ intersec, lightDir };

	for (size_t i = 0; i < spheres->size(); i++) {
		if (i == sphereIndex) continue;
		Sphere sphere = (*spheres)[i];
		float tempDist;
		if (sphere.intersect(shadowRay, tempDist)) {
			return Color();
		}
	}



	float strength = dot(shadowRay.dir, normal.normalize());

	endColor = hitSphere->color * 0.7f + light->color * strength * 1.1f;

	if (depth < MAX_RAY_DEPTH) {
		// compute reflection
		Vec3 reflectionDir = ray.dir - normal * 2 * dot(ray.dir, normal);
		Ray reflectionRay{ intersec, -reflectionDir.normalize() };
		// recurse
		Color reflectionColor = traceMine(spheres, light, reflectionRay, depth + 1);
		endColor = endColor + reflectionColor;
	}

	if (endColor.rgb.x < 0) endColor.rgb.x = 0;
	if (endColor.rgb.y < 0) endColor.rgb.y = 0;
	if (endColor.rgb.z < 0) endColor.rgb.z = 0;
	if (endColor.rgb.x > 255) endColor.rgb.x = 255;
	if (endColor.rgb.y > 255) endColor.rgb.y = 255;
	if (endColor.rgb.z > 255) endColor.rgb.z = 255;

	return endColor;

}


void rayTraceScratch(std::vector<Sphere>* spheres, Sphere* light, HDC* hdc) {
	std::vector<Color> pixels(HEIGHT * WIDTH);

	float imagePlaneRatio = WIDTH / imagePlaneWidth;
	Vec3 cameraPosition{ 0, 0, 10 };
	bool firstLoop = true;
	while (true) {
		for (int y = 0; y < HEIGHT; y++) {
			for (int x = 0; x < WIDTH; x++) {
				float xc = x - WIDTH / 2;
				float yc = y - HEIGHT / 2;

				Vec3 imagePlanePoint{ xc / imagePlaneRatio, yc / imagePlaneRatio, -10 };

				Vec3 direction = (imagePlanePoint - cameraPosition).normalize();
				Ray primaryRay{ cameraPosition, direction };

				Color endColor = traceMine(spheres, light, primaryRay, 0);

				if (!(endColor == pixels[y * WIDTH + x]) || firstLoop) {
					SetPixel(*hdc, x, y, RGB(endColor.rgb.x, endColor.rgb.y, endColor.rgb.z));
					pixels[y * WIDTH + x] = endColor;
				}
			}
		}
		firstLoop = false;
		(*spheres)[0].center = (*spheres)[0].center + Vec3(1, 0, 0);
	}
}


int maain() {

	HDC hdc = GetDC(GetConsoleWindow());

	std::vector<Sphere> spheres{
		/*{ {0, 0, -25}, 10, {{255, 0, 255}}  },
		{ {10, 20, -20}, 20, {{0, 255, 0}} },
		{ {-10, -20, -40}, 25, {{255, 255, 0}} },
		{ {0, -80, -10}, 400, {{0, 0, 255}} },*/
		{{0.0, -10004, -20}, 10000, {{ 0.20, 0.20, 0.20 }}},
		{{ 0.0, 0, -20 }, 4, {{ 1.00, 0.32, 0.36 }}},
		{{ 5.0, -1, -15 }, 2, {{ 0.90, 0.76, 0.46 }}},
		{{ 5.0, 0, -25 }, 3, {{ 0.65, 0.77, 0.97 }}},
		{{ -5.5, 0, -15 }, 3, {{ 0.90, 0.90, 0.90 }}},
	};
		// light
		//spheres.push_back(Sphere(Vec3f(0.0, 20, -30), 3, Vec3f(0.00, 0.00, 0.00), 0, 0.0, Vec3f(3)));

	//Sphere light{ {10, 0, 10}, 5, {{255, 255, 255}} };

	Sphere light{ { 0.0, 20, 0}, 3, {{0.00, 0.00, 0.00}} };

	//rayTraceScratch(&spheres, &light, &hdc);
	return 0;
}