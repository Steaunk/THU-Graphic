#ifndef IMAGE_H
#define IMAGE_H

#include <cassert>
#include <vecmath.h>

// Simple image class
class Image {

public:

    Image(int w, int h) {
        width = w;
        height = h;
        data = new Vector3f[width * height];
    }

    ~Image() {
        delete[] data;
    }

    int Width() const {
        return width;
    }

    int Height() const {
        return height;
    }

    const Vector3f &GetPixel(int x, int y) const {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        return data[y * width + x];
    }

    void SetAllPixels(const Vector3f &color) {
        for (int i = 0; i < width * height; ++i) {
            data[i] = color;
        }
    }

    void addPixel(int x, int y, const Vector3f &color) {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        data[y * width + x] += color;
    }

    Image &operator += (const Image &t){
        int s = width * height;
        for(int i = 0; i < s; ++i){
            data[i] += t.data[i];
        }
        return *this;
    }

    Image &operator /= (const double &k){
        int s = width * height;
        for(int i = 0; i < s; ++i)
            data[i] /= k;
        return *this;
    }

    void clamp(){
        int s = width * height;
        for(int i = 0; i < s; ++i)
            data[i] = data[i].clamp();    
    }

    void SetPixel(int x, int y, const Vector3f &color) {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        data[y * width + x] = color;
    }

    static Image *LoadPPM(const char *filename);

    void SavePPM(const char *filename) const;

    static Image *LoadTGA(const char *filename);

    void SaveTGA(const char *filename) const;

    int SaveBMP(const char *filename);

    void SaveImage(const char *filename);

    Vector3f *data;

protected:

    int width;
    int height;
    
};

/*class ImageBuff : public Image{
public:
    ImageBuff(int w, int h) : Image(w, h){
        cnt = new int [width * height];
    }
    ~ImageBuff(){
        delete cnt;
    }
    void addPixel(int x, int y, const Vector3f &color) {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        data[y * width + x] += color;
        ++cnt[y * width + x]; 
    }
    ImageBuff &operator += (const ImageBuff &t){
        int s = width * height;
        for(int i = 0; i < s; ++i){
            cnt[i] += t.cnt[i];
            data[i] += t.data[i];
        }
        return *this;
        
    }
    void trans(){
        int size = width * height;
        for(int i = 0; i < size; ++i){
            data[i] /= cnt[i];
            cnt[i] = 1;
        }
    }
    int *cnt;
};*/

#endif
