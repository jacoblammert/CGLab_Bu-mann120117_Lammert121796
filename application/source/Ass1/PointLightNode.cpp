//
// Created by Jacob on 03.11.2021.
//

#include "PointLightNode.h"

#include <utility>

/**
 *
 * @return color as vector with 3 values
 * glm:vector?
 */
std::vector<int> PointLightNode::getColor() {
    return color_;
}

/**
 * set the values of the color for the light
 * @param color
 */
void PointLightNode::setColor(std::vector<int> color) {
    color_ = std::move(color);
}

/**
 * getter for the brightness
 * @return
 */
float PointLightNode::getBrightness() {
    return brightness_;
}

/**
 * getter for the brightness
 * @param brightness
 */
void PointLightNode::setBrightness(float brightness) {
    brightness_ = brightness;
}
