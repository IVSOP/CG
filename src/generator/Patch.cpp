#include "Patch.h"
#include "Consts.h"

#include <iosfwd>
#include <fstream>

std::vector<Vertex> Patch::generatePatch(std::string& patchFilename, int tesselation){
    std::ifstream patchFile(patchFilename.c_str(), std::ios_base::in);

    if (!patchFile.is_open()) {
        std::cerr << "Error opening file for reading." << std::endl;

        return std::vector<Vertex>();
    }

    // Grant we are reading from the file's beginning
    patchFile.seekg(0, std::ios_base::beg);

    int patchesNumber, pointsNumber;

    std::string tmp, token;
    std::getline(patchFile, tmp);

    patchesNumber = std::stoi(tmp);

    std::vector<std::vector<int>> tmpPatches = std::vector<std::vector<int>>(patchesNumber);

    for(int i = 0; i < patchesNumber; i++){
        tmpPatches[i] = std::vector<int>();

        std::getline(patchFile, tmp);
        int j;

        while ((j = tmp.find(',')) != std::string::npos) {
            token = tmp.substr(0, j);

            tmpPatches[i].emplace_back(std::stoi(token));

            tmp.erase(0, j + 1);
        }

        tmpPatches[i].emplace_back(std::stoi(tmp));
    }

    std::getline(patchFile, tmp);
    std::vector<Vertex> points = std::vector<Vertex>();
    std::vector<float> tmpPoint = std::vector<float>();

    pointsNumber = std::stoi(tmp);

    for(int i = 0; i < pointsNumber; i++) {
        std::getline(patchFile, tmp);
        tmpPoint.clear();

        int j;

        while ((j = tmp.find(',')) != std::string::npos) {
            token = tmp.substr(0, j);

            tmpPoint.emplace_back(std::stof(token));

            tmp.erase(0, j + 1);
        }

        tmpPoint.emplace_back(std::stof(tmp));

        points.emplace_back(tmpPoint[0], tmpPoint[1], tmpPoint[2]);
    }

    std::vector<std::vector<Vertex>> patches = std::vector<std::vector<Vertex>>(patchesNumber);

    for(int i = 0; i < patchesNumber; i++){
        patches[i] = std::vector<Vertex>();

        for (auto index : tmpPatches[i]){
            patches[i].emplace_back(points[index]);
        }
    }

    std::string tmpFilename = std::string("og_patches.patch");
    writePatchesToFile(patches, tmpFilename);

    return tessellatePatches(patches, tesselation);
}

void Patch::writePatchesToFile(std::vector<std::vector<Vertex>>& patches, std::string& filename){
    std::ofstream outFile(filename.c_str(), std::ios_base::out);

    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }

    /*
     * nº patches
     * for patch
     *     for point in patch
     *          (point.x; point.y; point.z)
     * */
    outFile << patches.size() << std::endl;

    for(auto& patch: patches) {
        for (auto& point: patch) {
            outFile << '(' << point.coords.x << ";" << point.coords.y << ";" << point.coords.z << ")" << " ";
        }

        outFile << '\n';
    }
    outFile.close();
}

std::vector<Vertex> Patch::tessellatePatches(std::vector<std::vector<Vertex>>& patches, int divisions){
    std::vector<Vertex> ans = std::vector<Vertex>();

    for(auto& patch : patches){
        std::vector<Vertex> tessellatedPatch = dividePatch(patch, divisions);

        ans.insert(ans.end(), tessellatedPatch.begin(), tessellatedPatch.end());
    }

    return ans;
}

/*
 * De Casteljau’s algorithm
 * Divide each patch into smaller patches until each patch projects to only one point (pixel in screen)
 * */
std::vector<Vertex> Patch::dividePatch(std::vector<Vertex>& patch, int divisions){
    std::vector<Vertex> ans = std::vector<Vertex>();
    std::vector<Vertex> prevPoints = std::vector<Vertex>();
    std::vector<Vertex> curPoints = std::vector<Vertex>();

    float u, v;

    for(int i = 0; i <= divisions; i++){
        v = static_cast<float>(i) / divisions;
        curPoints.clear();

        for(int j = 0; j <= divisions; j++){
            u = static_cast<float>(j) / divisions;

            curPoints.emplace_back(interpolatePatch(patch, u, v));
        }

        if(i > 0){
            for(int j = 0; j < divisions; j++){
                Vertex curV = curPoints[j];
                Vertex rightV = curPoints[j+1];
                Vertex rightDownV = prevPoints[j+1];
                Vertex downV = prevPoints[j];

                // Triangle  |‾/
                //           |/

                ans.emplace_back(curV);
                ans.emplace_back(rightV);
                ans.emplace_back(rightDownV);

                // Triangle  /|
                //          /_|

                ans.emplace_back(rightDownV);
                ans.emplace_back(downV);
                ans.emplace_back(curV);
            }
        }

        prevPoints.clear();
        prevPoints.insert(prevPoints.end(), curPoints.begin(), curPoints.end());
    }

    return ans;
}

Vertex Patch::interpolatePatch(std::vector<Vertex>& patch, float u, float v){
    std::vector<Vertex> curvePoints = std::vector<Vertex>();
    glm::vec3 dU = derivativeUBezier(patch, u, v);
    glm::vec3 dV = derivativeVBezier(patch, u, v);

    glm::vec3 normal = glm::normalize(glm::cross(dV, dU));

    for(int i = 0; i < 4; i++){

        curvePoints.emplace_back(interpolateVertex(patch[4*i], patch[4*i + 1], patch[4*i + 2], patch[4*i + 3], v));
    }

    return interpolateVertex(curvePoints[0], curvePoints[1], curvePoints[2], curvePoints[3], u, u, v, normal);
}

/* Code inspired on https://www.scratchapixel.com/lessons/geometry/bezier-curve-rendering-utah-teapot/bezier-patch-normal.html */

glm::vec3 Patch::derivativeUBezier(std::vector<Vertex>& controlPoints, float u, float v){
    std::vector<Vertex> curvePoints = std::vector<Vertex>();
    float mu = 1 - u;

    for(int i = 0; i < 4; i++) curvePoints.emplace_back(interpolateVertex(controlPoints[4*i], controlPoints[4*i + 1], controlPoints[4*i + 2], controlPoints[4*i + 3], v));

    return (-3 * (float) pow(mu, 2)) * glm::vec3(curvePoints[0].coords.x, curvePoints[0].coords.y, curvePoints[0].coords.z) +
            (3 * (float) pow(mu, 2) - 6 * u * mu) * glm::vec3(curvePoints[1].coords.x, curvePoints[1].coords.y, curvePoints[1].coords.z) +
            (6 * mu  * u - 3 * (float) pow(u, 2)) * glm::vec3(curvePoints[2].coords.x, curvePoints[2].coords.y, curvePoints[2].coords.z) +
            (3 * (float) pow(u, 2)) * glm::vec3(curvePoints[3].coords.x, curvePoints[3].coords.y, curvePoints[3].coords.z);
}

glm::vec3 Patch::derivativeVBezier(std::vector<Vertex>& controlPoints, float u, float v){
    std::vector<Vertex> curvePoints = std::vector<Vertex>();
    float mv = 1 - v;

    for(int i = 0; i < 4; i++) curvePoints.emplace_back(interpolateVertex(controlPoints[i], controlPoints[i + 4], controlPoints[i + 8], controlPoints[i + 12], u));

    return (-3 * (float) pow(mv, 2)) * glm::vec3(curvePoints[0].coords.x, curvePoints[0].coords.y, curvePoints[0].coords.z) +
           (3 * (float) pow(mv, 2) - 6 * v * mv) * glm::vec3(curvePoints[1].coords.x, curvePoints[1].coords.y, curvePoints[1].coords.z) +
           (6 * mv  * v - 3 * (float) pow(v, 2)) * glm::vec3(curvePoints[2].coords.x, curvePoints[2].coords.y, curvePoints[2].coords.z) +
           (3 * (float) pow(v, 2)) * glm::vec3(curvePoints[3].coords.x, curvePoints[3].coords.y, curvePoints[3].coords.z);
}

Vertex Patch::interpolateVertex(Vertex& p0, Vertex& p1, Vertex& p2, Vertex& p3, float t){
    float mt = 1-t;

    // Bernstein polynomials
    float coordX = pow(mt, 3) * p0.coords.x + 3.0f * pow(mt, 2) * t * p1.coords.x + 3.0f * mt * pow(t, 2) * p2.coords.x + pow(t, 3) * p3.coords.x;
    float coordY = pow(mt, 3) * p0.coords.y + 3.0f * pow(mt, 2) * t * p1.coords.y + 3.0f * mt * pow(t, 2) * p2.coords.y + pow(t, 3) * p3.coords.y;
    float coordZ = pow(mt, 3) * p0.coords.z + 3.0f * pow(mt, 2) * t * p1.coords.z + 3.0f * mt * pow(t, 2) * p2.coords.z + pow(t, 3) * p3.coords.z;

    return Vertex(coordX, coordY, coordZ);
}

Vertex Patch::interpolateVertex(Vertex& p0, Vertex& p1, Vertex& p2, Vertex& p3, float t, float u, float v, glm::vec3 normal){
    float mt = 1-t;

    // d mt = 3 mt^2 p0.x + 6 mt * t * p1.x +  +

    // Bernstein polynomials
    float coordX = pow(mt, 3) * p0.coords.x + 3.0f * pow(mt, 2) * t * p1.coords.x + 3.0f * mt * pow(t, 2) * p2.coords.x + pow(t, 3) * p3.coords.x;
    float coordY = pow(mt, 3) * p0.coords.y + 3.0f * pow(mt, 2) * t * p1.coords.y + 3.0f * mt * pow(t, 2) * p2.coords.y + pow(t, 3) * p3.coords.y;
    float coordZ = pow(mt, 3) * p0.coords.z + 3.0f * pow(mt, 2) * t * p1.coords.z + 3.0f * mt * pow(t, 2) * p2.coords.z + pow(t, 3) * p3.coords.z;
    float normalX = normal.x;
    float normalY = normal.y;
    float normalZ = normal.z;
    float textX = 1 - v;
    float textY = 1 - u;

    return Vertex(coordX, coordY, coordZ, normalX, normalY, normalZ, textX, textY);
}