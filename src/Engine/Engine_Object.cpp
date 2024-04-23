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

    Transformation tmp;

    for(auto& p : this->points){

        tmp = Transformation(transformation);

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

            tmp.appendTransformation(*transform, t);
        }

        ans.emplace_back(p.first, tmp);
    }

    for(Engine_Object& engineObject : this->children_objects){
        std::vector<Engine_Object_Info> tmp = engineObject.getObjectInfo(t, transformation);

        ans.insert(ans.end(), tmp.begin(), tmp.end());
    }

    return ans;
}

std::vector<std::pair<std::vector<Vertex>, std::vector<Vertex>>> Engine_Object::getCurvePoints(int tesselation_level){
    std::vector<std::pair<std::vector<Vertex>, std::vector<Vertex>>> ans = std::vector<std::pair<std::vector<Vertex>, std::vector<Vertex>>>();

    Translate* translate;
    bool found;

    for(auto& p : this->points){
        found = false;

        // Assuming there is only one translate per group, since it doesn't make sense to have several curves to the same objects
        for(auto& obj : this->transformations){
            if(std::holds_alternative<Translate>(obj)){
                translate = &std::get<Translate>(obj);

                ans.emplace_back(translate->getCurvePoints(tesselation_level));
                found = true;
                break;
            }
        }

        if(!found)
            ans.emplace_back(std::vector<Vertex>(), std::vector<Vertex>()); //preencher sem pontos e derivadas no caso de não haver curva a ser desenhada
    }

    for(Engine_Object& engineObject : this->children_objects){
        std::vector<std::pair<std::vector<Vertex>, std::vector<Vertex>>> tmp = engineObject.getCurvePoints(tesselation_level);

        ans.insert(ans.end(), tmp.begin(), tmp.end());
    }

    return ans;
}
