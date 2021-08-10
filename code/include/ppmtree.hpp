#ifndef PPMTREE_H
#define PPMTREE_H 

#include <vecmath.h>
#include <vector>
#include "mesh.hpp"
#include <cmath>
using namespace std;
class PPMnode{
public:
    Vector3f p, n, color;
    int x, y;
    PPMnode() = default;
    PPMnode(Vector3f p, Vector3f n, Vector3f color, int x, int y): p(p), n(n), color(color), x(x), y(y){}
};

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

class PPMTree {
protected:
    class node{
    public:
        PPMnode v;
        node *l = nullptr, *r = nullptr;
        Vector3f min, max;
    } *root;
    void del(node *u){
        if(u->l != nullptr) del(u->l);
        if(u->r != nullptr) del(u->r);
        delete u;
    }
public:
    void description(){
        printf("I'm PPMTree\n");
    }
    PPMTree(vector<PPMnode> &v){
        root = new node;
        //get_MinMax(root->min, root->max, v);
        build(root, 0, v, 0, v.size());
    }
    ~PPMTree(){
        del(root);
    }
    void build(node *u, int d, vector<PPMnode> &v, int l, int r){
        int mid = l + r >> 1;

        nth_element(v.begin() + l, v.begin() + mid, v.begin() + r, [=](const PPMnode &i, const PPMnode &j){
            return i.p[d] < j.p[d];
        });

        u->v = v[mid]; 
        u->min = u->max = v[mid].p;

        if(r - l == 1) return ;

        if(mid - l > 0){
            build(u->l = new node, (d + 1) % 3, v, l, mid);
            Vector3f_CMax(u->max, u->l->max);
            Vector3f_CMin(u->min, u->l->min);
        }
        if(r - mid - 1 > 0){
            build(u->r = new node, (d + 1) % 3, v, mid + 1, r);
            Vector3f_CMax(u->max, u->r->max);
            Vector3f_CMin(u->min, u->r->min);
        }
    }
    void find(vector<const PPMnode*> &result, const Vector3f& pos, const double &r){
        find(root, result, pos, r);
    }
    void find(node* u, vector<const PPMnode*> &result, const Vector3f& pos, const double &r) {
        double dx, dy, dz;
        if (pos[0] <= u->max[0] && pos[0] >= u->min[0]) dx = 0;
        else dx = min(fabs(pos[0] - u->max[0]), fabs(pos[0] - u->min[0]));
        if (pos[1] <= u->max[1] && pos[1] >= u->min[1]) dy = 0;
        else dy = min(fabs(pos[1] - u->max[1]), fabs(pos[1] - u->min[1]));
        if (pos[2] <= u->max[2] && pos[2] >= u->min[2]) dz = 0;
        else dz = min(fabs(pos[2] - u->max[2]), fabs(pos[2] - u->min[2]));

        if (dx * dx + dy * dy + dz * dz > r * r) return ;

        if((pos - u->v.p).length() <= r) result.push_back(&(u->v));

        if(u->l != nullptr) find(u->l, result, pos, r);
        if(u->r != nullptr) find(u->r, result, pos, r);
    }
};

#endif 