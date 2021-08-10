#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "image.hpp"
#include "object3d.hpp"
#include <iostream>
#include <algorithm>

using namespace std;
// TODO: Implement Shade function that computes Phong introduced in class.

class Texture{
public:
    Image *image = nullptr;
    Image *normal = nullptr;
    double rx = 1, ry = 1;
    ~Texture(){
        delete image;
    }
    Texture() = default;
    Texture(Image *image, double rx, double ry): image(image), rx(rx), ry(ry){}
    Texture(Image *image): image(image){}

    void getPos(int &a, int &b, const Vector2f &p){
        a = ((int)(p[0] / rx * image->Width())) % image->Width();
        a = (a + image->Width()) % image->Width();
        b = ((int)(p[1] / ry * image->Height())) % image->Height();
        b = (b + image->Height()) % image->Height();
    }
    
    Vector3f getNormal(int ix, int iy){
        return 2 * normal->GetPixel(ix, iy) - (Vector3f)1;
    }

    Vector3f getColor(const Vector2f &p) {
        
        int ix, iy;
        getPos(ix, iy, p);
        return image->GetPixel(ix, iy);
    }

};

class Object3D;

class Material{
public:
    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, const Vector3f &color = Vector3f::ZERO, double s = 0, Vector3f e = 0, string t = string("DIFF")) : diffuseColor(d_color), specularColor(s_color), Color(color), shininess(s), emission(e), type(t){
    }

    virtual ~Material(){
        delete texture;
    }

    virtual Vector3f getDiffuseColor() const
    {
        return diffuseColor;
    }
    virtual Vector3f getSpecularColor() const
    {
        return specularColor;
    }
    //virtual Vector3f getColor() const
    //{
    //    if(texture->image != nullptr) puts("$");
    //    return Color;
    //}
    virtual Vector3f getColor(const Vector2f &uv)
    {
        if(texture->image == nullptr) return Color;
        else {
            return texture->getColor(uv);
        }
    }

    Texture *getTexture() const {
        return texture;
    }

    virtual Vector3f getEmission() const
    {
        return emission;
    }
    virtual double getN() const
    {
        return n;
    }
    virtual double getFuzz() const
    {
        return fuzz;
    }
    void setN(double n)
    {
        this->n = n;
    }
    void setFUZZ(double fuzz)
    {
        this->fuzz = fuzz;
    }
    void setTexture(Texture *texture){
        this->texture = texture;
    }
    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor)
    {
        using namespace std;
        Vector3f shaded = Vector3f::ZERO;
        //
        Vector3f N = hit.getNormal();
        Vector3f Li = dirToLight.normalized();
        Vector3f diffuse = diffuseColor * max(Vector3f::dot(N, Li), (double)0);
        Vector3f V = -ray.getDirection().normalized();
        Vector3f R = 2 * Vector3f::dot(N, Li) * N - Li;
        R.normalize();
        Vector3f specular = pow(max(Vector3f::dot(V, R), (double)0), shininess) * specularColor;
        shaded = diffuse + specular;
        return lightColor * shaded;
    }
    std::string type;
    Vector3f emission;

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    Vector3f Color;
    double shininess, fuzz = 0, n = 1.5;
    Texture *texture;
};

#endif // MATERIAL_H
