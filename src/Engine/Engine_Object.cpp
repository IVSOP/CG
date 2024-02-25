#include "Engine_Object.h"

std::vector<Vertex> Engine_Object::getPoints() {
    std::vector<Vertex> ans = this->transformation.apply(this->points);

    for(Engine_Object& engineObject : this->children_objects){
        std::vector<Vertex> tmp = engineObject.getPoints();

        ans.insert(ans.end(), tmp.begin(), tmp.end());
    }

    return ans;
}