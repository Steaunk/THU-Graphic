#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

// TODO: implement this class and add more fields as necessary,
class Triangle: public Object3D {

public:
	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		normal = Vector3f::cross(b - a, c - a);
		normal = normal.normalized();
	}

	void description(){
		printf("I'm the Triangle.\n");
	}

	bool intersect( const Ray& ray,  Hit& hit , double tmin, unsigned short *X) override {
		Vector3f E1 = vertices[0] - vertices[1];
		Vector3f E2 = vertices[0] - vertices[2];
		Vector3f S = vertices[0] - ray.getOrigin();
		Vector3f t(
			Matrix3f(S, E1, E2).determinant(),
			Matrix3f(ray.getDirection(), S, E2).determinant(),
			Matrix3f(ray.getDirection(), E1, S).determinant()
		);
		double m = Matrix3f(ray.getDirection(), E1, E2).determinant();
		if(fabs(m) < 1e-9) return false;
		// t = [t, beta, gamma]
		t /= m;
		if(t[0] > tmin && 0 <= t[1] && t[1] <= 1 && 0 <= t[2] && t[2] <= 1 && t[1] + t[2] <= 1){
			if(t[0] < hit.getT()){
				hit = Hit(t[0], material, normal);
				return true;
			}
		}
        return false;
	}
	Vector3f normal;
	Vector3f vertices[3];
protected:

};

#endif //TRIANGLE_H
