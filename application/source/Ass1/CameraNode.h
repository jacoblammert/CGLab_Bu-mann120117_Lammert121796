//
// Created by Jacob on 23.10.2021.
//

#ifndef OPENGLFRAMEWORK_CAMERANODE_H
#define OPENGLFRAMEWORK_CAMERANODE_H


#include "Node.h"

class CameraNode : Node{
public:
    bool getPerspective();
    bool getEnabled();
    bool setEnabled(bool enabled);
    glm::fmat4 getProjectionMatrix();
    void setProjectionMatrix(glm::fmat4 projectionmatrix);

private:
    bool isPerspective;
    bool isEnabled;
    glm::fmat4 projectionMatrix;

};


#endif //OPENGLFRAMEWORK_CAMERANODE_H
