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

std::vector<Engine_Object_Info> Engine_Object::getObjectInfo(float t, Transformation transformation){
    std::vector<Engine_Object_Info> ans = std::vector<Engine_Object_Info>();

    Transform* transform;

    for(auto& p : this->points){

        for(auto& obj : this->transformations){
            if(std::holds_alternative<Translate>(obj)){
                transform = dynamic_cast<Transform*>(&std::get<Translate>(obj));
            } else if(std::holds_alternative<Rotate>(obj)) {
                transform = dynamic_cast<Transform*>(&std::get<Rotate>(obj));
            } else if(std::holds_alternative<Scale>(obj)) {
                transform = dynamic_cast<Transform*>(&std::get<Scale>(obj));
            } else {
                perror("Found unrecognized object inside a engine object transformations.");
            }

            transformation.appendTransformation(*transform, t);
        }

        ans.emplace_back(p.first, transformation);
    }

    for(Engine_Object& engineObject : this->children_objects){
        std::vector<Engine_Object_Info> tmp = engineObject.getObjectInfo(t, transformation);

        ans.insert(ans.end(), tmp.begin(), tmp.end());
    }

    return ans;
}
