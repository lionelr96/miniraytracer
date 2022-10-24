#ifndef __SPHERE_H__
#define __SPHERE_H__

struct Sphere {
	Vec3f center, color;
	float radius, specular, reflectance;
	bool is_sphere;

	Sphere() {
		is_sphere = false;
	};

	Sphere(Vec3f c, Vec3f col, float rad, float s, float ref)
		: center(c)
		, color(col)
		, radius(rad)
		, specular(s)
		, reflectance(ref)
	{
		is_sphere = true;
	}
};

#endif // __SPHERE_H__