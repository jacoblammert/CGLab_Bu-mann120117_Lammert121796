//
// Created by Jacob on 23.10.2021.
//

#include "GeometryNode.h"

/**
 *
 * @returns the geometry of a planet
 */
model_object GeometryNode::getGeometry() {
    return geometry_;
}
/**
 *  (has to be set each frame due to conversion loss)
 * @param model - model_object of the planet (sphere)
 */
void GeometryNode::setGeometry(model_object model) {
    geometry_ = model;
}
/**
 *
 * @returns the trail of a planet
 */
model_object GeometryNode::getTrail() {
    return trail_;
}
/**
 * @param model - trail of the planet (ring)
 */
void GeometryNode::setTrail(model_object model) {
    trail_ = model;
}

void GeometryNode::setTexture(texture_object texture) {
    texture_ = texture;
}

texture_object GeometryNode::getTexture() {
    return texture_;
}
