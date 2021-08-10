#ifndef RECTANGLE_H
#define RECTANGLE_H 

#include "plane.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions
// 禁止隐式构造
// nP+D=0
class Rectangle : public Plane {
public:
    Rectangle() = delete;

    Rectangle(const Vector3f &normal, double d, Vector3f min, Vector3f max, Material *m): Plane(normal, d, m), min(min), max(max){
    }

    ~Rectangle() override = default;

    void description(){
        printf("I'm the Rectangle.\n");
    }

    Vector2f UVtrans(Vector3f pos){
        pos -= min;
        //puts("UV");
        if(dd == 0) return Vector2f(pos[1], pos[2]);
        if(dd == 1) return Vector2f(pos[2], pos[0]);
        if(dd == 2) return Vector2f(pos[0], pos[1]);
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
        if(t < h.getT()){\
            Vector3f tmp = r.pointAtParameter(t); 
            if(dd != 0 && (min[0] > tmp[0] || tmp[0] > max[0])) return false;
            if(dd != 1 && (min[1] > tmp[1] || tmp[1] > max[1])) return false;
            if(dd != 2 && (min[2] > tmp[2] || tmp[2] > max[2])) return false;
           
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
    Vector3f min, max;
};

#endif //PLANE_H
		

