#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "omp.h"

#include <string>
#include <random>
using namespace std;

int spp = 100;
Vector3f random_in_unit_sphere(unsigned short *Xi){
    double alpha = erand48(Xi) * M_PI * 2;
    double beta = erand48(Xi) * M_PI;
    return Vector3f(sin(beta) * cos(alpha), sin(beta) * sin(alpha), cos(beta));
}

Vector3f radiance(const Ray &ray, SceneParser *sceneParser, int depth, unsigned short *Xi, int E = 1){
    /*double rrFactor = 1.0;
	if(depth >= 5){
		const double rrStopProbability = 0.1;
		if(RND2 <= rrStopProbability) return ;
		rrFactor = 1.0 / (1.0 - rrStopProbability);
	}*/
    //printf("%lf\n", depth); fflush(stdout);
    ++depth;

	Hit hit;
	bool isIntersect = sceneParser->getGroup()->intersect(ray, hit, 1e-8);
	if(isIntersect == false){
        //clr = sceneParser->getBackgroundColor();
        return Vector3f::ZERO;
    }
    /*if(Vector3f::dot(ray.getDirection(), hit.getNormal()) > 0){
        //puts("!");
        hit.normal = -hit.getNormal();
    }*/
    Material *m = hit.getMaterial();

    Vector3f x = ray.pointAtParameter(hit.getT());
    Vector3f n = hit.getNormal();
    Vector3f nl = Vector3f::dot(n, ray.getDirection()) < 0 ? n : -n;
    Vector3f f = m->getColor(hit.getUV());
    //printf("%lf %lf %lf\n", f.x(), f.y(), f.z());
    double p = max(f.x(), max(f.y(), f.z()));
    if(depth > 5){
        if(erand48(Xi) < p) f /= p;
        else return m->getEmission();
    }
    Ray reflRay(x, (ray.getDirection() - n * 2 * Vector3f::dot(n, ray.getDirection())).normalized());
            
    if(m->type == "DIFF"){
        double r1 = 2 * M_PI * erand48(Xi), 
              r2 = erand48(Xi), 
              r2s = sqrt(r2);

        Vector3f w = nl;
        Vector3f u, v;

        if(fabs(w.x()) > .1)
            u = Vector3f::cross(Vector3f(0, 1, 0), w).normalized();
        else 
            u = Vector3f::cross(Vector3f(1, 0, 0), w).normalized();
        v = Vector3f::cross(w, u);
        Vector3f d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normalized();
        return m->getEmission() + f * radiance(Ray(x, d), sceneParser, depth, Xi);
    }
    else if(m->type == "SPEC"){
        //高光反射时的 f 用于模拟不同的金属，金属会立即吸收任何透射光
        if(m->getFuzz() < 1e-5)
            return m->getEmission() + f * radiance(reflRay, sceneParser, depth, Xi);
        else {
            reflRay.setDirection(reflRay.getDirection() + m->getFuzz() * random_in_unit_sphere(Xi));
            return m->getEmission() + f * radiance(reflRay, sceneParser, depth, Xi);
        }
    }

    bool into = Vector3f::dot(n, nl) > 0;

    double nc = 1, nt = 1.5; //nt 是折射率
    double nnt = into ? nc / nt : nt / nc, ddn = Vector3f::dot(ray.getDirection(), nl);
    double cos2t = 1 - nnt * nnt * (1 - ddn * ddn);

    if(cos2t < 0)
        return m->getEmission() + f * radiance(reflRay, sceneParser, depth, Xi);

    Vector3f tdir = (ray.getDirection() * nnt - n * ((into ? 1 : -1) *
        (ddn * nnt + sqrt(cos2t)))).normalized();

    double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : Vector3f::dot(tdir, n));
    double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);

    return m->getEmission() + f * (
        depth > 2 ? 
            (erand48(Xi) < P ? 
                radiance(reflRay, sceneParser, depth, Xi) * RP :
                radiance(Ray(x, tdir), sceneParser ,depth, Xi) * TP
            ) :
            radiance(reflRay, sceneParser,depth, Xi) * Re 
            + radiance(Ray(x, tdir), sceneParser ,depth, Xi) * Tr
    );

    /*
    ray.setOrigin(ray.pointAtParameter(hit.getT()));
    const double emission = hit.getMaterial()->emission;
    clr = clr + Vector3f(emission, emission, emission) * rrFactor;

    if(hit.getMaterial()->type == "DIFF"){
        Vector3f rotX, rotY;
        ons(N, rotX, rotY);
        Vector3f sampledDir = hemisphere(RND2, RND2);
        Vector3f rotatedDir;
        rotatedDir.x() = Vector3f::dot(Vector3f(rotX.x(), rotY.x(), N.x()), sampledDir);
        rotatedDir.y() = Vector3f::dot(Vector3f(rotX.y(), rotY.y(), N.y()), sampledDir);
        rotatedDir.z() = Vector3f::dot(Vector3f(rotX.z(), rotY.z(), N.z()), sampledDir);
        ray.setDirection(rotatedDir);
        double cost = Vector3f::dot(rotatedDir, N);
        Vector3f tmp;
        trace(ray, sceneParser, depth + 1, tmp);
        clr = clr + (tmp * hit.getMaterial()->getColor()) * cost * 0.1 * rrFactor;
    }
    if(hit.getMaterial()->type == "SPEC"){
        double cost = Vector3f::dot(ray.getDirection(), N);
        ray.setDirection((ray.getDirection() - N * (cost * 2)).normalized());
        Vector3f tmp = Vector3f::ZERO;
        trace(ray, sceneParser, depth + 1, tmp);
        clr = clr + tmp * rrFactor;
        //clr = clr + tmp * hit.getMaterial()->getSpecularColor() * rrFactor;
    }
    if(hit.getMaterial()->type == "REFR"){
        double n = refr_index;
        double R0 = (1.0 - n) / (1.0 + n);
        R0 = R0 * R0;
        if(Vector3f::dot(N, ray.getDirection()) > 0)
            N = -N;
        else n = 1 / n;
        double cost1 = -Vector3f::dot(N, ray.getDirection());
        double cost2 = 1.0 - n * n * (1.0 - cost1 * cost1);
        double Rprob = R0 + (1.0 - R0) * pow(1.0 - cost1, 5.0);
        if(cost2 > 0 && RND > Rprob){
            ray.setDirection((ray.getDirection() * n + N * (n * cost1 - sqrt(cost2))).normalized());
        }
        else {
            ray.setDirection((ray.getDirection() + N * (cost1 * 2)).normalized());
        }
        Vector3f tmp;
        trace(ray, sceneParser, depth + 1, tmp);
        clr = clr + tmp * 1.15 * rrFactor;
    }*/
   /*if(clr.length() > 1e-9){
        printf("%d : %lf %lf %lf\n", depth, clr.x(), clr.y(), clr.z());
        cout << hit.getMaterial()->type << endl;
    }*/
}

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc < 3) {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.
    if (argc > 3) spp = atoi(argv[3]);
    // TODO: Main RayCasting Logic
    // First, parse the scene using SceneParser.
    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    // 循 环 屏 幕 空 间 的 像 素
    SceneParser *sceneParser = new SceneParser(inputFile.c_str());
    Camera *camera = sceneParser->getCamera();
    Image image(camera->getWidth(), camera->getHeight());
    puts("BEGIN");
    #pragma omp parallel for schedule(dynamic, 1)
    for(int x = 0; x < camera->getWidth(); ++x){
        fprintf(stdout,"\rRendering: %dspp %8.2f%%",spp,(double)x/camera->getWidth()*100);
        fflush(stdout);
        for (int y = 0; y < camera->getHeight(); ++y) {
            Vector3f finalColor = Vector3f::ZERO;
            for(int sx = 0; sx < 2; ++sx)
                for(int sy = 0; sy < 2; ++sy){
                    unsigned short X[3] = {y + sx, y * x + sy, y * x * y + sx * sy};
                    Vector3f color;
                    for(int s = 0; s < spp; ++s){
                        double r1 = 2 * erand48(X), dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                        double r2 = 2 * erand48(X), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                        Ray camRay = camera->generateRay(Vector2f(x + (sx + .5 + dx) / 2, y + (sy + .5 + dx) / 2), X); //反锯齿
                        color += radiance(camRay, sceneParser, 0, X) / spp;
                        
                    }
                    finalColor += color.clamp() / 4;  
                }
            //Gamma校正
            image.SetPixel(x, y, finalColor.pow(1.0 / 2.2));
        }
    }
    image.SaveBMP(outputFile.c_str());
    return 0;
}

