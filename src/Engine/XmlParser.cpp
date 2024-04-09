#include "XmlParser.h"
#include "tinyxml2.h"
#include "Transformation.h"

#include <iosfwd>
#include <fstream>

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

std::vector<std::vector<Vertex>> XmlParser::getPoints(){
    return this->engineObject.getPoints();
}

std::vector<Engine_Object_Info> XmlParser::getObjectInfo(float t) {
    return this->engineObject.getObjectInfo(t, Transformation());
}

std::vector<std::pair<Engine_Object_Materials, std::vector<Vertex>>> XmlParser::parseModels(tinyxml2::XMLElement *models){
    std::vector<std::pair<Engine_Object_Materials, std::vector<Vertex>>> ans = std::vector<std::pair<Engine_Object_Materials, std::vector<Vertex>>>();

    for (tinyxml2::XMLElement* node = models->FirstChildElement(); node != nullptr; node = node->NextSiblingElement()){
        std::string name = std::string(node->Name());

        if(name == "model"){
            std::vector<Vertex> tmp = this->parseVertex(node);

            Engine_Object_Materials engineObjectMaterials = this->parseEngineObjectMaterials(node);

            ans.emplace_back(std::pair<Engine_Object_Materials, std::vector<Vertex>>(engineObjectMaterials, tmp));

            continue;
        }

        perror("Found unrecognized tag inside a models tag.");
        std::cout << name << std::endl;
    }

    return ans;
}

// TODO Acabar
std::vector<std::variant<Translate, Rotate, Scale>> XmlParser::parseTransformation(tinyxml2::XMLElement *transform){
    std::vector<std::variant<Translate, Rotate, Scale>> transformations = std::vector<std::variant<Translate, Rotate, Scale>>();

    for (tinyxml2::XMLElement* node = transform->FirstChildElement(); node != nullptr; node = node->NextSiblingElement()){
        std::string name = std::string(node->Name());

        if(name == "rotate"){
            Rotate r = Rotate(node->DoubleAttribute("angle"), node->FloatAttribute("time"), node->FloatAttribute("x"), node->FloatAttribute("y"), node->FloatAttribute("z"), node->FindAttribute("time") != nullptr);

            transformations.emplace_back(r);

            continue;
        }

        if(name == "translate"){
            std::vector<Vertex> points = std::vector<Vertex>();

            bool curve = node->FindAttribute("time") != nullptr;

            if(curve){
                for(tinyxml2::XMLElement* point = node->FirstChildElement(); point != nullptr; point = point->NextSiblingElement()){
                    std::string point_name = std::string(point->Name());

                    if(point_name == "Point"){
                        points.emplace_back(point->FloatAttribute("x"), point->FloatAttribute("y"), point->FloatAttribute("z"));

                        continue;
                    }

                    perror("Found unrecognized tag inside a translate tag.");
                    std::cout << point_name << std::endl;
                }
            }

            Translate translate = Translate(node->FloatAttribute("time"), node->BoolAttribute("align"), node->FloatAttribute("x"), node->FloatAttribute("y"), node->FloatAttribute("z"), curve, points);

            transformations.emplace_back(translate);

            continue;
        }

        if(name == "scale"){
            Scale s = Scale(node->FloatAttribute("x"), node->FloatAttribute("y"), node->FloatAttribute("z"));

            transformations.emplace_back(s);

            continue;
        }

        perror("Found unrecognized tag inside a transform tag.");
        std::cout << name << std::endl;
    }

    return transformations;
}

std::vector<Vertex> XmlParser::parseVertex(tinyxml2::XMLElement *model){
    std::vector<Vertex> points;
    std::ifstream inFile(model->Attribute("file"), std::ios::binary);

    if (inFile.is_open()) {
        inFile.seekg(0, std::ios::end);
        size_t fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        points.resize(fileSize / sizeof(Vertex));
        inFile.read(reinterpret_cast<char*>(points.data()), fileSize);
        inFile.close();

    } else {
        std::cerr << "Error opening file for reading." << " " << model->Attribute("file") << std::endl;
    }

    return points;
}

Engine_Object_Materials XmlParser::parseEngineObjectMaterials(tinyxml2::XMLElement *model){
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 ambient = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
    glm::vec3 emissive = glm::vec3(0.0f);
    int shininess = 0;

    std::string texture = std::string("white.png");

    bool hasTexture = false, hasColor = false;

    for (tinyxml2::XMLElement* node = model->FirstChildElement(); node != nullptr; node = node->NextSiblingElement()){
        std::string name = std::string(node->Name());

        if(name == "color"){
            hasColor = true;

            tinyxml2::XMLElement* nodeChild = node->FirstChildElement("diffuse");
            diffuse = glm::vec3(nodeChild->FloatAttribute("R") / 255.0f, nodeChild->FloatAttribute("G") / 255.0f, nodeChild->FloatAttribute("B") / 255.0f);

            nodeChild = node->FirstChildElement("ambient");
            ambient = glm::vec3(nodeChild->FloatAttribute("R") / 255.0f, nodeChild->FloatAttribute("G") / 255.0f, nodeChild->FloatAttribute("B") / 255.0f);

            nodeChild = node->FirstChildElement("specular");
            specular = glm::vec3(nodeChild->FloatAttribute("R") / 255.0f, nodeChild->FloatAttribute("G") / 255.0f, nodeChild->FloatAttribute("B") / 255.0f);

            nodeChild = node->FirstChildElement("emissive");
            emissive = glm::vec3(nodeChild->FloatAttribute("R") / 255.0f, nodeChild->FloatAttribute("G") / 255.0f, nodeChild->FloatAttribute("B") / 255.0f);

            nodeChild = node->FirstChildElement("shininess");
            shininess = nodeChild->IntAttribute("value");

            continue;
        }

        if(name == "texture"){
            hasTexture = true;

            texture = std::string(node->Attribute("file"));

            continue;
        }

        perror("Found unrecognized tag inside a models tag.");
        std::cout << name << std::endl;
    }

    if(!hasColor && !hasTexture){
        diffuse = glm::vec3(200 / 255.0f, 200 / 255.0f, 200 / 255.0f);
        ambient = glm::vec3(50 / 255.0f, 50 / 255.0f, 50 / 255.0f);
        specular = glm::vec3(0, 0, 0);
        emissive = glm::vec3(0, 0, 0);
        shininess = 0;
    }

    return Engine_Object_Materials(diffuse, ambient, specular, emissive, shininess, texture);
}

Engine_Object XmlParser::parseEngineObject(tinyxml2::XMLElement *group){
    std::vector<std::variant<Translate, Rotate, Scale>> transformations = std::vector<std::variant<Translate, Rotate, Scale>>();
    std::vector<std::pair<Engine_Object_Materials, std::vector<Vertex>>> obj_points = std::vector<std::pair<Engine_Object_Materials, std::vector<Vertex>>>();
    std::vector<Engine_Object> child_objs = std::vector<Engine_Object>();

    for (tinyxml2::XMLElement* node = group->FirstChildElement(); node != nullptr; node = node->NextSiblingElement()){
        std::string name = std::string(node->Name());

        if(name == "transform"){
            std::vector<std::variant<Translate, Rotate, Scale>> t = parseTransformation(node);
            transformations.insert(transformations.end(), t.begin(), t.end());

            continue;
        }

        if(name == "models"){
            std::vector<std::pair<Engine_Object_Materials, std::vector<Vertex>>> tmp_points = parseModels(node);
            obj_points.insert(obj_points.end(), tmp_points.begin(), tmp_points.end());

            continue;
        }

        if(name == "group"){
            Engine_Object child = parseEngineObject(node);
            child_objs.emplace_back(child.transformations, child.points, child.children_objects);

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

    this->engineObject = parseEngineObject(group);
}