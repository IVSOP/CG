#include "XmlParser.h"
#include "tinyxml2.h"
#include "Transformation.h"
#include "Rotate.h"
#include "Translate.h"
#include "Scale.h"

void XmlParser::setWindowWidth(int width){
        this->windowWidth = width;
}

int XmlParser::getWindowWidth(){
        return this->windowWidth;
}


void XmlParser::setWindowHeight(int height){
    this->windowHeight = height;
}

int XmlParser::getWindowHeight(){
    return this->windowHeight;
}


void XmlParser::setWindowFov(GLdouble windowFov){
    this->windowFov = windowFov;
}

GLdouble XmlParser::getWindowFov(){
    return this->windowFov;
}


void XmlParser::setWindowZNear(GLdouble windowZNear){
    this->windowZNear = windowZNear;
}

GLdouble XmlParser::getWindowZNear(){
    return this->windowZNear;
}


void XmlParser::setWindowZFar(GLdouble windowZFar){
    this->windowZFar = windowZFar;
}

GLdouble XmlParser::getWindowZFar(){
    return this->windowZFar;
}


void XmlParser::setCameraXPos(GLdouble cameraXPos){
    this->cameraXPos = cameraXPos;
}

GLdouble XmlParser::getCameraXPos(){
    return this->cameraXPos;
}


void XmlParser::setCameraYPos(GLdouble cameraYPos){
    this->cameraYPos = cameraYPos;
}

GLdouble XmlParser::getCameraYPos(){
    return this->cameraYPos;
}


void XmlParser::setCameraZPos(GLdouble cameraZPos){
    this->cameraZPos = cameraZPos;
}

GLdouble XmlParser::getCameraZPos(){
    return this->cameraZPos;
}


void XmlParser::setCameraXLook(GLdouble cameraXLook){
    this->cameraXLook = cameraXLook;
}

GLdouble XmlParser::getCameraXLook(){
    return this->cameraXLook;
}


void XmlParser::setCameraYLook(GLdouble cameraYLook){
    this->cameraYLook = cameraYLook;
}

GLdouble XmlParser::getCameraYLook(){
    return this->cameraYLook;
}


void XmlParser::setCameraZLook(GLdouble cameraZLook){
    this->cameraZLook = cameraZLook;
}

GLdouble XmlParser::getCameraZLook(){
    return this->cameraZLook;
}


void XmlParser::setCameraXUp(GLdouble cameraXUp){
    this->cameraXUp = cameraXUp;
}

GLdouble XmlParser::getCameraXUp(){
    return this->cameraXUp;
}


void XmlParser::setCameraYUp(GLdouble cameraYUp){
    this->cameraYUp = cameraYUp;
}

GLdouble XmlParser::getCameraYUp(){
    return this->cameraYUp;
}


void XmlParser::setCameraZUp(GLdouble cameraZUp){
    this->cameraZUp = cameraZUp;
}

GLdouble XmlParser::getCameraZUp(){
    return this->cameraZUp;
}

void XmlParser::setPoints(std::vector<Vertex>& points){
    this->points = std::vector<Vertex>(points);
}

std::vector<Vertex> XmlParser::getPoints(){
    return this->engineObject.getPoints();

    /*std::vector<Vertex> tmp = this->engineObject.getPoints();

    std::cout << this->points.size() << "\t" << tmp.size() << std::endl;

    for(uint32_t i = 0; i < this->points.size(); i++){
        Vertex v1 = this->points[i];
        Vertex v2 = tmp[i];

        if(v1.getCoords() != v2.getCoords()){
            std::cout << v1 << "\t" << v2 << std::endl;
        }
    }

    return this->points;*/
}

std::vector<Vertex> XmlParser::parseModels(tinyxml2::XMLElement *models){
    std::vector<Vertex> ans = std::vector<Vertex>();

    for (tinyxml2::XMLElement* node = models->FirstChildElement(); node != nullptr; node = node->NextSiblingElement()){
        std::string name = std::string(node->Name());

        if(name == "model"){
            std::vector<Vertex> tmp = this->parseVertex(node);

            ans.insert(ans.end(), tmp.begin(), tmp.end());

            continue;
        }

        perror("Found unrecognized tag inside a models tag.");
        std::cout << name << std::endl;
    }

    return ans;
}

// TODO Acabar
Transformation XmlParser::parseTransformation(tinyxml2::XMLElement *transform){
    Transformation transformation = Transformation();

    for (tinyxml2::XMLElement* node = transform->FirstChildElement(); node != nullptr; node = node->NextSiblingElement()){
        std::string name = std::string(node->Name());

        if(name == "rotate"){
            Rotate r = Rotate(node->DoubleAttribute("angle"), node->FloatAttribute("x"), node->FloatAttribute("y"), node->FloatAttribute("z"));

            Transformation t = Transformation(r);

            transformation.appendTransformation(t);

            continue;
        }

        if(name == "translate"){
            Translate translate = Translate(node->FloatAttribute("x"), node->FloatAttribute("y"), node->FloatAttribute("z"));

            Transformation t = Transformation(translate);

            transformation.appendTransformation(t);

            continue;
        }

        if(name == "scale"){
            Scale s = Scale(node->FloatAttribute("x"), node->FloatAttribute("y"), node->FloatAttribute("z"));

            Transformation t = Transformation(s);

            transformation.appendTransformation(t);

            continue;
        }

        perror("Found unrecognized tag inside a models tag.");
        std::cout << name << std::endl;
    }

    return transformation;
}

std::vector<Vertex> XmlParser::parseVertex(tinyxml2::XMLElement *model){
    FILE* f = std::fopen(model->Attribute("file"), "rb");

    if(f == nullptr){
        perror("There is no such file with the given name.");
        std::cout << model->Attribute("file") << std::endl;

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

Engine_Object XmlParser::parseEngineObject(tinyxml2::XMLElement *group, Transformation transformations){
    std::vector<Vertex> obj_points = std::vector<Vertex>();
    std::vector<Engine_Object> child_objs = std::vector<Engine_Object>();

    for (tinyxml2::XMLElement* node = group->FirstChildElement(); node != nullptr; node = node->NextSiblingElement()){
        std::string name = std::string(node->Name());

        // std::cout << name << std::endl;

        if(name == "transform"){
            Transformation t = parseTransformation(node);
            transformations.appendTransformation(t);

            continue;
        }

        if(name == "models"){
            std::vector<Vertex> tmp_points = parseModels(node);
            obj_points.insert(obj_points.end(), tmp_points.begin(), tmp_points.end());

            continue;
        }

        if(name == "group"){
            Engine_Object child = parseEngineObject(node, Transformation(transformations));
            child_objs.emplace_back(child.transformation, child.points, child.children_objects);

            continue;
        }

        perror("Found unrecognized tag inside a group tag.");
        std::cout << name << std::endl;
    }

    return Engine_Object(transformations, obj_points, child_objs);
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

    Transformation transformations = Transformation();
    this->engineObject = parseEngineObject(group, transformations);

    /*
    tinyxml2::XMLElement *models = group->FirstChildElement("models");

    this->points = parseModels(models);*/
}