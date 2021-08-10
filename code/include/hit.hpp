#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"
#include "object3d.hpp"

class Object3D;
class Material;

//交点
class Hit {
public:

    // constructors
    Hit() {
        material = nullptr;
        t = 1e38;
    }
    Hit(double _t, Material *m, const Vector3f &n, Vector2f u = Vector2f::ZERO){
        t = _t;
        material = m;
        normal = n;
        uv = u;
    }

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
        uv = h.uv;
    }

    // destructor
    ~Hit() = default;

    double getT() const {
        return t;
    }

    Material *getMaterial() const {
        return material;
    }

    Vector2f getUV() const {
        return uv;
    }

    const Vector3f &getNormal() const {
        return normal;
    }

    void set(double _t, Material *m, const Vector3f &n, Vector2f u = Vector2f::ZERO) {
        t = _t;
        material = m;
        normal = n;
        uv = u;
    }

    Vector3f normal;

private:
    double t;
    Material *material;
    Vector2f uv;
    Matrix3f m;

};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
