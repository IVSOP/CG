#include "Engine_Object.h"

std::vector<std::vector<Vertex>> Engine_Object::getPoints() {
    std::vector<std::vector<Vertex>> ans = std::vector<std::vector<Vertex>>();

    for(auto p : this->points){
        ans.emplace_back(p.second);
    }

    for(Engine_Object& engineObject : this->children_objects){
        std::vector<std::vector<Vertex>> tmp = engineObject.getPoints();

        ans.insert(ans.end(), tmp.begin(), tmp.end());
    }

    return ans;
}

std::vector<Engine_Object_Info> Engine_Object::getObjectInfo(){
    std::vector<Engine_Object_Info> ans = std::vector<Engine_Object_Info>();

    for(auto& p : this->points){
        ans.emplace_back(p.first, this->transformation);
    }

    for(Engine_Object& engineObject : this->children_objects){
        std::vector<Engine_Object_Info> tmp = engineObject.getObjectInfo();

        ans.insert(ans.end(), tmp.begin(), tmp.end());
    }

    return ans;
}
