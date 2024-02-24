#include "XmlParser.h"
#include "tinyxml2.h"

void XmlParser::setWindowWidth(int width){

}

int XmlParser::getWindowWidth(){
        return this->windowWidth;
}


void XmlParser::setWindowHeight(int height){

}

int XmlParser::getWindowHeight(){
    return this->windowHeight;
}


void XmlParser::setWindowFov(GLdouble windowFov){

}

GLdouble XmlParser::getWindowFov(){
    return this->windowFov;
}


void XmlParser::setWindowZNear(GLdouble windowZNear){

}

GLdouble XmlParser::getWindowZNear(){
    return this->windowZNear;
}


void XmlParser::setWindowZFar(GLdouble windowZFar){

}

GLdouble XmlParser::getWindowZFar(){
    return this->windowZFar;
}


void XmlParser::setCameraXPos(GLdouble cameraXPos){

}

GLdouble XmlParser::getCameraXPos(){
    return this->cameraXPos;
}


void XmlParser::setCameraYPos(GLdouble cameraYPos){

}

GLdouble XmlParser::getCameraYPos(){
    return this->cameraYPos;
}


void XmlParser::setCameraZPos(GLdouble cameraZPos){

}

GLdouble XmlParser::getCameraZPos(){
    return this->cameraZPos;
}


void XmlParser::setCameraXLook(GLdouble cameraXLook){

}

GLdouble XmlParser::getCameraXLook(){
    return this->cameraXLook;
}


void XmlParser::setCameraYLook(GLdouble cameraYLook){

}

GLdouble XmlParser::getCameraYLook(){
    return this->cameraYLook;
}


void XmlParser::setCameraZLook(GLdouble cameraZLook){

}

GLdouble XmlParser::getCameraZLook(){
    return this->cameraZLook;
}


void XmlParser::setCameraXUp(GLdouble cameraXUp){

}

GLdouble XmlParser::getCameraXUp(){
    return this->cameraXUp;
}


void XmlParser::setCameraYUp(GLdouble cameraYUp){

}

GLdouble XmlParser::getCameraYUp(){
    return this->cameraYUp;
}


void XmlParser::setCameraZUp(GLdouble cameraZUp){

}

GLdouble XmlParser::getCameraZUp(){
    return this->cameraZUp;
}

void XmlParser::setPoints(std::vector<Vertex>& points){
    this->points = std::vector<Vertex>(points);
}

std::vector<Vertex> XmlParser::getPoints(){
    return this->points;
}

std::vector<Vertex> XmlParser::parseVertex(tinyxml2::XMLElement *model){
    FILE* f = std::fopen(model->Attribute("file"), "rb");

    if(f == nullptr){

        return {};
    }

    int size;

    std::fread(&size, sizeof(int), 1, f);

    Vertex* pts = new Vertex[size];
    fread(pts, sizeof(Vertex), size, f);

    std::vector<Vertex> points = std::vector<Vertex>(pts, pts + size);
    delete[] pts;

    return points;
}

Engine_Object XmlParser::parseEngineObject(tinyxml2::XMLElement *group, std::vector<Transformation>& transformations){
    std::vector<Vertex> points = std::vector<Vertex>();
    std::vector<Engine_Object> child_objs = std::vector<Engine_Object>();

    for (tinyxml2::XMLNode* node = group->FirstChild(); node != nullptr; node = node->NextSibling()){
        std::cout << node << std::endl;
    }

    return Engine_Object(transformations, points, child_objs);
}

void XmlParser::parseXML(char * xmlFile) {
    tinyxml2::XMLDocument xmlDoc;

    /*
     * There has been an error loading the given file
     * Either file doesn't exist (wrong path) or there has been another error
     */
    if (xmlDoc.LoadFile(xmlFile) != tinyxml2::XML_SUCCESS) {
        perror("There has been an error reading the given file.");

        return;
    }

    tinyxml2::XMLElement *root = xmlDoc.RootElement();

    /* The given file doesn't have any root element */
    if (root == nullptr) {
        perror("The given XML file has no root element to read.");

        return;
    }

    /* Acquire all needed elements from the XML file */
    tinyxml2::XMLElement *window = root->FirstChildElement("window");
    tinyxml2::XMLElement *camera = root->FirstChildElement("camera");
    tinyxml2::XMLElement *group = root->FirstChildElement("group");

    /* Acquire all needed sub-elements from the camera element */
    tinyxml2::XMLElement *position = camera->FirstChildElement("position");
    tinyxml2::XMLElement *lookAt = camera->FirstChildElement("lookAt");
    tinyxml2::XMLElement *up = camera->FirstChildElement("up");
    tinyxml2::XMLElement *projection = camera->FirstChildElement("projection");


    /* Acquire all needed values to draw the window */
    this->windowWidth = window->IntAttribute("width");
    this->windowHeight = window->IntAttribute("height");

    this->windowFov = projection->DoubleAttribute("fov");
    this->windowZNear = projection->DoubleAttribute("near");
    this->windowZFar = projection->DoubleAttribute("far");

    this->cameraXPos = position->DoubleAttribute("x");
    this->cameraYPos = position->DoubleAttribute("y");
    this->cameraZPos = position->DoubleAttribute("z");

    this->cameraXLook = lookAt->DoubleAttribute("x");
    this->cameraYLook = lookAt->DoubleAttribute("y");
    this->cameraZLook = lookAt->DoubleAttribute("z");

    this->cameraXUp = up->DoubleAttribute("x");
    this->cameraYUp = up->DoubleAttribute("y");
    this->cameraZUp = up->DoubleAttribute("z");

    std::vector<Transformation> transformations = std::vector<Transformation>();
    // this->engineObject = parseEngineObject(group, transformations);

    tinyxml2::XMLElement *models = group->FirstChildElement("models");

    for (tinyxml2::XMLElement* model = models->FirstChildElement("model"); model != nullptr; model = model->NextSiblingElement("model")){
        FILE* f = std::fopen(model->Attribute("file"), "rb");

        if(f == nullptr){

            continue;
        }

        int size;

        std::fread(&size, sizeof(int), 1, f);

        Vertex* pts = new Vertex[size];
        fread(pts, sizeof(Vertex), size, f);

        this->points = std::vector<Vertex>(pts, pts + size);
        delete[] pts;
    }
}