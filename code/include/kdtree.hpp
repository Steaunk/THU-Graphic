#ifndef KDTREE_H
#define KDTREE_H 

#include <vecmath.h>
#include <vector>
#include "mesh.hpp"
#include <cmath>
using namespace std;
class KDTree : public Mesh{
protected:
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
    void get_MinMax(Vector3f &min, Vector3f &max, const vector<Vector3f> &vi){   
        for(auto i : v){
            Vector3f_CMin(min, i);
            Vector3f_CMax(max, i);
        }
    }
    Vector3f min, max;
    struct node{
        node *l = nullptr, *r = nullptr;
        vector<TriangleIndex> t;
    } *root;
    const int max_trig = 7;
    int max_step;
public:
    void description(){
        printf("I'm KDTree\n");
    }
    void init(){
        root = new node;
        get_MinMax(min, max, v);
        min.print();
        max.print();
        max_step = 8 + 1.3 * log(t.size()); //Physically Based Rendering: From Theory to Implementation
        build(root, t, 0, min, max, 0);
    }
    KDTree(Material *m) : Mesh(m){}
    KDTree(const char *filename, Material *m) : Mesh(filename, m){
        init();
    }
    int pos;
    void build(node *u, vector<TriangleIndex> t, int d, Vector3f min, Vector3f max, int step){
        if(t.size() < max_trig || step > max_step){
            u->t = t;
            return ;
        }
        double m = (min[d] + max[d]) / 2;
        u->l = new node;
        u->r = new node;
        vector<TriangleIndex> l;
        vector<TriangleIndex> r;
        for(auto i : t){
            if(v[i[0]][d] < m + 1e-5 || v[i[1]][d] < m + 1e-5 || v[i[2]][d] < m + 1e-5) 
                l.push_back(i);
            if(!(v[i[0]][d] < m - 1e-5) || !(v[i[1]][d] < m - 1e-5) || !(v[i[2]][d] < m - 1e-5))
                r.push_back(i);
        }
        Vector3f mid;
        mid = max; mid[d] = m;
        build(u->l, l, (d + 1) % 3, min, mid, step + 1);
        mid = min; mid[d] = m;
        build(u->r, r, (d + 1) % 3, mid, max, step + 1);
    }

    bool intersect_bound(const Ray &r, const double &tmin, const Vector3f &min, const Vector3f &max) {
        Vector3f pos = r.getOrigin();
        if(min.x() - 1e-5 < pos.x() && min.y() - 1e-5 < pos.y() && min.z() - 1e-5 < pos.z() && 
           pos.x() < max.x() + 1e-5 && pos.y() < max.y() + 1e-5 && pos.z() < max.z() + 1e-5)
            return true;
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
            return true;
            
        return false;
    }
    void intersect_kdtree(node *u, const Ray &r, Hit &h, double tmin, int d, Vector3f min, Vector3f max, bool &result){
        if(intersect_bound(r, tmin, min, max) == true){
            if(u->l == nullptr){
                for(auto triIndex : u->t){
                    SmoothTriangle triangle(v[triIndex[0]], v[triIndex[1]], v[triIndex[2]], 
                                            vn[triIndex[0]], vn[triIndex[1]], vn[triIndex[2]], material);
                    result |= triangle.intersect(r, h, tmin, nullptr);
                }
            }
            else{
                double m = (min[d] + max[d]) / 2;
                Vector3f mid;
                if(r.getOrigin()[d] < m){
                    mid = max; mid[d] = m;
                    intersect_kdtree(u->l, r, h, tmin, (d + 1) % 3, min, mid, result);
                    if(!result){
                        mid = min; mid[d] = m;
                        intersect_kdtree(u->r, r, h, tmin, (d + 1) % 3, mid, max, result);
                    }
                }
                else{
                    mid = min; mid[d] = m;
                    intersect_kdtree(u->r, r, h, tmin, (d + 1) % 3, mid, max, result);
                    if(!result){
                        mid = max; mid[d] = m;
                        intersect_kdtree(u->l, r, h, tmin, (d + 1) % 3, min, mid, result);
                    }
                }
            }
        }
    }
    bool intersect(const Ray &r, Hit &h, double tmin, unsigned short *X) {
        bool result = false;
        intersect_kdtree(root, r, h, tmin, 0, min, max, result);
        return result;
    }
};

#endif 