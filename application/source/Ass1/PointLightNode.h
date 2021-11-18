//
// Created by Jacob on 03.11.2021.
//

#ifndef OPENGLFRAMEWORK_POINTLIGHTNODE_H
#define OPENGLFRAMEWORK_POINTLIGHTNODE_H


#include "Node.h"

class PointLightNode : public Node {

public:
    std::vector<float> getColor();
    float getIntensity();

    void setIntensity(float intensity);
    void setColor(std::vector<float> color);

private:
    std::vector<float> lightColor_ = {}; // private variable for the color
    float lightIntensity_ = 1;
};


#endif //OPENGLFRAMEWORK_POINTLIGHTNODE_H
