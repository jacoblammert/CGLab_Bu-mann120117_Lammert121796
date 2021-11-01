//
// Created by Jacob on 23.10.2021.
//

#include "SceneGraph.h"

#include <utility>

/**
 * Creates Scene Graph, sets root node
 */
SceneGraph::SceneGraph() {
    setRoot(std::make_shared<Node>(Node()));
}

/**
 *
 * @return name of the scene
 */
std::string SceneGraph::getName() {
    return name_;
}

/**
 * sets the name of the scene
 * @param name
 */
void SceneGraph::setName(std::string name) {
    this->name_ = std::move(name);
}

/**
 * sets the root object of a scene
 * @param root
 */
void SceneGraph::setRoot(std::shared_ptr<Node> root) {
    this->root_ = std::move(root);
}

/**
 * returns the root object of a scene
 * @return
 */
std::shared_ptr<Node> SceneGraph::getRoot() const {
    return root_;
}

/**
 * returns the path of the root node
 * @return
 */
std::string SceneGraph::printGraph() {
    return root_->getPath();
}

SceneGraph::~SceneGraph() {

}

