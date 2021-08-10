#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions
// 禁止隐式构造
// nP+D=0
class Plane : public Object3D {
public:
    Plane() = delete;

    Plane(const Vector3f &normal, double d, Material *m) : Object3D(m), d(d) {
        this->normal = normal.normalized();
        //this->normal = normal;
        if(fabs(normal[0]) > 1e-5) this->dd = 0;
        if(fabs(normal[1]) > 1e-5) this->dd = 1;
        if(fabs(normal[2]) > 1e-5) this->dd = 2;
    }

    ~Plane() override = default;

    Vector2f UVtrans(Vector3f pos){
        if(dd == 0) return Vector2f(pos[1], pos[2]);
        if(dd == 1) return Vector2f(pos[2], pos[0]);
        if(dd == 2) return Vector2f(pos[0], pos[1]);
    }

    void description(){
        printf("I'm the Plane.\n");
    }

    //平面求交
    //当误差小于1e-9时认为是平行，即不相交
    bool intersect(const Ray &r, Hit &h, double tmin, unsigned short *X) override {
        double nRd = Vector3f::dot(normal, r.getDirection());
        if(fabs(nRd) < 1e-9)
            return false;
        double nR0 = Vector3f::dot(normal, r.getOrigin());
        double t = -(nR0 + d) / nRd;
        if(t <= tmin) return false;
        if(t < h.getT()){
            h = Hit(t, material, normal);
            
            Vector3f tmp = r.pointAtParameter(t);
            Vector2f uv = UVtrans(tmp);
            tmp = normal;
            Texture *texture = material->getTexture();
            if(texture->normal != nullptr){
                int ix, iy;
                texture->getPos(ix, iy, uv);
                tmp = texture->getNormal(ix, iy);
                if(dd == 0) tmp = Vector3f(tmp[2], tmp[0], tmp[1]);
                if(dd == 1) tmp = Vector3f(tmp[1], tmp[2], tmp[0]); 
            }
            h = Hit(t, material, tmp.normalized(), uv);
            return true;
        }
        return false;
    }

protected:
    Vector3f normal;
    double d;
    int dd;

};

#endif //PLANE_H
		

