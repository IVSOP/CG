#include "common.h"

const GLchar *readFromFile(const char *filepath) {
    std::ifstream inFile(filepath, std::ios::binary);

    if (inFile.is_open()) {
        inFile.seekg(0, std::ios::end);
        size_t fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);


        GLchar* ret = new GLchar[fileSize + 1];

        inFile.read(reinterpret_cast<GLchar*>(ret), fileSize);

        inFile.close();

        ret[fileSize] = '\0';

        /*if (charread. != fileSize) {
            fprintf(stderr, "Did not read all chars: %ld vs %ld\n", size, charread);
            exit(1);
        }*/

        return ret;
    }

    std::cerr << "Invalid file path " << filepath << std::endl;

    exit(1);
}
