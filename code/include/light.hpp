#ifndef LIGHT_H
#define LIGHT_H

#include <Vector3f.h>
#include "object3d.hpp"
#include "ray.hpp"


class Light {
protected:
    Vector3f color;
    Vector3f random_in_unit_sphere(unsigned short *Xi){
        double alpha = erand48(Xi) * M_PI * 2;
        double beta = erand48(Xi) * M_PI;
        return Vector3f(sin(beta) * cos(alpha), sin(beta) * sin(alpha), cos(beta));
    }
public:
    Light() = default;

    virtual ~Light() = default;
    virtual Ray randomEmit(unsigned short *Xi){
        puts("Haven't finished (Light.hpp)");
    }
    virtual Vector3f getColor(){
        return color;
    }
    virtual void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const = 0;
};


class DirectionalLight : public Light {
public:
    DirectionalLight() = delete;

    DirectionalLight(const Vector3f &d, const Vector3f &c) {
        direction = d.normalized();
        color = c;
    }

    ~DirectionalLight() override = default;

    ///@param p unsed in this function
    ///@param distanceToLight not well defined because it's not a point light
    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = -direction;
        col = color;
    }

private:
    
    Vector3f direction;

};

class CircleLight : public Light {
public:
    CircleLight() = delete;

    CircleLight(const Vector3f &center, Vector3f up, Vector3f left,
        double radius, const Vector3f &c): center(center), radius(radius){
        dir = Vector3f::cross(up, left);
        left.normalize();
        dir.normalize();
        up = Vector3f::cross(left, dir);
        color = c;
        d = Matrix3f(left, -up, dir);
    }

    ~CircleLight() override = default;

    Ray randomEmit(unsigned short *X){
        double alpha = erand48(X) * 2 * M_PI;
        double r = pow(erand48(X), 0.5);
        return Ray(
            center + d * Vector3f(r * sin(alpha), r * cos(alpha), 0) * radius,
            dir
        );
    }

    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const override {
    }
private:
    Vector3f center, dir;
    Matrix3f d;
    double radius;
};

class PointLight : public Light {
public:
    PointLight() = delete;

    PointLight(const Vector3f &p, const Vector3f &c) {
        position = p;
        color = c;
    }

    ~PointLight() override = default;
    Ray randomEmit(unsigned short *Xi){
	Vector3f dir = random_in_unit_sphere(Xi);
//	if(dir[1] > 0) dir[1] = -dir[1];
        return Ray(position, dir);
    }
    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = (position - p);
        dir = dir / dir.length();
        col = color;
    }

private:

    Vector3f position;

};

#endif // LIGHT_H
