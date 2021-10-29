//
// Created by Jacob on 23.10.2021.
//

#include "GeometryNode.h"

model_object GeometryNode::getGeometry() {
    return geometry;
}

void GeometryNode::setGeometry(model_object model) {
    geometry = model;
}
