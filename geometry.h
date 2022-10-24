#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <vector>

class Point;
class Vec3f;

struct Vec3f {
	float x, y, z, magnitude;

	// Vec3f() {}
	Vec3f(float x = 0, float y = 0, float z = 0)
		: x(x)
		, y(y)
		, z(z)
	{
		// std::cout << "Vector created!" << std::endl;
		magnitude = std::sqrt(x * x + y * y + z * z);
	}

	float dot(Vec3f &w) {
		return this->x * w.x + this->y * w.y + this->z * w.z;
	}

	Vec3f normalize() {
		this->x = this->x / magnitude;
		this->y = this->y / magnitude;
		this->z = this->z / magnitude;
		return *this;
	}

	Vec3f operator+ (const Vec3f &w) {
		return Vec3f(this->x + w.x, this->y + w.y, this->z + w.z);
	}

	Vec3f operator- (const Vec3f &w) {
		return Vec3f(this->x - w.x, this->y - w.y, this->z - w.z);
	}

	Vec3f operator* (const float &x) {
		return Vec3f(this->x * x, this->y * x, this->z * x);
	}

	Vec3f operator/ (const float &x) {
		return Vec3f(this->x / x, this->y / x, this->z / x);
	}

	Vec3f operator-() {
		return Vec3f(this->x * -1, this->y * -1, this->z * -1);
	}

	float &operator[] (int i) {
		assert(i < 3);
		return i <= 0 ? x : ( 1 == i ? y : z );
	}

	// cross product
	Vec3f operator* (const Vec3f &w) {
		int x_value = this->y * w.z - this->z * w.y;
		int y_value = this->x * w.z - this->z * w.x;
		int z_value = this->x * w.y - this->y * w.x;
		return Vec3f(x_value, y_value, z_value);
	}

};

struct Matrix {
	int rows, cols, size;
	std::vector<std::vector<int>> v;

	Matrix(int rows, int cols): rows(rows), cols(cols) {
		size = rows * cols;
	}

	int &operator()(int row, int column) {
		return v[row][column];
	}

	std::vector<int> &operator[] (int row) {
		return v[row];
	}

	Matrix operator+(const Matrix n) {
		if (this->size != n.size) {
			std::cout << "Not same size!" << std::endl;
			return *this;
		}
		return *this;
	}
};

#endif // __GEOMETRY_H__