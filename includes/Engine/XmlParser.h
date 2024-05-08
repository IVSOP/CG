#ifndef CG_XMLPARSER_H
#define CG_XMLPARSER_H

#include "Vertex.h"
#include "common.h"
#include "Engine_Object.h"
#include "tinyxml2.h"
#include "Transformation.h"
#include "RendererObjectInfo.h" // just for the sun
#include "Lights.h"

#include <vector>
#include <map>
#include <functional>
#include <variant>

class XmlParser {
private:
    int windowWidth;
    int windowHeight;

    GLdouble windowFov;
    GLdouble windowZNear;
    GLdouble windowZFar;

    GLdouble cameraXPos;
    GLdouble cameraYPos;
    GLdouble cameraZPos;

    GLdouble cameraXLook;
    GLdouble cameraYLook;
    GLdouble cameraZLook;

    GLdouble cameraXUp;
    GLdouble cameraYUp;
    GLdouble cameraZUp;

    Engine_Object engineObject;

    SceneLights lights;

    Engine_Object parseEngineObject(tinyxml2::XMLElement *group);
    void parseSceneLights(tinyxml2::XMLElement *lights);

    PointLight parsePointLight(tinyxml2::XMLElement *light);

    DirLight parseDirectionalLight(tinyxml2::XMLElement *light);

    SpotLight parseSpotLight(tinyxml2::XMLElement *light);

    std::vector<Vertex> parseVertex(tinyxml2::XMLElement *model);

    Engine_Object_Materials parseEngineObjectMaterials(tinyxml2::XMLElement *model);

    std::vector<std::pair<Engine_Object_Materials, std::vector<Vertex>>> parseModels(tinyxml2::XMLElement *models);

    std::vector<std::variant<Translate, Rotate, Scale>> parseTransformation(tinyxml2::XMLElement *transform);
public:
    XmlParser(){
        this->windowWidth = 512;
        this->windowHeight = 512;

        this->windowFov = 45.0f;
        this->windowZNear = 1.0f;
        this->windowZFar = 1000.0f;

        this->cameraXPos = 0.0f;
        this->cameraYPos = 0.0f;
        this->cameraZPos = 5.0f;

        this->cameraXLook = 0.0f;
        this->cameraYLook = 0.0f;
        this->cameraZLook = -1.0f;

        this->cameraXUp = 0.0f;
        this->cameraYUp = 0.0f;
        this->cameraZUp = -1.0f;

        this->engineObject = Engine_Object();

        this->lights = SceneLights();
    }

    void parseXML(char * xmlFile);

    void setWindowWidth(int width);
    int getWindowWidth();

    void setWindowHeight(int height);
    int getWindowHeight();

    void setWindowFov(GLdouble windowFov);
    GLdouble getWindowFov();

    void setWindowZNear(GLdouble windowZNear);
    GLdouble getWindowZNear();

    void setWindowZFar(GLdouble windowZFar);
    GLdouble getWindowZFar();

    void setCameraXPos(GLdouble cameraXPos);
    GLdouble getCameraXPos();

    void setCameraYPos(GLdouble cameraYPos);
    GLdouble getCameraYPos();

    void setCameraZPos(GLdouble cameraZPos);
    GLdouble getCameraZPos();

    void setCameraXLook(GLdouble cameraXLook);
    GLdouble getCameraXLook();

    void setCameraYLook(GLdouble cameraYLook);
    GLdouble getCameraYLook();

    void setCameraZLook(GLdouble cameraZLook);
    GLdouble getCameraZLook();

    void setCameraXUp(GLdouble cameraXUp);
    GLdouble getCameraXUp();

    void setCameraYUp(GLdouble cameraYUp);
    GLdouble getCameraYUp();

    void setCameraZUp(GLdouble cameraZUp);
    GLdouble getCameraZUp();

    void setLights(SceneLights lights);
    SceneLights getLights();

    std::vector<std::vector<Vertex>> getPoints();
    std::vector<Engine_Object_Info> getObjectInfo(float t);
    std::vector<Engine_Object_Curve> getCurvePoints(float t, int tesselation_level);


	// sun
	// RendererObjectInfo sunInfo;
	// std::vector<SunVertex> sunVertices;
};


#endif //CG_XMLPARSER_H
