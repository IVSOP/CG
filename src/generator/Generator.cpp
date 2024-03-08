#include <iostream>
#include <map>
#include <functional>
#include <string>
#include <cstdio>
#include <stdlib.h>
#include <iosfwd>
#include <fstream>

#include "Vertex.h"
#include "Plane.h"
#include "Box.h"
#include "Sphere.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Torus.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        perror("Not enough arguments.");

        return -1;
    }

    std::string storageFile = std::string();
    std::string primitive = std::string(argv[1]);
    std::vector<Vertex> points = std::vector<Vertex>();

    // TODO Check length, division radius, slices stacks and height arguments type

    std::map<std::string, std::function<std::vector<Vertex>()>> primitiveMap;
    primitiveMap.insert({"plane", [&]() {
        if (argc != 5) {
            perror("Wrong number of arguments given to Plane primitive. Make sure you provide the length of the plan and the number of divisions along the axis. Also, don't forget to provide the name of the storage file.");

            exit(-1);
        }
        storageFile = std::string(argv[4]);

        float length = std::stof(std::string(argv[2]));
        int divisions = std::stoi(std::string(argv[3]));

        return Plane::createPlanePoints(length, divisions);
    }});

    primitiveMap.insert({"box", [&]() {
        if (argc != 5) {
            perror("Wrong number of arguments given to Box primitive. Make sure you provide the length of the Box and the number of divisions along each side. Also, don't forget to provide the name of the storage file.");

            exit(-1);
        }

        storageFile = std::string(argv[4]);

        float length = std::stof(std::string(argv[2]));
        int divisions = std::stoi(std::string(argv[3]));

        return Box::createBoxPoints(length, divisions);
    }});

    primitiveMap.insert({"sphere", [&]() {
        if (argc != 6) {
            perror("Wrong number of arguments given to Sphere primitive. Make sure you provide the radius of the Sphere, the number of slices and the number of stacks. Also, don't forget to provide the name of the storage file.");

            exit(-1);
        }

        storageFile = std::string(argv[5]);

        int radius = std::stoi(std::string(argv[2]));
        int slices = std::stoi(std::string(argv[3]));
        int stacks = std::stoi(std::string(argv[4]));

        return Sphere::createSpherePoints(radius, slices, stacks);
    }});

    primitiveMap.insert({"cone", [&]() {
        if (argc != 7) {
            perror("Wrong number of arguments given to Cone primitive. Make sure you provide the radius of the base, the height of the Cone, the number of slices and the number of stacks. Also, don't forget to provide the name of the storage file.");

            exit(-1);
        }

        storageFile = std::string(argv[6]);

        float radius = std::stof(std::string(argv[2]));
        float height = std::stof(std::string(argv[3]));
        int slices = std::stoi(std::string(argv[4]));
        int stacks = std::stoi(std::string(argv[5]));

        return Cone::createConePoints(radius, height, slices, stacks);
    }});

    primitiveMap.insert({"cylinder", [&]() {
        if (argc != 7) {
            perror("Wrong number of arguments given to Cone primitive. Make sure you provide the radius of the base, the height of the Cone, the number of slices and the number of stacks. Also, don't forget to provide the name of the storage file.");

            exit(-1);
        }

        storageFile = std::string(argv[6]);

        float radius = std::stof(std::string(argv[2]));
        float height = std::stof(std::string(argv[3]));
        int slices = std::stoi(std::string(argv[4]));
        int stacks = std::stoi(std::string(argv[5]));

        return Cylinder::createCylinderPoints(radius, height, slices, stacks);
    }});

    primitiveMap.insert({"torus", [&]() {
        if (argc != 7) {
            perror("Wrong number of arguments given to Cone primitive. Make sure you provide the radius of the base, the height of the Cone, the number of slices and the number of stacks. Also, don't forget to provide the name of the storage file.");

            exit(-1);
        }

        storageFile = std::string(argv[6]);

        float intRadius = std::stof(std::string(argv[2]));
        float extRadius = std::stof(std::string(argv[3]));
        int slices = std::stoi(std::string(argv[4]));
        int stacks = std::stoi(std::string(argv[5]));

        return Torus::createTorusPoints(intRadius, extRadius, slices, stacks);
    }});

    if (primitiveMap.find(primitive) == primitiveMap.end()) {
        perror("There is no such primitive available.");

        return -1;
    }

    points = primitiveMap[primitive].operator()();

    std::ofstream outFile(storageFile.c_str(), std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(reinterpret_cast<const char*>(points.data()), points.size() * sizeof(Vertex));
        outFile.close();
    } else {
        std::cerr << "Error opening file for writing." << std::endl;
    }

    return 0;
}
