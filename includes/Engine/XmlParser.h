#ifndef CG_XMLPARSER_H
#define CG_XMLPARSER_H

#include "Vertex.h"
#include "common.h"
#include "Engine_Object.h"
#include "tinyxml2.h"

#include <vector>
#include <map>
#include <functional>

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
    std::vector<Vertex> points;

    Engine_Object parseEngineObject(tinyxml2::XMLElement *group, Transformation transformations);

    std::vector<Vertex> parseVertex(tinyxml2::XMLElement *model);

    Engine_Object_Materials parseEngineObjectMaterials(tinyxml2::XMLElement *model);

    std::vector<std::pair<Engine_Object_Materials, std::vector<Vertex>>> parseModels(tinyxml2::XMLElement *models);

    Transformation parseTransformation(tinyxml2::XMLElement *transform);
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
        this->points = std::vector<Vertex>();
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

    void setPoints(std::vector<Vertex>& points);
    std::vector<std::vector<Vertex>> getPoints();
    std::vector<Engine_Object_Info> getMaterials();
};


#endif //CG_XMLPARSER_H
