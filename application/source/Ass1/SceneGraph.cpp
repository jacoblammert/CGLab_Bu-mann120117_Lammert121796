//
// Created by Jacob on 23.10.2021.
//

#include "SceneGraph.h"
#include "Node.h"

#include <utility>

SceneGraph::SceneGraph(){}

std::string SceneGraph::getName(){
    return name;
}

void SceneGraph::setName(std::string name){
    this->name = std::move(name);
}

void SceneGraph::setRoot(std::shared_ptr<Node> root){
    this->root = root;
}

std::shared_ptr<Node> SceneGraph::getRoot(){
    return root;
}