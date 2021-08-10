#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>
#include <random>

class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up);
		this->horizontal.normalize();
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point, unsigned short *X = nullptr) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
// 此处并未实现f，图像中一个像素的距离在真实场景中对应的长度
class PerspectiveCamera : public Camera {
protected:
    double fx, fy;
public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, double angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        //fx = width / tan(angle / 2) / 2;

        printf("%lf !", angle);
        fy = height / tan(angle / 2) / 2;
        fx = fy;
    }

    Ray generateRay(const Vector2f &point, unsigned short *Xi) override {
        // 首先先找出对应的大小，即画布的方位，距离
        //printf("%d %d %lf %lf \n", width, height, fx, fy);
        Vector2f c(width / 2, height / 2);
        Vector3f dRc((point.x() - c.x()) / fx, 
                     (c.y() - point.y()) / fy,
                     1);
        dRc.normalize();
        Matrix3f R(horizontal, -up, direction);
        return Ray(center, R * dRc);
    }
};

class PerspectiveCameraWithDefocus : public PerspectiveCamera {
public:
    PerspectiveCameraWithDefocus(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, double angle,
            double aperture, double focus_dist): PerspectiveCamera(center, direction, up, imgW, imgH, angle), 
                aperture(aperture), focus_dist(focus_dist){}
    
    Ray generateRay(const Vector2f &point, unsigned short *Xi) override {
        Vector3f offset = random_in_unit_disk(Xi) * aperture / 2;
        Vector2f c(width / 2, height / 2);
        Vector3f dRc((point.x() - c.x()) / fx, 
            (c.y() - point.y()) / fy,
            1);
        dRc = dRc * focus_dist - offset;

        dRc.normalize();
        Matrix3f R(horizontal, -up, direction);
        return Ray(center + R * offset, R * dRc);
    }
protected:

    Vector3f random_in_unit_disk(unsigned short *Xi) {
        double alpha = erand48(Xi) * 2 * M_PI;
        double r = erand48(Xi);
        return Vector3f(r * sin(alpha), r * cos(alpha), 0);
    }

    double aperture, focus_dist;
};

#endif //CAMERA_H
