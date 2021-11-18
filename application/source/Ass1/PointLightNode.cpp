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
std::vector<float> PointLightNode::getColor() {
    return lightColor_;
}

/**
 * set the values of the color for the light
 * @param color
 */
void PointLightNode::setColor(std::vector<float> color) {
    lightColor_ = std::move(color);
}

/**
 * getter for the brightness
 * @return
 */
float PointLightNode::getIntensity() {
    return lightIntensity_;
}

/**
 * getter for the brightness
 * @param brightness
 */
void PointLightNode::setIntensity(float intensity) {
    lightIntensity_ = intensity;
}
