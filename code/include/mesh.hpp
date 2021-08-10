#ifndef MESH_H
#define MESH_H

#include <vector>
#include "object3d.hpp"
#include "smoothtriangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"


class Mesh : public Object3D {

public:
    Mesh(Material *m);
    Mesh(const char *filename, Material *m);
    Mesh(std::vector<Vector3f> VV, std::vector<Vector3f> VN, Material *material);

    struct TriangleIndex {
        TriangleIndex() {
            x[0] = 0; x[1] = 0; x[2] = 0;
        }
        int &operator[](const int i) { return x[i]; }
        // By Computer Graphics convention, counterclockwise winding is front face
        int x[3]{};
    };

    std::vector<Vector3f> v, vn;
    std::vector<TriangleIndex> t;
    std::vector<Vector3f> n;
    bool intersect(const Ray &r, Hit &h, double tmin, unsigned short *X) override;

private:

    // Normal can be used for light estimation
    void computeNormal();
};

#endif
