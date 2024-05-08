#ifndef CG_LIGHTS_H
#define CG_LIGHTS_H

#include <vector>

#include "glm/glm.hpp"

struct PointLight {
    glm::vec3 position;

    GLfloat constant;
    GLfloat linear;
    GLfloat quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // need one extra float for 16. read Material.h
    GLfloat padding_1;

    PointLight(): position(), constant(), linear(), quadratic(), ambient(), diffuse(), specular(), padding_1(){};

    PointLight(glm::vec3& position, GLfloat constant, GLfloat linear, GLfloat quadratic, glm::vec3& ambient,
        glm::vec3& diffuse, glm::vec3& specular): position(position), constant(constant), linear(linear),
        quadratic(quadratic), ambient(ambient), diffuse(diffuse), specular(specular), padding_1(){};

    PointLight(glm::vec3& position, GLfloat constant, GLfloat linear, GLfloat quadratic, glm::vec3& ambient,
               glm::vec3& diffuse, glm::vec3& specular, GLfloat padding_1):
               position(position), constant(constant), linear(linear),
               quadratic(quadratic), ambient(ambient), diffuse(diffuse), specular(specular), padding_1(padding_1){};

    PointLight(glm::vec3& position): position(position), constant(), linear(),
                                     quadratic(), ambient(), diffuse(), specular(), padding_1(){};
};
static_assert(sizeof(PointLight) == 4 * sizeof(glm::vec4), "Error: PointLight has unexpected size");

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    DirLight(): direction(), ambient(), diffuse(), specular(){};

    DirLight(glm::vec3& direction, glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular):
    direction(direction), ambient(ambient), diffuse(diffuse), specular(specular){};

    DirLight(glm::vec3& direction):
            direction(direction), ambient(), diffuse(), specular(){};
};
static_assert(sizeof(DirLight) == 3 * sizeof(glm::vec4), "Error: DirLight has unexpected size");

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    GLfloat cutOff;
    GLfloat outerCutOff;

    GLfloat constant;
    GLfloat linear;
    GLfloat quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    SpotLight(): position(), direction(), cutOff(), outerCutOff(), constant(), linear(), quadratic(),
    ambient(), diffuse(), specular(){};

    SpotLight(glm::vec3& position, glm::vec3& direction, GLfloat cutOff, GLfloat outerCutOff, GLfloat constant
              , GLfloat linear, GLfloat quadratic, glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular):
              position(position) , direction(direction),
              cutOff(cutOff), outerCutOff(outerCutOff), constant(constant), linear(linear),
              quadratic(quadratic), ambient(ambient), diffuse(diffuse), specular(specular){};

    SpotLight(glm::vec3& position, glm::vec3& direction, GLfloat cutOff): position(position), direction(direction),
              cutOff(cutOff), constant(), linear(), quadratic(), ambient(), diffuse(), specular(){};
};
static_assert(sizeof(SpotLight) == 5 * sizeof(glm::vec4), "Error: SpotLight has unexpected size");

class SceneLights{
public:
    std::vector<PointLight> pointLights;
    std::vector<DirLight> dirLights;
    std::vector<SpotLight> spotLights;

    SceneLights(): pointLights(), dirLights(), spotLights(){}

    SceneLights(std::vector<PointLight>& pointLights, std::vector<DirLight>& dirLights,
                std::vector<SpotLight>& spotLights): pointLights(pointLights), dirLights(dirLights),
                spotLights(spotLights){}
};

#endif //CG_LIGHTS_H
