#pragma once
#include "Vec2.h"
class Rect {
    public: 
        Rect();
        Rect(float x, float y, float w, float h);
        static Rect AddVector(Rect a, Vec2 b);
        Vec2 center();

        Vec2 GetPos();
        Vec2 GetSize();
        Vec2 GetPivot();

        void SetPos(Vec2 newVec);
        void SetSize(Vec2 newVec);
        void SetPivot(Vec2 newVec);

        //Distance between the centers of two rectangles
        static float CenterDistance(Rect a, Rect b);
        //Check if a point is inside the rectangle
        bool contains(Vec2 point);


        //Move using the center as ref 
        void Move(Vec2 newPos);
        void RawMove(Vec2 newPos);

        void operator=(const Rect& b){
            this->size = b.size; 
            this->pos = b.pos;
            this->pivot = b.pivot;
        }
    private:
        Vec2 pivot;
        Vec2 pos;
        Vec2 size;
};
