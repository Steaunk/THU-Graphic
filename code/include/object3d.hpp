#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "ray.hpp"
#include "hit.hpp"
#include "material.hpp"

// Base class for all 3d entities.
// 基类中已经含有 Material，判断时 h.getT() 可能已经有值了。
class Object3D {
public:
    Object3D() : material(nullptr) {}

    virtual ~Object3D() = default;

    explicit Object3D(Material *material) {
        this->material = material;
    }
    virtual void description(){
        printf("I'm the Base of Object3D.\n");
    }

    virtual Vector2f UVtrans(Vector3f p){
        printf("DO NOT IMPLEMENT UVtrans.\n");
        return Vector2f::ZERO;
    };

    // Intersect Ray with this object. If hit, store information in hit structure.
    virtual bool intersect(const Ray &r, Hit &h, double tmin, unsigned short *X = nullptr) = 0; 
    Material *material;

protected:
};

#endif

