#ifndef CG_PATCH_H
#define CG_PATCH_H


#include <string>
#include <vector>

#include "Vertex.h"

class Patch {
public:
    static std::vector<Vertex> generatePatch(std::string& patchFilename, int tesselation);
    static std::vector<Vertex> tessellatePatches(std::vector<std::vector<Vertex>>& patches, int divisions);
    static std::vector<Vertex> dividePatch(std::vector<Vertex>& patch, int divisions);

    static void writePatchesToFile(std::vector<std::vector<Vertex>>& patches, std::string& filename);

    static Vertex calculatePatchPoint(std::vector<Vertex>& patch, float v, float u);
    static float interpolateCoords(std::vector<Vertex>& patch, glm::mat4& bezierM, glm::mat4& bezierMT, glm::vec4& uM, glm::vec4& vM, int coord);
};


#endif //CG_PATCH_H
