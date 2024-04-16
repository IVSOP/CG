#ifndef CG_ENGINE_OBJECT_H
#define CG_ENGINE_OBJECT_H

#include <vector>
#include <variant>

#include "Transformation.h"
#include "Vertex.h"


class Engine_Object_Materials {
public:
    glm::vec3 diffuse;
    glm::vec3 ambient;
    glm::vec3 specular;
    glm::vec3 emissive;
    int shininess;

    std::string texture;

    Engine_Object_Materials()
            : diffuse(0), ambient(0), specular(0), emissive(0), shininess(0), texture("") {}

    Engine_Object_Materials(const glm::vec3 &diffuse, const glm::vec3 &ambient, const glm::vec3 &specular, const glm::vec3 &emissive,
                            int shininess, std::string &texture)
            : diffuse(diffuse), ambient(ambient), specular(specular), emissive(emissive), shininess(shininess),
              texture(texture) {}
};

class Engine_Object_Info {
public:
    glm::vec3 diffuse;
    glm::vec3 ambient;
    glm::vec3 specular;
    glm::vec3 emissive;
    int shininess;

    std::string texture;
    Transformation transformation;

    Engine_Object_Info() : diffuse(), ambient(), specular(), emissive(), shininess(), texture(), transformation() {}

    Engine_Object_Info(const Engine_Object_Materials &engineObjectMaterials, const Transformation transformation)
            : diffuse(engineObjectMaterials.diffuse), ambient(engineObjectMaterials.ambient),
              specular(engineObjectMaterials.specular), emissive(engineObjectMaterials.emissive),
              shininess(engineObjectMaterials.shininess), texture(engineObjectMaterials.texture), transformation(transformation) {}
};

class Engine_Object {
public:
    std::vector<std::variant<Translate, Rotate, Scale>> transformations;
    // (Texture, points)
    std::vector<std::pair<Engine_Object_Materials, std::vector<Vertex>>> points;
    std::vector<Engine_Object> children_objects;

    Engine_Object() {
        this->transformations = std::vector<std::variant<Translate, Rotate, Scale>>();
        this->points = std::vector<std::pair<Engine_Object_Materials, std::vector<Vertex>>>();
        this->children_objects = std::vector<Engine_Object>();
    }

    Engine_Object(std::vector<std::variant<Translate, Rotate, Scale>> &transformations,
                  std::vector<std::pair<Engine_Object_Materials, std::vector<Vertex>>> &points,
                  std::vector<Engine_Object> &children_objects) {
        this->transformations = std::vector<std::variant<Translate, Rotate, Scale>>(transformations);
        this->points = std::move(points);
        this->children_objects = std::move(children_objects);
    }

    std::vector<std::vector<Vertex>> getPoints();

    std::vector<Engine_Object_Info> getObjectInfo(float t, Transformation transformation);

    std::vector<std::pair<std::vector<Vertex>, std::vector<Vertex>>> getCurvePoints(int tesselation_level);
};

#endif //CG_ENGINE_OBJECT_H
