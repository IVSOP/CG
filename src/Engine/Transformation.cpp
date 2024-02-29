#include "Transformation.h"

void Transformation::appendTransformation(glm::mat4& transformation){
    this->transformMatrix *= transformation;
}

void Transformation::appendTransformation(Transformation& transformation){
    this->appendTransformation(transformation.transformMatrix);
}

std::vector<Vertex> Transformation::apply(std::vector<Vertex>& points){
    std::vector<Vertex> ans = std::vector<Vertex>();

    for(uint32_t i = 0; i < points.size(); i++){
        glm::vec4 coords = glm::vec4(points[i].getCoords());

        coords = this->transformMatrix * coords;

        ans.emplace_back(coords.x, coords.y, coords.z);
    }

    return ans;
}