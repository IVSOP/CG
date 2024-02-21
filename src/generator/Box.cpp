#include <iostream>

#include "Box.h"
#include "Plane.h"
#include "Consts.h"

std::vector<Vertex> Box::createBoxPoints(int length, int divisions) {
    std::vector<Vertex> plane = Plane::createPlane(length,divisions);
    
    std::vector<Vertex> up = std::vector<Vertex>();
    std::vector<Vertex> down = std::vector<Vertex>();
    std::vector<Vertex> left = std::vector<Vertex>();
    std::vector<Vertex> right = std::vector<Vertex>();
    std::vector<Vertex> front = std::vector<Vertex>();
    std::vector<Vertex> back = std::vector<Vertex>();
    
    glm::vec4 currPoint;
    const float shiftValue = static_cast<float>(length)/2.0f;

    glm::mat4 rotZMatrix = Consts::rotZMatrix(90.0f);
    glm::mat4 rotXMatrix = Consts::rotXMatrix(.0f);

    glm::mat4 shiftUp = Consts::translateMatrix(0.0f, shiftValue, 0.0f);
    glm::mat4 shiftDown = Consts::translateMatrix(0.0f, -shiftValue, 0.0f);
    glm::mat4 shiftRight = rotZMatrix * Consts::translateMatrix(shiftValue, 0.0f, 0.0f);
    glm::mat4 shiftLeft = rotZMatrix * Consts::translateMatrix(-shiftValue, 0.0f, 0.0f);
    glm::mat4 shiftFront = rotXMatrix * Consts::translateMatrix(0.0f, 0.0f, shiftValue);
    glm::mat4 shiftBack = rotXMatrix * Consts::translateMatrix(0.0f, 0.0f, -shiftValue);
    
    //plano base
    for (auto point: plane) {
        currPoint = shiftDown * point.getCoords();

        down.emplace_back(currPoint.x, currPoint.y, currPoint.z);
    }

    //plano cima
    for (auto point: plane) {
        currPoint = shiftUp * point.getCoords();

        up.emplace_back(currPoint.x, currPoint.y, currPoint.z);
    }

    //plano esq

    for (auto point: plane) {
        currPoint = shiftLeft * point.getCoords();

        left.emplace_back(currPoint.x, currPoint.y, currPoint.z);
    }

    // glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //plano dir 
    for (auto point: plane) {
        currPoint = shiftRight * point.getCoords();

        right.emplace_back(currPoint.x, currPoint.y, currPoint.z);
    }

    //plano trás

    for (auto point: plane) {
        currPoint = shiftBack * point.getCoords();

        back.emplace_back(currPoint.x, currPoint.y, currPoint.z);
    }

    //plano frente
    for (auto point: plane) {
        currPoint = shiftFront * point.getCoords();

        front.emplace_back(currPoint.x, currPoint.y, currPoint.z);
    }

    std::vector<Vertex> ans = std::vector<Vertex>();

    ans.insert( ans.end(), up.begin(), up.end() );
    ans.insert( ans.end(), down.begin(), down.end() );
    ans.insert( ans.end(), left.begin(), left.end() );
    ans.insert( ans.end(), right.begin(), right.end() );
    ans.insert( ans.end(), front.begin(), front.end() );
    ans.insert( ans.end(), back.begin(), back.end() );

    for( const Vertex &v: ans){
        std::cout << v << std::endl;
    }

    return ans;
}
