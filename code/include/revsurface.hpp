#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include "object3d.hpp"
#include "curve.hpp"
#include "kdtree.hpp"
#include <tuple>
#include <map>

class RevSurface : public KDTree {

    const int resolution = 40, steps = 40;
    Curve *pCurve;
    int size;
public:
    RevSurface(Curve *pCurve, Material* material) : pCurve(pCurve), KDTree(material) {
        // Check flat.
        for (const auto &cp : pCurve->getControls()) {
            if (cp.z() != 0.0) {
                printf("Profile of revSurface must be flat on xy plane.\n");
                exit(0);
            }
        }
        typedef std::tuple<unsigned, unsigned, unsigned> Tup3u;
        // Surface is just a struct that contains vertices, normals, and
        // faces.  VV[i] is the position of vertex i, and VN[i] is the normal
        // of vertex i.  A face is a triple i,j,k corresponding to a triangle
        // with (vertex i, normal i), (vertex j, normal j), ...
        // Currently this struct is computed every time when canvas refreshes.
        // You can store this as member function to accelerate rendering.

        struct Surface {
            std::vector<Vector3f> VV;
            std::vector<Vector3f> VN;
            std::vector<Tup3u> VF;
        } surface;

        std::vector<CurvePoint> curvePoints;
        pCurve->discretize(resolution, curvePoints);
        size = curvePoints.size();
        for (unsigned int ci = 0; ci < size; ++ci) {
            const CurvePoint &cp = curvePoints[ci];
            for (unsigned int i = 0; i < steps; ++i) {
                float t = (float) i / steps;
                Quat4f rot;
                rot.setAxisAngle(t * 2 * M_PI, Vector3f::UP);
                Vector3f pnew = Matrix3f::rotation(rot) * cp.V;
                Vector3f pNormal = Vector3f::cross(cp.T, -Vector3f::FORWARD);
                Vector3f nnew = Matrix3f::rotation(rot) * pNormal;
                surface.VV.push_back(pnew);
                surface.VN.push_back(nnew);
                int i1 = (i + 1 == steps) ? 0 : i + 1;
                if (ci != curvePoints.size() - 1) {
                    surface.VF.emplace_back((ci + 1) * steps + i, ci * steps + i1, ci * steps + i);
                    surface.VF.emplace_back((ci + 1) * steps + i, (ci + 1) * steps + i1, ci * steps + i1);
                }
            }
        }
        vn = surface.VN;
        v = surface.VV;
        for (unsigned i = 0; i < surface.VF.size(); i++) {
            TriangleIndex trig;
            
            trig[0] = std::get<0>(surface.VF[i]);
            trig[1] = std::get<1>(surface.VF[i]);
            trig[2] = std::get<2>(surface.VF[i]);
            t.push_back(trig);
        }

        init();
    }
    void intersect_kdtree(node *u, const Ray &r, Hit &h, double tmin, int d, Vector3f min, Vector3f max, int &result){
        if(intersect_bound(r, tmin, min, max) == true){
            if(u->l == nullptr){
                for(auto triIndex : u->t){
                    SmoothTriangle triangle(v[triIndex[0]], v[triIndex[1]], v[triIndex[2]], 
                                            vn[triIndex[0]], vn[triIndex[1]], vn[triIndex[2]], material);
                    if(triangle.intersect(r, h, tmin, nullptr)) result = triIndex[0];
                }
            }
            else{
                double m = (min[d] + max[d]) / 2;
                Vector3f mid;
                if(r.getOrigin()[d] < m){
                    mid = max; mid[d] = m;
                    intersect_kdtree(u->l, r, h, tmin, (d + 1) % 3, min, mid, result);
                    if(result == -1){
                        mid = min; mid[d] = m;
                        intersect_kdtree(u->r, r, h, tmin, (d + 1) % 3, mid, max, result);
                    }
                }
                else{
                    mid = min; mid[d] = m;
                    intersect_kdtree(u->r, r, h, tmin, (d + 1) % 3, mid, max, result);
                    if(result == -1){
                        mid = max; mid[d] = m;
                        intersect_kdtree(u->l, r, h, tmin, (d + 1) % 3, min, mid, result);
                    }
                }
            }
        }
    }
    void Newton_iteration(const Ray &r, double &u, double &v, double &t){
        int round = 15;
        for(int i = 0; i < round; ++i){
            CurvePoint B = pCurve->getCurvePoint(v);
            //CurvePoint pu = pCurve->getCurvePoint(u);
            Vector3f x = Vector3f(v, u, t) - Matrix3f(
                Vector3f(B.T[0] * cos(u),  -B.V[0] * sin(u), -r.getDirection()[0]),
                Vector3f(B.T[1],           0,                 -r.getDirection()[1]),
                Vector3f(-B.T[0] * sin(u), -B.V[0] * cos(u), -r.getDirection()[2]),
                false
            ).inverse() * (Vector3f(B.V[0] * cos(u), B.V[1], -B.V[0] * sin(u)) - r.pointAtParameter(t));
            v = x[0]; u = x[1]; t = x[2];
        }
    }
    bool intersect(const Ray &r, Hit &h, double tmin, double cur_time){
        int result = -1;
        intersect_kdtree(root, r, h, tmin, 0, min, max, result);
        if(result == -1) return false;
        return true;
        double u = result % steps * 2.0 / steps  * M_PI, v = 1.0 * result / steps / size;
        double t = h.getT();
        Newton_iteration(r, u, v, t);
        Quat4f rot;
        rot.setAxisAngle(u, Vector3f::UP);
        Vector3f normal = Vector3f::cross(pCurve->getCurvePoint(v).T, -Vector3f::FORWARD);
        normal = (Matrix3f::rotation(rot) * normal).normalized();
        h = Hit(t, h.getMaterial(), normal, Vector2f(t, u / (2 * M_PI)));
        return true;
    }

    ~RevSurface() override {
        delete pCurve;
    }

};

#endif //REVSURFACE_HPP
