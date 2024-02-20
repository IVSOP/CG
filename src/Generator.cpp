#include <iostream>
#include <map>
#include <functional>
#include <string>
#include <cstdio>
#include <stdlib.h>

#include "Vertex.h"
#include "Plane.h"
#include "Box.h"
#include "Sphere.h"
#include "Cone.h"

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

        int length = std::stoi(std::string(argv[2]));
        int divisions = std::stoi(std::string(argv[3]));

        return Plane::createPlanePoints(length, divisions);
    }});

    primitiveMap.insert({"box", [&]() {
        if (argc != 5) {
            perror("Wrong number of arguments given to Box primitive. Make sure you provide the length of the Box and the number of divisions along each side. Also, don't forget to provide the name of the storage file.");

            exit(-1);
        }

        storageFile = std::string(argv[4]);

        int length = std::stoi(std::string(argv[2]));
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

        int radius = std::stoi(std::string(argv[2]));
        int height = std::stoi(std::string(argv[3]));
        int slices = std::stoi(std::string(argv[4]));
        int stacks = std::stoi(std::string(argv[5]));

        return Cone::createConePoints(radius, height, slices, stacks);
    }});

    if (primitiveMap.find(primitive) == primitiveMap.end()) {
        perror("There is no such primitive available.");

        return -1;
    }

    points = primitiveMap[primitive].operator()();

    FILE* f = std::fopen(storageFile.c_str(), "w");
    int size = static_cast<int>(points.size());

    std::fwrite(&size, sizeof(int), 1, f);
    std::fwrite(points.data(), sizeof(Vertex), size, f);

    std::fclose(f);

    return 0;
}
