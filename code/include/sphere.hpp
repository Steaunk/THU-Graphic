#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary
//需要注意 Ray 可能不是单位向量！
class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
        center = Vector3f::ZERO;
        radius = 1;
    }

    Sphere(const Vector3f &center, double radius, Material *material) : Object3D(material) {
        // 
        this->center = center;
        this->radius = radius;
    }

    ~Sphere() override = default;

    void description(){
        printf("I'm the Sphere.\n");
    }

    bool intersect(const Ray &r, Hit &h, double tmin, unsigned short *X) override {
        //
        Vector3f l = center - r.getOrigin();
        double tp = Vector3f::dot(l, r.getDirection().normalized());
        if(l.length() > radius && tp < 0) return false;
        double d_square = l.length() * l.length() - tp * tp;
        if(d_square > radius * radius) return false;
        double tt = sqrt(radius * radius - d_square);
        double t;
        if(l.length() > radius) t = tp - tt;
        else t = tp + tt;
        if(t <= tmin) return false;
        Vector3f normal = r.pointAtParameter(t) - center;
        //if(l.length() < radius) normal = -normal;
        if(t < h.getT()){
            h = Hit(t, material, normal.normalized(), UVtrans(normal));
            return true;
        }
        return false;
    }

    Vector2f UVtrans(Vector3f p) override {
        p.normalize();
        if(fabs(p[0]) < 1e-9 && fabs(p[1]) < 1e-9) return Vector2f(0, 0);
        return Vector2f((M_PI + atan2(p[1], p[0])) / (2 * M_PI), (M_PI - acos(p[2])) / M_PI);
    }   

protected:
    Vector3f center;
    double radius;
};

class FogSphere : public Sphere{
public:
    FogSphere(const Vector3f &center, double radius, Material *material) : Sphere(center, radius, material) {}
    void description(){
        printf("I'm the FogSphere.\n");
    }

    bool intersect(const Ray &r, Hit &h, double tmin, unsigned short *X) override {
        Vector3f l = center - r.getOrigin();
        double tp = Vector3f::dot(l, r.getDirection().normalized());
        if(l.length() > radius && tp < 0) return false;
        double d_square = l.length() * l.length() - tp * tp;
        if(d_square > radius * radius) return false;
        double tt = sqrt(radius * radius - d_square);
        double t;
        if(l.length() > radius) t = tp - tt;
        else t = tp + tt;
        if(t <= tmin) return false;
        //if(l.length() < radius) normal = -normal;
        if(t < h.getT()){
            double hit_distance = -(1 / density) * log(erand48(X));
            if(l.length() < radius){
                if(hit_distance > t) return false;
                t = hit_distance;
            }
            else{
                Vector3f pos = r.pointAtParameter(t + hit_distance) - center;
                if(pos.length() > radius) return false;
                t += hit_distance;
            }
            Vector3f normal = random_in_unit_sphere(X);
            //r.pointAtParameter(t).print();
            
            //printf("%lf ? %lf %lf\n", (r.pointAtParameter(t)-center).length(), hit_distance, radius);
            h = Hit(t, material, normal.normalized(), UVtrans(normal));
            return true;
        }
        return false;
    }
private:
    double density = 0.5;
    Vector3f random_in_unit_sphere(unsigned short *Xi){
        double alpha = erand48(Xi) * M_PI * 2;
        double beta = erand48(Xi) * M_PI;
        return Vector3f(sin(beta) * cos(alpha), sin(beta) * sin(alpha), cos(beta));
    }
};

#endif
