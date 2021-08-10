#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <random>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "omp.h"
//#include "random.hpp"
#include "ppmtree.hpp"

using namespace std;
int spp = 100;
Vector3f random_in_unit_sphere(unsigned short *Xi){
    double alpha = erand48(Xi) * M_PI * 2;
    double beta = erand48(Xi) * M_PI;
    return Vector3f(sin(beta) * cos(alpha), sin(beta) * sin(alpha), cos(beta));
}

void pathTracing(const Ray &ray, SceneParser *sceneParser, int depth, 
    unsigned short *Xi, const int &xx, const int &yy, vector<PPMnode> &data, 
    Vector3f E = Vector3f(1)){
    
    ++depth;

	Hit hit;
	bool isIntersect = sceneParser->getGroup()->intersect(ray, hit, 1e-8, Xi);
	if(isIntersect == false){
        return ;
    }
    Material *m = hit.getMaterial();

    Vector3f x = ray.pointAtParameter(hit.getT());
    Vector3f n = hit.getNormal();
    Vector3f nl = Vector3f::dot(n, ray.getDirection()) < 0 ? n : -n;
    Vector3f f = m->getColor(hit.getUV());
    double p = max(f.x(), max(f.y(), f.z()));
    if(depth > 5){
        if(erand48(Xi) < p) f /= p;
        else return ;
    }
            
    if(m->type == "DIFF"){
        data.push_back(PPMnode(x, nl, f * E, xx, yy));
        return ;
    }

    Ray reflRay(x, (ray.getDirection() - n * 2 * Vector3f::dot(n, ray.getDirection())).normalized());
    if(m->type == "SPEC"){
        //高光反射时的 f 用于模拟不同的金属，金属会立即吸收任何透射光
        if(m->getFuzz() < 1e-5)
            pathTracing(reflRay, sceneParser, depth, Xi, xx, yy, data, f * E);
        else {
            reflRay.setDirection(reflRay.getDirection() + m->getFuzz() * random_in_unit_sphere(Xi));
            pathTracing(reflRay, sceneParser, depth, Xi, xx, yy, data, f * E);
        }
        return ;
    }

    bool into = Vector3f::dot(n, nl) > 0;

    double nc = 1, nt = 1.5; //nt 是折射率
    double nnt = into ? nc / nt : nt / nc, ddn = Vector3f::dot(ray.getDirection(), nl);
    double cos2t = 1 - nnt * nnt * (1 - ddn * ddn);

    if(cos2t < 0){
        pathTracing(reflRay, sceneParser, depth, Xi, xx, yy, data, f * E);
        return ;
    }

    Vector3f tdir = (ray.getDirection() * nnt - n * ((into ? 1 : -1) *
        (ddn * nnt + sqrt(cos2t)))).normalized();

    double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : Vector3f::dot(tdir, n));
    double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);

    if(depth > 2){
        if(erand48(Xi) < P)
            pathTracing(reflRay, sceneParser, depth, Xi, xx, yy, data, f * RP * E);
        else pathTracing(Ray(x, tdir), sceneParser ,depth, Xi, xx, yy, data, f * TP * E);
    }
    else {
        pathTracing(reflRay, sceneParser,depth, Xi, xx, yy, data, f * Re * E);
        pathTracing(Ray(x, tdir), sceneParser ,depth, Xi, xx, yy, data, f * Tr * E);
    }
}
void photonTracing(const Ray &ray, SceneParser *sceneParser, int depth, 
    unsigned short *Xi, PPMTree *ppmTree, const double &radius, Image *ans, 
    Vector3f E = Vector3f(1)){
        
    ++depth;
	Hit hit;
	bool isIntersect = sceneParser->getGroup()->intersect(ray, hit, 1e-8, Xi);
	if(isIntersect == false){
        return ;
    }
    
    Material *m = hit.getMaterial();

    Vector3f x = ray.pointAtParameter(hit.getT());
    Vector3f n = hit.getNormal();
    Vector3f nl = Vector3f::dot(n, ray.getDirection()) < 0 ? n : -n;
    Vector3f f = m->getColor(hit.getUV());
    double p = max(f.x(), max(f.y(), f.z()));
    if(depth > 5){
        if(erand48(Xi) < p) f /= p;
        else return ;
    }
    Ray reflRay(x, (ray.getDirection() - n * 2 * Vector3f::dot(n, ray.getDirection())).normalized());
            
    if(m->type == "DIFF"){
        
        vector<const PPMnode*> result;
        ppmTree->find(result, x, radius);
        //printf("result : %d\n", result.size());
        for(auto i : result){
            if(Vector3f::dot(i->n, nl) > 0){
                ans->addPixel(i->x, i->y, E * i->color * pow((i->p - x).length() / radius, 2));
            }
            //else puts("#");
        }

        
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
        photonTracing(Ray(x, d), sceneParser, depth, Xi, ppmTree, radius, ans, f * E);

        return ;
    }
    else if(m->type == "SPEC"){
        //高光反射时的 f 用于模拟不同的金属，金属会立即吸收任何透射光
        if(m->getFuzz() < 1e-5)
            photonTracing(reflRay, sceneParser, depth, Xi, ppmTree, radius, ans, f * E);
        else {
            reflRay.setDirection(reflRay.getDirection() + m->getFuzz() * random_in_unit_sphere(Xi));
            photonTracing(reflRay, sceneParser, depth, Xi, ppmTree, radius, ans, f * E);
        }
        return ;
    }

    bool into = Vector3f::dot(n, nl) > 0;

    double nc = 1, nt = 1.5; //nt 是折射率
    double nnt = into ? nc / nt : nt / nc, ddn = Vector3f::dot(ray.getDirection(), nl);
    double cos2t = 1 - nnt * nnt * (1 - ddn * ddn);

    if(cos2t < 0){
        photonTracing(reflRay, sceneParser, depth, Xi, ppmTree, radius, ans, f * E);
        return ;
    }

    Vector3f tdir = (ray.getDirection() * nnt - n * ((into ? 1 : -1) *
        (ddn * nnt + sqrt(cos2t)))).normalized();

    double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : Vector3f::dot(tdir, n));
    double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);

    if(depth > 2){
        if(erand48(Xi) < P)
            photonTracing(reflRay, sceneParser, depth, Xi, ppmTree, radius, ans, f * RP * E);
        else photonTracing(Ray(x, tdir), sceneParser ,depth, Xi, ppmTree, radius, ans, f * TP * E);
    }
    else {
        photonTracing(reflRay, sceneParser,depth, Xi, ppmTree, radius, ans, f * Re * E);
        photonTracing(Ray(x, tdir), sceneParser ,depth, Xi, ppmTree, radius, ans, f * Tr * E);
    }
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

    double radius = 1.5;
    double alpha = 0.95;
    double emission = 100000;
    const int round = 20; 
    double energy = 1.0 / log(round);
    const int thread_cnt = omp_get_num_procs();

    vector<PPMnode> data[thread_cnt];
    vector<const PPMnode*> light_data[thread_cnt];

    SceneParser *sceneParser = new SceneParser(inputFile.c_str());
    Camera *camera = sceneParser->getCamera();
    PPMTree *ppmTree;
    printf("thread cnt = %d\n", thread_cnt);
    Image image(camera->getWidth(), camera->getHeight());
    Image *image_t[thread_cnt];
    for(int i = 0; i < round; ++i){
        if(radius < 1e-3) break;
        printf("ROUND #%d\n", i);
        for(int i = 0; i < thread_cnt; ++i){
            data[i].clear();
            light_data[i].clear();
        }
        const int tmp = 4;
        for(int i = 0; i < thread_cnt; ++i) image_t[i] = new Image(camera->getWidth(), camera->getHeight());
        #pragma omp parallel for schedule(dynamic, 1)
        for(int x = 0; x < camera->getWidth(); ++x){
            const int thread_num = omp_get_thread_num();
            fprintf(stdout,"\rRendering: %8.2f%%",spp,(double)x/camera->getWidth()*100);
            for (int y = 0; y < camera->getHeight(); ++y) {
                for(int sx = 0; sx < 2; ++sx)
                    for(int sy = 0; sy < 2; ++sy){
                        unsigned short X[3] = {y + sx + i * i, y * x + sy, y * x * i + sx * sy};
                        for(int u = 0; u < tmp; ++u){
                        double r1 = 2 * erand48(X), dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                        double r2 = 2 * erand48(X), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                        //printf("%lf %lf\n", r1, r2);
                        Ray camRay = camera->generateRay(Vector2f(x + (sx + .5 + dx) / 2, y + (sy + .5 + dx) / 2), X); //反锯齿
                        pathTracing(camRay, sceneParser, 0, X, x, y, data[thread_num], Vector3f(energy));
                        }
                    }
            }
        }

        vector<PPMnode> sum;
        for(int i = 0; i < thread_cnt; ++i) sum.insert(sum.end(), data[i].begin(), data[i].end());
        printf("\nPPMTree node : %d\n", sum.size());
        ppmTree = new PPMTree(sum);
        int li_num = sceneParser->getNumLights();
        printf("Light num : %d\n", li_num);
        double total_light = 0;
        for(int li = 0; li < li_num; ++li) total_light += sceneParser->getLight(li)->getColor().length();
        for(int li = 0; li < li_num; ++li){
            Light *light = sceneParser->getLight(li);
            int spp_li = spp / total_light * light->getColor().length() + 1;
            #pragma omp parallel for schedule(dynamic, 1)
            for(int j = 0; j < spp_li; ++j){
                fprintf(stdout, "\r# %d Lighting: %dspp %8.2f%%", li, spp_li, (double)j/spp_li *100);
                int thread_num = omp_get_thread_num();
                unsigned short X[3] = {li + i * j, i,  j};
                photonTracing(light->randomEmit(X), sceneParser, 0, X, ppmTree, radius, image_t[thread_num], emission * light->getColor() / spp);
            }
            puts("");
        }
        for(int i = 0; i < thread_cnt; ++i) image += (*image_t[i] /= 4 * tmp * (i + 1));
        
        delete ppmTree;
        for(int i = 0; i < thread_cnt; ++i){
            delete image_t[i];
        }
        printf("%lf %lf\n", emission, radius);  
        emission /= alpha;
        radius *= alpha;
    }
    //image /= round;
    image.clamp();
            /*for(int x = 0; x < camera->getWidth(); ++x)for(int y = 0; y < camera->getHeight(); ++y){
                printf("%d %d ", x, y); image.GetPixel(x, y).print();
            }*/
    for(int i = 0; i < camera->getWidth(); ++i)
        for(int j = 0; j < camera->getHeight(); ++j)
            image.SetPixel(i, j, image.GetPixel(i, j).pow(1. / 2.2));
    image.SaveBMP(outputFile.c_str());
    return 0;
}

