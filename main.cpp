#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <algorithm>
#include <utility>
#include <cmath>
#include "geometry.h"
#include "sphere.h"
#include "light.h"

std::pair<float, float> intersect_ray(Vec3f &origin, Vec3f &direction, Sphere &sphere) {
	float inf = std::numeric_limits<float>::infinity();

	float radius = sphere.radius;
	Vec3f co = origin - sphere.center;

	float a = direction.dot(direction);
	float b = 2 * co.dot(direction);
	float c = co.dot(co) - radius * radius;

	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0) {
		return std::make_pair(inf, inf);
	}

	float t1 = ( -b + std::sqrt(discriminant) ) / ( 2 * a );
	float t2 = ( -b - std::sqrt(discriminant) ) / ( 2 * a );
	return std::make_pair(t1, t2);
}

Vec3f reflect_ray(Vec3f &r, Vec3f &n) {
	return n * 2 * n.dot(r) - r;
}

Vec3f multiply_mv(std::vector <std::vector< float >> &m, Vec3f &vec) {
	Vec3f result(0.0, 0.0, 0.0);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			result[i] += vec[j] * m[i][j];
		}
	}
	return result;
}

std::pair <Sphere, float> closest_intersection(Vec3f &origin, Vec3f &direction, std::vector<Sphere> &spheres, float t_min, float t_max) {
	float closest_t = std::numeric_limits<float>::infinity();
	Sphere closest_sphere = Sphere();
	for (int i = 0; i < spheres.size(); i++) {
		std::pair<float, float> intersect = intersect_ray(origin, direction, spheres[i]);
		if (intersect.first >= t_min && intersect.first < t_max && intersect.first < closest_t) {
			closest_t = intersect.first;
			closest_sphere = spheres[i];
		}
		if (intersect.second >= t_min && intersect.second < t_max && intersect.second < closest_t) {
			closest_t = intersect.second;
			closest_sphere = spheres[i];
		}
	}
	return std::make_pair(closest_sphere, closest_t);
}

double compute_lighting(Vec3f &point, Vec3f &normal, std::vector<Light> &lights, std::vector<Sphere> &spheres, Vec3f v, float specular) {
	double i { 0.0 };
	float inf = std::numeric_limits<float>::infinity();
	float t_min = 0.001;
	Vec3f light;
	for (int x = 0; x < lights.size(); ++x) {
		if (lights[x].type == "ambient") {
			i += lights[x].intensity;
		} else {
			if (lights[x].type == "point") {
				light = lights[x].position - point;
			} else {
				light = lights[x].direction;
			}

			// shadow check
			std::pair <Sphere, float> shadow = closest_intersection(point, light, spheres, t_min, inf);
			if (shadow.first.is_sphere) {
				continue;
			}

			// diffuse
			float n_dot_l = normal.dot(light);
			if (n_dot_l > 0) {
				i += lights[x].intensity * n_dot_l / ( normal.magnitude * light.magnitude );
			}

			// specular
			if (specular != -1) {
				Vec3f r;
				r = normal * 2 * normal.dot(light) - light;
				float r_dot_v = r.dot(v);
				if (r_dot_v > 0) {
					i += lights[x].intensity * std::pow(r_dot_v / ( r.magnitude * v.magnitude ), specular);
				}
			}
		}
	}
	return i;
}

Vec3f trace_ray(Vec3f &origin, Vec3f &direction, std::vector<Sphere> &spheres, std::vector<Light> &lights, float t_min, float t_max, int recursion_depth) {
	std::pair <Sphere, float> closest = closest_intersection(origin, direction, spheres, t_min, t_max);

	if (closest.first.is_sphere == false)
		return Vec3f(0, 0, 0); // background color

	Vec3f point = origin + ( direction * closest.second );
	Vec3f normal = point - closest.first.center;
	normal = normal / normal.magnitude;
	Vec3f local_color = closest.first.color * compute_lighting(point, normal, lights, spheres, -direction, closest.first.specular);

	float r = closest.first.reflectance;
	if (recursion_depth <= 0 || r <= 0) {
		return local_color;
	}

	Vec3f reflected_color;
	Vec3f neg_dir = -direction;
	Vec3f reflected_ray_color = reflect_ray(neg_dir, normal);
	reflected_color = trace_ray(point, reflected_ray_color, spheres, lights, t_min, t_max, recursion_depth - 1);

	return local_color * ( 1 - r ) + reflected_color * r;
}

void render(std::vector<Sphere> &s, std::vector<Light> &l, int recursion_depth) {
	const int width { 1024 };
	const int height { 768 };
	const int fov = M_PI / 2.;
	float inf = std::numeric_limits<float>::infinity();
	Vec3f origin(0, 3, 0);

	std::vector< std::vector<float>> m(3, std::vector<float> (3));
	m[0] = { 0.7071, 0, -0.7071 };
	m[1] = { 0, 1, 0 };
	m[2] = { 0.7071, 0, 0.7071 };

	std::vector<Vec3f> framebuffer(width * height);

	std::cout << "Creating framebuffer..." << std::endl;
	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			// framebuffer[i + j * width] = Vec3f(j / float(height), i / float(width), 0);
			float x = ( 2 * ( i + 0.5 ) / (float) width - 1 ) * tan(fov / 2.) * width / (float) height;
			float y = -( 2 * ( j + 0.5 ) / (float) height - 1 ) * tan(fov / 2.);
			Vec3f dir = Vec3f(x, y, -1).normalize();
			// dir = multiply_mv(m, dir);
			framebuffer[i + j * width] = trace_ray(origin, dir, s, l, 1.0, inf, recursion_depth);
		}
	}
	std::cout << "Done creating framebuffer!" << std::endl;

	std::ofstream ofs; // save the framebuffer to file
	ofs.open("./out_test.ppm");
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (size_t i = 0; i < height * width; ++i) {
		for (size_t j = 0; j < 3; j++) {
			ofs << (char) ( 255 * std::max(0.f, std::min(1.f, framebuffer[i][j])) );
		}
	}
	ofs.close();
	std::cout << "Image created!" << std::endl;
}

int main() {
	std::cout << "Ray tracing!" << std::endl;
	std::vector<Sphere> spheres {};
	spheres.push_back(Sphere(Vec3f(-5, 0, -15), Vec3f(0.8, 0, 0), 1.5, 500, 0.4)); // red
	spheres.push_back(Sphere(Vec3f(3, 0, -17), Vec3f(0, 1, 0), 2, 500, 0.5)); // green
	spheres.push_back(Sphere(Vec3f(-1, 0, -14), Vec3f(0, 0, 1), 2, 500, 0.4)); // blue
	spheres.push_back(Sphere(Vec3f(-10, 0, -20), Vec3f(1, 0, 1), 1.5, 500, 0.5)); // purple
	spheres.push_back(Sphere(Vec3f(7, 0, -20), Vec3f(1, 1, 0), 1.5, 500, 0.4)); // yellow (small)
	spheres.push_back(Sphere(Vec3f(0, -5001, 0), Vec3f(1, 1, 0), 5000, 1000, 0.5)); // yellow
	// Sphere sphere(Vec3f(-3, 0, -16), Vec3f(1, 0, 0), 2);

	std::vector<Light> lights {};
	lights.push_back(Light("ambient", 0.2));
	lights.push_back(Light("point", 0.6, Vec3f(2, 5, 0)));
	lights.push_back(Light("directional", 0.6, Vec3f(1, 4, 4)));

	int reflection_depth = 3;

	render(spheres, lights, reflection_depth);

	return 0;
}