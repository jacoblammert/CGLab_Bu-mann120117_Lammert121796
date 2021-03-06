//
// Created by Jacob on 23.10.2021.
//

#ifndef OPENGLFRAMEWORK_CAMERANODE_H
#define OPENGLFRAMEWORK_CAMERANODE_H


#include "Node.h"

class CameraNode : public Node{
public:
    bool getPerspective();
    bool getEnabled();

    glm::fmat4 getProjectionMatrix();

    void setEnabled(bool enabled);
    void setProjectionMatrix(glm::fmat4 projectionmatrix);

private:
    bool isPerspective_;
    bool isEnabled_;

    glm::fmat4 projectionMatrix_;

};


#endif //OPENGLFRAMEWORK_CAMERANODE_H
