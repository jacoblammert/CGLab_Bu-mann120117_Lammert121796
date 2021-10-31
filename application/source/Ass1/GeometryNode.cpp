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
