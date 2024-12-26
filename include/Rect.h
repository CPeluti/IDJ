#pragma once
#include "Vec2.h"
class Rect {
    public: 
        float x,y,w,h;
        Rect();
        Rect(float x, float y, float w, float h);
        static Rect AddVector(Rect a, Vec2 b);
        Vec2 center();
        //Distance between the centers of two rectangles
        static float CenterDistance(Rect a, Rect b);
        //Check if a point is inside the rectangle
        bool contains(Vec2 point);
        void operator=(const Rect& b){
            this->h = b.h; 
            this->w = b.w;
            this->x = b.x;
            this->y = b.y;
        }
};
