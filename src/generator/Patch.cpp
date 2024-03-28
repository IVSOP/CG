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

    std::string tmpFilename = std::string("og_patches.patch");

    // Writing patch to file allow us to see the different bezier curves through a python program retrived from another person's github
    writePatchesToFile(patches, tmpFilename);

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
    std::vector<Vertex> curvePoints = std::vector<Vertex>();

    float u, v;

    // Traverse each v value
    for(int i = 0; i <= divisions; i++){
        v = static_cast<float>(i) / divisions;
        curPoints.clear();
        curvePoints.clear();

        // Calculate control points of bezier curve defined by v
        // For each curve of the patch, calculate P(v) (walk through the current curve until you reach v value and store the point)
        for(int k = 0; k < 4; k++){
            curvePoints.emplace_back(interpolateVertex(patch[4*k], patch[4*k + 1], patch[4*k + 2], patch[4*k + 3], v));
        }

        // Traverse each u value
        for(int j = 0; j <= divisions; j++){
            u = static_cast<float>(j) / divisions;

            // With the 4 control points obtained (1 for each previous curve), define a new curve and calculate P(u) on the given curve
            // Equivalent to calculate P(u, v)
            curPoints.emplace_back(interpolateVertex(curvePoints[0], curvePoints[1], curvePoints[2], curvePoints[3], u, u, v));
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

/*
 * Calculate P(v) for current curve of the patch
 *
 * Simple version because point's normal and texture coordinates are meaningless
 * */
Vertex Patch::interpolateVertex(Vertex& p0, Vertex& p1, Vertex& p2, Vertex& p3, float t){
    float mt = 1-t;

    // Bernstein polynomials
    float coordX = pow(mt, 3) * p0.coords.x + 3.0f * pow(mt, 2) * t * p1.coords.x + 3.0f * mt * pow(t, 2) * p2.coords.x + pow(t, 3) * p3.coords.x;
    float coordY = pow(mt, 3) * p0.coords.y + 3.0f * pow(mt, 2) * t * p1.coords.y + 3.0f * mt * pow(t, 2) * p2.coords.y + pow(t, 3) * p3.coords.y;
    float coordZ = pow(mt, 3) * p0.coords.z + 3.0f * pow(mt, 2) * t * p1.coords.z + 3.0f * mt * pow(t, 2) * p2.coords.z + pow(t, 3) * p3.coords.z;

    return Vertex(coordX, coordY, coordZ);
}

/*
 * Calculate P(u) for new bezier curve (result of the calculated control points P(v) )
 *
 * More complex version: gives coordinates, normal and texture coordinates
 * */
Vertex Patch::interpolateVertex(Vertex& p0, Vertex& p1, Vertex& p2, Vertex& p3, float t, float u, float v){
    float mt = 1-t;

    // Using Bernstein Polynomial to calculate P(u)
    // Final result will be equivalent to the method taught in class

    // Derivative of a bezier curve of third degree is a bezier curve of second degree
    // This will be the tangent of the current curve

    // Control points of a bezier curve of second degree (first derivative)
    glm::vec4 p0DCoords = 3.0f * (p1.coords - p0.coords);
    glm::vec4 p1DCoords = 3.0f * (p2.coords - p1.coords);
    glm::vec4 p2DCoords = 3.0f * (p3.coords - p2.coords);

    // Calculate first derivative
    float firstDerivativeX = pow(mt, 2) * p0DCoords.x + 2 * mt * t * p1DCoords.x + pow(t, 2) * p2DCoords.x;
    float firstDerivativeY = pow(mt, 2) * p0DCoords.y + 2 * mt * t * p1DCoords.y + pow(t, 2) * p2DCoords.y;
    float firstDerivativeZ = pow(mt, 2) * p0DCoords.z + 2 * mt * t * p1DCoords.z + pow(t, 2) * p2DCoords.z;

    // Store normalized firstDerivative
    glm::vec3 firstDerivative = glm::vec3(firstDerivativeX, firstDerivativeY, firstDerivativeZ);
    glm::vec3 firstDerivativeNormalized = glm::normalize(firstDerivative);

    // Derivative of a bezier curve of second degree is a bezier curve of first degree
    // This would be the next point's tangent if the curve stopped changing (the same first and second derivative for the remaining curve)

    // Calculate control points of first degree bezier curve (second derivative)
    glm::vec4 p0DDCoords = 2.0f * (p1DCoords - p0DCoords);
    glm::vec4 p1DDCoords = 2.0f * (p2DCoords - p1DCoords);

    // Calculate second derivative
    float secondDerivativeX = mt * p0DDCoords.x + t * p1DDCoords.x;
    float secondDerivativeY = mt * p0DDCoords.y + t * p1DDCoords.y;
    float secondDerivativeZ = mt * p0DDCoords.z + t * p1DDCoords.z;

    // Store second derivative
    glm::vec3 secondDerivative = glm::vec3(secondDerivativeX, secondDerivativeY, secondDerivativeZ);


    // Formula retrieved from https://pomax.github.io/bezierinfo/#pointvectors3d
    /*
     * a = normalize(B'(t))
     * b = normalize(a + B''(t))
     * r = normalize(b × a)
     * normal = normalize(r × a)
     * */

    // b from the formula above
    glm::vec3 sumFirstSecondDerivatives = glm::normalize(firstDerivativeNormalized + secondDerivative);

    // r from the formula above
    glm::vec3 crossProdNormalized = glm::normalize(glm::cross(sumFirstSecondDerivatives, firstDerivativeNormalized));

    // Point's normal
    glm::vec3 normal = glm::normalize(glm::cross(crossProdNormalized, firstDerivativeNormalized));

    // Bernstein polynomials for third degree bezier curve (used to calculate P(u) coordinates)
    float coordX = pow(mt, 3) * p0.coords.x + 3.0f * pow(mt, 2) * t * p1.coords.x + 3.0f * mt * pow(t, 2) * p2.coords.x + pow(t, 3) * p3.coords.x;
    float coordY = pow(mt, 3) * p0.coords.y + 3.0f * pow(mt, 2) * t * p1.coords.y + 3.0f * mt * pow(t, 2) * p2.coords.y + pow(t, 3) * p3.coords.y;
    float coordZ = pow(mt, 3) * p0.coords.z + 3.0f * pow(mt, 2) * t * p1.coords.z + 3.0f * mt * pow(t, 2) * p2.coords.z + pow(t, 3) * p3.coords.z;

    // Point's normal coordinates
    float normalX = normal.x;
    float normalY = normal.y;
    float normalZ = normal.z;

    // Point's texture coordinate
    // TODO Find explanation for the current results
    float textX = 1 - v; // flip texture vertical position
    float textY = 1 - u; // flip texture horizontal position

    return Vertex(coordX, coordY, coordZ, normalX, normalY, normalZ, textX, textY);
}