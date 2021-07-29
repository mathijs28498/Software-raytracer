#include "headers/dataClasses.h"

#include <windows.h>
#include <wingdi.h>
#include <cstdlib> 
#include <cstdio> 
#include <cmath> 
#include <vector> 
#include <iostream> 
#include <cassert> 

#if not defined __linux__ && not defined __APPLE__ 
#define M_PI 3.141592653589793 
#define INFINITY 1e8 
#endif 

constexpr int MAX_DEPTH = 5;
constexpr int WIDTH = 500, HEIGHT = 500;
constexpr float INV_WIDTH = 1.0f / WIDTH, INV_HEIGHT = 1.0f / HEIGHT;
constexpr float ASPECT_RATIO = float(WIDTH) / HEIGHT;

Vec3 trace(Ray& ray, std::vector<Sphere>* spheres, const Sphere& light, const int& depth) {
	Sphere* hitSphere = nullptr;
	float minDist = INFINITY;

	for (size_t i = 0; i < spheres->size(); i++) {
		Sphere* sphere = &(*spheres)[i];
		float dist = INFINITY;
		if (sphere->intersect(ray, dist) && dist < minDist) {
			minDist = dist;
			hitSphere = sphere;
		}
	}

	if (hitSphere == nullptr)
		return Vec3(1);

	Vec3 intersec = ray.origin + ray.dir * minDist;
	Vec3 normal = hitSphere->getNormal(intersec).normalize();
	float bias = 0.0001f;
	intersec = intersec + normal * bias;


	Vec3 lightDir = (light.center - intersec).normalize();
	float strength = max(0.0f, dot(normal, lightDir));

	float shadowMult = 1;

	Ray shadowRay = Ray(intersec, lightDir);
	for (size_t i = 0; i < spheres->size(); i++) {
		Sphere* sphere = &(*spheres)[i];
		if (sphere == hitSphere) continue;

		float dist;
		if (sphere->intersect(shadowRay, dist)) {
			shadowMult = 0.2f;
			break;
		}
	}

	if (depth < MAX_DEPTH) {
		float facingratio = -dot(ray.dir, normal);
		float fresneleffect = mix(pow(1.0f - facingratio, 3.0f), 1.0f, 0.8f);

		Vec3 reflect = ray.dir - normal * 2 * dot(ray.dir, normal);
		Ray reflRay(intersec, reflect);
		Vec3 reflColor = trace(reflRay, spheres, light, depth + 1);
		return reflColor * fresneleffect * hitSphere->color * shadowMult;
	} else {
		return hitSphere->color * strength * shadowMult;
	}

}

void startRaytrace(Vec3& cameraPos, const HDC& hdc, std::vector<Sphere>* spheres, const Sphere& light) {
	float fov = 45;
	float angle = tan(M_PI * 0.5 * fov / 180.);
	int amountOfSamples = 5;

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			Vec3 col(0);
			for (int i = 0; i < amountOfSamples; i++) {
				float rx = (rand() % 10 + 0.5f) / 10;
				float ry = (rand() % 10 + 0.5f) / 10;


				float xx = (2 * ((x + rx + 0.5) * INV_WIDTH) - 1) * angle * ASPECT_RATIO;
				float yy = (1 - 2 * ((y + ry + 0.5) * INV_HEIGHT)) * angle;

				Ray primaryRay(cameraPos, Vec3(xx, yy, -1).normalize());

				col = col + trace(primaryRay, spheres, light, 0) / amountOfSamples;
			}

			SetPixel(hdc, x, y, RGB(min(float(1), col.x) * 255, min(float(1), col.y) * 255, min(float(1), col.z) * 255));
		}
	}

}

int main() {
	HDC hdc = GetDC(GetConsoleWindow());
	std::vector<Sphere> spheres{
		Sphere(Vec3(0.0, -10004, -20), 10000, Vec3(0.20, 0.50, 0.80)),
		Sphere(Vec3(0.0, 0, -20), 4, Vec3(1.00, 0.32, 0.36)),
		Sphere(Vec3(5.0, -1, -15), 2, Vec3(0.90, 0.76, 0.46)),
		Sphere(Vec3(5.0, 0, -25), 3, Vec3(0.65, 0.77, 0.97)),
		Sphere(Vec3(-5.5, 0, -15), 3, Vec3(0.90, 0.90, 0.90))
	};
	// light
	Sphere light(Vec3(20, 5, -10), 3, Vec3(0.00, 0.00, 0.00));
	Vec3 cameraPos(0, 0, 0);
	while (true) {
		startRaytrace(cameraPos, hdc, &spheres, light);
		//cameraPos.x += 1;
		spheres[1].center.x--;
	}
}