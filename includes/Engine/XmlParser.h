#ifndef CG_XMLPARSER_H
#define CG_XMLPARSER_H

#include "Vertex.h"
#include "common.h"
#include "Engine_Object.h"
#include "tinyxml2.h"

#include <vector>

class XmlParser {
private:
    int windowWidth = 512;
    int windowHeight = 512;

    GLdouble windowFov = 45.0f;
    GLdouble windowZNear = 1.0f;
    GLdouble windowZFar = 1000.0f;

    GLdouble cameraXPos = 0.0f;
    GLdouble cameraYPos = 0.0f;
    GLdouble cameraZPos = 5.0f;

    GLdouble cameraXLook = 0.0f;
    GLdouble cameraYLook = 0.0f;
    GLdouble cameraZLook = -1.0f;

    GLdouble cameraXUp = 0.0f;
    GLdouble cameraYUp = 0.0f;
    GLdouble cameraZUp = -1.0f;

    // Engine_Object engineObject;
    std::vector<Vertex> points = std::vector<Vertex>();

    Engine_Object parseEngineObject(tinyxml2::XMLElement *group, std::vector<Transformation>& transformations);

    std::vector<Vertex> parseVertex(tinyxml2::XMLElement *model);
public:
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
    std::vector<Vertex> getPoints();
};


#endif //CG_XMLPARSER_H
