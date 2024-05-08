/*
 * General line of thought
 *
 * Read file to determine control points of each patch
 *
 * For each patch:
 *      for v E [0, 1], v += 1 / tesselation_level
 *          for u E [0, 1], u += 1 / tesselation_level
 *              Calculate a bezier curve based on the v value
 *                  Use control points of the calculated bezier curve to calculate a point based on the u value
 * */

#include "Patch.h"
#include "Consts.h"

#include <iosfwd>
#include <fstream>

/*
 * Read patches from given file and produce final object
 * */
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

    // Read number of existing patches
    patchesNumber = std::stoi(tmp);

    std::vector<std::vector<int>> tmpPatches = std::vector<std::vector<int>>(patchesNumber);

    // Read patches
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

    // Read number of points
    pointsNumber = std::stoi(tmp);

    // Read points and store them
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

    // Transform patches from index of point to point value
    for(int i = 0; i < patchesNumber; i++){
        patches[i] = std::vector<Vertex>();

        for (auto index : tmpPatches[i]){
            patches[i].emplace_back(points[index]);
        }
    }

//    std::string tmpFilename = std::string("og_patches.patch");

    // Writing patch to file allow us to see the different bezier curves through a python program retrived from another person's github
//    writePatchesToFile(patches, tmpFilename);

    // Calculate the final object from the patches and tesselation level
    return tessellatePatches(patches, tesselation);
}

/*
 * Write patches to file - debug func to print: patches number , each patch point
 * */
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

/*
 * Produce object using each of the given patches
 * */
std::vector<Vertex> Patch::tessellatePatches(std::vector<std::vector<Vertex>>& patches, int divisions){
    // Store final points
    std::vector<Vertex> ans = std::vector<Vertex>();

    // For each patch, calculate and store the points that will draw the object
    for(auto& patch : patches){
        std::vector<Vertex> tessellatedPatch = dividePatch(patch, divisions);

        ans.insert(ans.end(), tessellatedPatch.begin(), tessellatedPatch.end());
    }

    return ans;
}

/*
 * For a given patch, calculate and store, for each u and v values, P(u, v), and build the triangles that constitute the final object
 * */
std::vector<Vertex> Patch::dividePatch(std::vector<Vertex>& patch, int divisions){
    std::vector<Vertex> ans = std::vector<Vertex>();
    std::vector<Vertex> prevPoints = std::vector<Vertex>();
    std::vector<Vertex> curPoints = std::vector<Vertex>();

    float u, v;

    // Traverse each v value
    for(int i = 0; i <= divisions; i++){
        v = static_cast<float>(i) / divisions;
        curPoints.clear();

        // Traverse each u value
        for(int j = 0; j <= divisions; j++){
            u = static_cast<float>(j) / divisions;

            // With the 4 control points obtained (1 for each previous curve), define a new curve and calculate P(u) on the given curve
            // Equivalent to calculate P(u, v)
            curPoints.emplace_back(calculatePatchPoint(patch, v, u));
        }

        // Calculate and store triangles that constitute the new object
        // Similar strategy was used to build the cylinder and other primitives
        if(i > 0){
            for(int j = 0; j < divisions; j++){
                Vertex curV = curPoints[j];
                Vertex rightV = curPoints[j+1];
                Vertex rightDownV = prevPoints[j+1];
                Vertex downV = prevPoints[j];

                // Triangle  |‾/
                //           |/

                ans.emplace_back(rightDownV);
                ans.emplace_back(rightV);
                ans.emplace_back(curV);

                // Triangle  /|
                //          /_|

                ans.emplace_back(curV);
                ans.emplace_back(downV);
                ans.emplace_back(rightDownV);
            }
        }

        prevPoints.clear();
        prevPoints.insert(prevPoints.end(), curPoints.begin(), curPoints.end());
    }

    return ans;
}

float Patch::interpolateCoords(std::vector<Vertex>& patch, glm::mat4& bezierM, glm::mat4& bezierMT, glm::vec4& uM, glm::vec4& vM, int coord){
    glm::mat4 points = glm::mat4();
    float ans = 0;
    glm::vec4 tmp;

    for(int i = 0; i < patch.size(); i++){
        points[i/4][i%4] = patch[i].coords[coord];
    }

    tmp = uM * bezierM * points * bezierMT;

    for(int i = 0; i < 4; i++){
        ans += tmp[i] * vM[i];
    }

    return ans;
}

Vertex Patch::calculatePatchPoint(std::vector<Vertex>& patch, float v, float u){
    glm::vec4 coords = glm::vec4();
    glm::vec2 text = glm::vec2(1 - u, 1 - v);
    glm::vec3 normal = glm::vec3();

    glm::vec4 uM = glm::vec4(powf(u, 3), powf(u, 2), u, 1);
    glm::vec4 uMDeriv = glm::vec4(3 * powf(u, 2), 2 * u, 1, 0);

    glm::vec4 vM = glm::vec4(powf(v, 3), powf(v, 2), v, 1);
    glm::vec4 vMDeriv = glm::vec4(3 * powf(v, 2), 2 * v, 1, 0);

    glm::mat4 bezierM = Consts::bezierCoefficients();
    glm::mat4 bezierMT = glm::transpose(bezierM);

    glm::vec3 uVector = glm::vec3();
    glm::vec3 vVector = glm::vec3();

    for(int i = 0; i < 3; i++){
        coords[i] = interpolateCoords(patch, bezierM, bezierMT, uM, vM, i);
        uVector[i] = interpolateCoords(patch, bezierM, bezierMT, uMDeriv, vM, i);
        vVector[i] = interpolateCoords(patch, bezierM, bezierMT, uM, vMDeriv, i);
    }

    coords[3] = 1;

    normal = glm::cross(uVector, vVector);

    return Vertex(coords, normal, text);
}
