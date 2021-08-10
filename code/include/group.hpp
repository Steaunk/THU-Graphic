#ifndef GROUP_H
#define GROUP_H

#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>

// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D
{

public:
    Group() = delete;

    explicit Group(int num_objects) : group_size(num_objects)
    {
        v = new Object3D *[num_objects];
    }

    ~Group() override
    {
        delete[] v;
    }

    bool intersect(const Ray &r, Hit &h, double tmin, unsigned short *X) override
    {
        bool result = false;
        for (int i = 0; i < group_size; ++i)
        {
            result |= v[i]->intersect(r, h, tmin, X);
        }
        return result;
    }

    void addObject(int index, Object3D *obj)
    {
        assert(index < group_size);
        v[index] = obj;
    }

    int getGroupSize()
    {
        return group_size;
    }
    Object3D **v;
private:
    const int group_size;

};

#endif
