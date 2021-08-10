#ifndef BOUND_H
#define BOUND_H 

#include <vecmath.h>
#include <vector>
#include "object3d.hpp"
#include <cmath>
using namespace std;
class Bound : public Object3D{
    void Vector3f_CMin(Vector3f &a, const Vector3f &b){
        if(a.x() > b.x()) a.x() = b.x();
        if(a.y() > b.y()) a.y() = b.y();
        if(a.z() > b.z()) a.z() = b.z();
    }
    void Vector3f_CMax(Vector3f &a, const Vector3f &b){
        if(a.x() < b.x()) a.x() = b.x();
        if(a.y() < b.y()) a.y() = b.y();
        if(a.z() < b.z()) a.z() = b.z();
    }
public:
    Bound(const vector<Vector3f> v, Object3D *obj) : o(obj){
        min = max = v[0];
        for(auto i : v){
            Vector3f_CMin(min, i);
            Vector3f_CMax(max, i);
        }
    }

    virtual bool intersect(const Ray &r, Hit &h, double tmin, unsigned short *X) {
        Vector3f pos = r.getOrigin();
        if(min.x() - 1e-5 < pos.x() && min.y() - 1e-5 < pos.y() && min.z() - 1e-5 < pos.z() && 
           pos.x() < max.x() + 1e-5 && pos.y() < max.y() + 1e-5 && pos.z() < max.z() + 1e-5)
            return o->intersect(r, h, tmin);
        double x, y, z;
        x = pos.x() < min.x() ? min.x() : max.x();
        y = pos.y() < min.y() ? min.y() : max.y();
        z = pos.z() < min.z() ? min.z() : max.z();

        Vector3f dir = r.getDirection();
        double t = tmin;
        if(fabs(dir.x()) > 1e-5 && !(min.x() - 1e-5 < pos.x() && pos.x() < max.x() + 1e-5))
            t = std::max(t, (x - pos.x()) / dir.x());        
        if(fabs(dir.y()) > 1e-5 && !(min.y() - 1e-5 < pos.y() && pos.y() < max.y() + 1e-5))
            t = std::max(t, (y - pos.y()) / dir.y());        
        if(fabs(dir.z()) > 1e-5 && !(min.z() - 1e-5 < pos.z() && pos.z() < max.z() + 1e-5))
            t = std::max(t, (z - pos.z()) / dir.z());        

        pos = pos + t * dir;
        
        if(min.x() - 1e-5 < pos.x() && min.y() - 1e-5 < pos.y() && min.z() - 1e-5 < pos.z() && 
           pos.x() < max.x() + 1e-5 && pos.y() < max.y() + 1e-5 && pos.z() < max.z() + 1e-5)
            return o->intersect(r, h, tmin);
            
        return false;
    }

protected:
    Object3D *o; //un-transformed object
    Vector3f min, max;
};

#endif 
