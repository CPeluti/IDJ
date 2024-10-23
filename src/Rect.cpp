#include "Rect.h"

Rect::Rect() {
    this->x = 0;
    this->y = 0;
    this->w = 0;
    this->h = 0;
}

Rect Rect::AddVector(Rect a, Vec2 b){
    return Rect(a.x + b.x, a.y + b.y, a.w, a.h);
}
Vec2 Rect::center(){
    return Vec2(this->x + this->w/2, this->y + this->h/2);
}
//Distance between the centers of two rectangles
float Rect::CenterDistance(Rect a, Rect b){
    return Vec2::Distance(a.center(), b.center());
}
//Check if a point is inside the rectangle
bool Rect::contains(Vec2 point){
    return (point.x >= this->x && point.x <= this->x + this->w && point.y >= this->y && point.y <= this->y + this->h);
}