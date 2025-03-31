#include "Core/Rect.h"
#include <iostream>
Rect::Rect() {
    this->pivot = {0,0};
    this->size = {0,0};
    this->pos = {0,0};
}

Rect::Rect(float x, float y, float w, float h) {
    this->size = {w,h};
    this->pos = {x,y};
    this->pivot = {w/2, h/2};
}

Rect Rect::AddVector(Rect a, Vec2 b){
    return Rect(a.pos.x + b.x, a.pos.y + b.y, a.size.x, a.size.y);
}
Vec2 Rect::center(){
    return Vec2(this->pos.x + this->size.x/2, this->pos.y + this->size.y/2);
}
//Distance between the centers of two rectangles
float Rect::CenterDistance(Rect a, Rect b){
    return Vec2::Distance(a.center(), b.center());
}
//Check if a point is inside the rectangle
bool Rect::contains(Vec2 point){
    return (point.x >= this->pos.x && point.x <= this->pos.x + this->size.x && point.y >= this->pos.y && point.y <= this->pos.y + this->size.y);
}

void Rect::Move(Vec2 newPos){
    pos = newPos-pivot;

}

void Rect::RawMove(Vec2 newPos){
    pos = newPos;
}

Vec2 Rect::GetSize(){
    return this->size;
}

Vec2 Rect::GetPos(){
    return this->pos;
}

Vec2 Rect::GetPivot(){
    return this->pivot;
}


void Rect::SetSize(Vec2 newVec){
    this->size = newVec;
    SetPivot({newVec.x/2, newVec.y/2});
}

void Rect::SetPos(Vec2 newVec){
    this->pos = newVec;
}

void Rect::SetPivot(Vec2 newVec){
    this->pivot = newVec;
}