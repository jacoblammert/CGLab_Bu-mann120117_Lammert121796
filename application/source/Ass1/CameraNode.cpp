//
// Created by Jacob on 23.10.2021.
//

#include "CameraNode.h"

bool CameraNode::getPerspective() {
    return isPerspective;
}

bool CameraNode::getEnabled() {
    return isEnabled;
}

bool CameraNode::setEnabled(bool enabled) {
    return isEnabled;
}

glm::fmat4 CameraNode::getProjectionMatrix() {
    return projectionMatrix;
}

void CameraNode::setProjectionMatrix(glm::fmat4 projectionmatrix) {
    projectionMatrix = projectionmatrix;
}
