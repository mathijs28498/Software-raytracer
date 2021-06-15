#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <limits>

# define PI           3.14159265358979323846  /* pi */

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
		return (intersec - center) / radius;
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
		return true;
	}
};

int main() {
	constexpr int WIDTH = 500;
	constexpr int HEIGHT = 500;

	std::ofstream out("output/trace.ppm");
	out << "P3\n" << WIDTH << "\n" << HEIGHT << "\n255\n";

	std::vector<Color> pixelColors;
	pixelColors.resize(HEIGHT * WIDTH);

	std::vector<Sphere> spheres{
		{ {0, 0, -10}, 5, {{255, 0, 255}}  },
		{ {10, 20, -10}, 7, {{0, 255, 0}} },
		{ {-10, -20, -30}, 25, {{255, 255, 0}} },
	};

	Sphere light{ {15, 0, 0}, 5, {{255, 255, 255}} };

	//float fov = PI / 4, aspectRatio = (WIDTH + 0.0f) / HEIGHT;

	float invWidth = 1 / float(WIDTH), invHeight = 1 / float(HEIGHT);
	float fov = 90, aspectratio = WIDTH / float(HEIGHT);
	float angle = fov * PI / 180;
	float imagePlaneWidth = 100;
	float imagePlaneHeight = aspectratio * imagePlaneWidth;

	float imagePlaneRatio = WIDTH / imagePlaneWidth;

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			float xc = x - WIDTH / 2;
			float yc = y - HEIGHT / 2; 

			Vec3 imagePlanePoint{ xc / imagePlaneRatio, yc / imagePlaneRatio, -10 };
			Vec3 cameraPosition{0, 0, 0};

			Vec3 direction = (imagePlanePoint - cameraPosition).normalize();
			Ray primaryRay{ cameraPosition, direction };

			float minDist = std::numeric_limits<float>::max();
			for (Sphere sphere : spheres) {
				float dist;
				if (sphere.intersect(primaryRay, dist) && dist < minDist) {
					minDist = dist;

					Vec3 intersec = primaryRay.origin + primaryRay.dir * dist;


					Vec3 lightDir = light.center - intersec;
					Vec3 normal = sphere.getNormal(intersec);
					float strength = dot(lightDir.normalize(), normal.normalize());

					Color endColor = sphere.color * 0.7f + light.color * strength * 1.1f;

					if (endColor.rgb.x < 0) endColor.rgb.x = 0;
					if (endColor.rgb.y < 0) endColor.rgb.y = 0;
					if (endColor.rgb.z < 0) endColor.rgb.z = 0;
					if (endColor.rgb.x > 255) endColor.rgb.x = 255;
					if (endColor.rgb.y > 255) endColor.rgb.y = 255;
					if (endColor.rgb.z > 255) endColor.rgb.z = 255;

					pixelColors[y * WIDTH + x] = endColor;
				}
			}
			out << static_cast<int>(pixelColors[y * WIDTH + x].rgb.x) << '\n';
			out << static_cast<int>(pixelColors[y * WIDTH + x].rgb.y) << '\n';
			out << static_cast<int>(pixelColors[y * WIDTH + x].rgb.z) << '\n';
		}
	}
	out.close();

	return 0;
}