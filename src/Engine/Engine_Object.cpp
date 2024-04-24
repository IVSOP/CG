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

    for(auto& obj : this->transformations) {
        if (std::holds_alternative<Translate>(obj)) {
            Translate translate = std::get<Translate>(obj);
            glm::mat4 translateMatrix = translate.getMatrix(t);
            transformation.appendTransformation(translateMatrix);
        } else if (std::holds_alternative<Rotate>(obj)) {
            Rotate rotate = std::get<Rotate>(obj);
            glm::mat4 rotateMatrix = rotate.getMatrix(t);
            transformation.appendTransformation(rotateMatrix);
        } else if (std::holds_alternative<Scale>(obj)) {
            Scale scale = std::get<Scale>(obj);
            glm::mat4 scaleMatrix = scale.getMatrix(t);
            transformation.appendTransformation(scaleMatrix);
        } else {
            perror("Found unrecognized object inside a engine object transformations.");
            return {};
        }
    }

    for(auto& p : this->points){
        ans.emplace_back(p.first, transformation);
    }

    for(Engine_Object& engineObject : this->children_objects){
        std::vector<Engine_Object_Info> tmp = engineObject.getObjectInfo(t, transformation);

        ans.insert(ans.end(), tmp.begin(), tmp.end());
    }

    return ans;
}


std::vector<Engine_Object_Curve> Engine_Object::getCurvePoints(float t, int tesselation_level, Transformation transformation){
    std::vector<Engine_Object_Curve> ans = std::vector<Engine_Object_Curve>();

    Translate* translate;
    bool found;

    for(auto& p : this->points){
        found = false;

        // Assuming there is only one translate per group, since it doesn't make sense to have several curves to the same objects
        for(auto& obj : this->transformations){
            if(std::holds_alternative<Translate>(obj)){
                translate = &std::get<Translate>(obj);

                std::pair<std::vector<Vertex>, std::vector<Vertex>> tmp = translate->getCurvePoints(tesselation_level);
                Engine_Object_Curve engineCurve = Engine_Object_Curve(tmp.first, tmp.second, transformation);

                ans.emplace_back(engineCurve);
                found = true;
                break;
            }
        }

        if(!found)
            ans.emplace_back(Engine_Object_Curve()); //preencher sem pontos e derivadas no caso de não haver curva a ser desenhada
    }

    Transformation newTransformation = Transformation(transformation);

    for(auto& obj : this->transformations) {
        if (std::holds_alternative<Translate>(obj)) {
            Translate translate = std::get<Translate>(obj);
            glm::mat4 translateMatrix = translate.getMatrix(t);
            newTransformation.appendTransformation(translateMatrix);
        } else if (std::holds_alternative<Rotate>(obj)) {
            Rotate rotate = std::get<Rotate>(obj);
            glm::mat4 rotateMatrix = rotate.getMatrix(t);
            newTransformation.appendTransformation(rotateMatrix);
        } else if (std::holds_alternative<Scale>(obj)) {
            Scale scale = std::get<Scale>(obj);
            glm::mat4 scaleMatrix = scale.getMatrix(t);
            newTransformation.appendTransformation(scaleMatrix);
        } else {
            perror("Found unrecognized object inside a engine object transformations.");
            return {};
        }
    }

    for(Engine_Object& engineObject : this->children_objects){
        std::vector<Engine_Object_Curve> tmp = engineObject.getCurvePoints(t, tesselation_level, newTransformation);

        ans.insert(ans.end(), tmp.begin(), tmp.end());
    }

    return ans;
}
