#ifndef CG_TRANSLATE_H
#define CG_TRANSLATE_H


struct Translate {
    int x, y, z;

    Translate(int x, int y, int z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Translate(){
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }
};


#endif //CG_TRANSLATE_H
