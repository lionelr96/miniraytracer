#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "geometry.h"
#include <string>

struct Light {
    float intensity;
    std::string type;
    Vec3f position, direction;

    Light();
    // ambient
    Light(std::string type, float i)
        : type(type)
        , intensity(i)
    {
    }
    // point & directional
    Light(std::string t, float i, Vec3f p)
        : type(t)
        , intensity(i)
		, position(p)
		, direction(p)
	{
    }
};

#endif /* __LIGHT_H__ */
