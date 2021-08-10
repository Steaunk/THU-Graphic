#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>

// TODO (PA3): Implement Bernstein class to compute spline basis function.
//       You may refer to the python-script for implementation.

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve : public Object3D {
protected:
    std::vector<Vector3f> controls;
    double *B, *Bn;
    int n;
public:
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {
        n = controls.size() - 1;
    }

    bool intersect(const Ray &r, Hit &h, double tmin, unsigned short *X) override {
        return false;
    }

    std::vector<Vector3f> &getControls() {
        return controls;
    }
    virtual CurvePoint getCurvePoint(double t) = 0;
    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;

};

class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
        B = new double[n + 1];
        Bn = new double[n + 1];
    }
    CurvePoint getCurvePoint(double t){
        for(int i = 0; i <= n; ++i) B[i] = 1;
            for(int i = 1; i <= n; ++i){
                if(i == n){
                    for(int j = 0; j < n; ++j)
                        Bn[j] = n * (B[j - 1] - B[j]);
                    Bn[n] = n * B[n - 1];
                }
                B[i] = t * B[i - 1];
                for(int j = i - 1; j; --j)
                    B[j] = (1 - t) * B[j] + t * B[j - 1];
                B[0] = (1 - t) * B[0];
            }
        Vector3f P, Pn;
        for(int i = 0; i <= n; ++i) P += controls[i] * B[i];
        for(int i = 0; i <= n; ++i) Pn += controls[i] * Bn[i];
        return (CurvePoint){P, Pn};
    }
    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA3): fill in data vector
        for(int k = 0; k <= resolution; ++k){
            double t = 1.0 / resolution * k;
            data.push_back(
                getCurvePoint(t)
            );
        }
    }

protected:

};

class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }

        t = new double[n + k + 2];
        B = new double[n + k + 2];
        Bn = new double[n + k + 2];
    }
    CurvePoint getCurvePoint(double tt){
        for(int i = 0; i <= n + k + 1; ++i){
            if(t[i] <= tt && tt < t[i + 1])
                B[i] = 1;
            else B[i] = 0;
        }
        for(int i = 1; i <= k; ++i){
            if(i == k){
                for(int j = 0; j <= n; ++j)
                    Bn[j] = k * (B[j] / (t[j + k] - t[j]) - B[j + 1] / (t[j + k + 1] - t[j + 1]));
            }
            for(int j = 0; j <= n + k - i; ++j){
                B[j] = (tt - t[j]) / (t[j + i] - t[j]) * B[j]
                    + (t[j + i + 1] - tt) / (t[j + i + 1] - t[j + 1]) * B[j + 1];
            }
        }
        
        Vector3f P, Pn;
        for(int i = 0; i <= n; ++i) P += controls[i] * B[i];
        for(int i = 0; i <= n; ++i) Pn += controls[i] * Bn[i];
        return (CurvePoint){P, Pn};
    }
    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA3): fill in data vector
        for(int i = 0; i < n + k + 2; ++i) t[i] = 1.0 * i / (n + k + 1);
        double w = (t[n + 1] - t[k]) / resolution;
        for(int u = k; u <= n; ++u){
            double w = (t[u + 1] - t[u]) / resolution;
            double tt = t[u];
            for(int v = 0; v < resolution; ++v, tt += w){
                data.push_back(
                    getCurvePoint(tt)
                );
            }
        }
    }


protected:
    double *t;
    const int k = 3;
};

#endif // CURVE_HPP
