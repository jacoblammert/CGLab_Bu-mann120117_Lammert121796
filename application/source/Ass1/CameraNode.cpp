//
// Created by Jacob on 23.10.2021.
//

#include "CameraNode.h"

/**
 *
 * @return if persepective is enabled
 */
bool CameraNode::getPerspective() {
    return isPerspective_;
}

/**
 *
 * @return if camera is enabled
 */
bool CameraNode::getEnabled() {
    return isEnabled_;
}

/**
 *
 * @param enables the camera
 */
void CameraNode::setEnabled(bool enabled) {
    isEnabled_ = enabled;
}
/**
 *
 * @return returns the projection matrix of the camera
 */
glm::fmat4 CameraNode::getProjectionMatrix() {
    return projectionMatrix_;
}
/**
 *
 * @param projectionmatrix sets the projection matrix of the camera
 */
void CameraNode::setProjectionMatrix(glm::fmat4 projectionmatrix) {
    projectionMatrix_ = projectionmatrix;
}
