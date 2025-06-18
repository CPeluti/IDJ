#include "Core/Vec2.h"
#include <cmath>
#include <math.h>
Vec2 Vec2::Zero = Vec2();
Vec2::Vec2() {
    this->x = 0;
    this->y = 0;
}

Vec2::Vec2(float x,float y) {
    this->x = x;
    this->y = y;
}

Vec2::Vec2(double x,double y) {
    this->x = x;
    this->y = y;
}

Vec2::Vec2(int x,int y) {
    this->x = (float)x;
    this->y = (float)y;
}

Vec2 Vec2::Add(Vec2 a, Vec2 b) {
    return Vec2(a.x + b.x, a.y + b.y);
}

Vec2 Vec2::Sub(Vec2 a, Vec2 b) {
    return Vec2(a.x - b.x, a.y - b.y);
}

float Vec2::Dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

Vec2 Vec2::MultiplyByScalar(Vec2 a, int b) {
    return Vec2(a.x * b, a.y * b);
}
Vec2 Vec2::MultiplyByScalar(Vec2 a, float b) {
    return Vec2(a.x * b, a.y * b);
}
Vec2 Vec2::MultiplyByScalar(Vec2 a, double b) {
    return Vec2(a.x * (float)b, a.y * (float)b);
}
Vec2 Vec2::DivideByScalar(Vec2 a, int b) {
    return Vec2(a.x / b, a.y / b);
}
Vec2 Vec2::DivideByScalar(Vec2 a, float b) {
    return Vec2(a.x / b, a.y / b);
}
Vec2 operator+(const Vec2& a, const Vec2& b) {
    return Vec2::Add(a, b);
}

Vec2 operator-(const Vec2& a, const Vec2& b) {
    return Vec2::Sub(a, b);
}

Vec2 operator*(const Vec2& a, int b) {
    return Vec2::MultiplyByScalar(a, b);
}
Vec2 operator*(const Vec2 a, float b) {
    return Vec2::MultiplyByScalar(a, b);
}
Vec2 operator*(const Vec2 a, double b) {
    return Vec2::MultiplyByScalar(a, b);
}
float operator*(const Vec2& a, const Vec2& b) {
    return Vec2::Dot(a,b);
}

Vec2 operator/(const Vec2& a, int b) {
    return Vec2::DivideByScalar(a, b);
}
Vec2 operator/(const Vec2& a, float b) {
    return Vec2::DivideByScalar(a, b);
}
bool operator==(const Vec2 a, const Vec2 b) {
    return a.x == b.x && a.y == b.y;
}

bool operator!=(const Vec2 a, const Vec2 b) {
    return a.x != b.x || a.y != b.y;
}

float Vec2::magnitude() {
    return sqrt(x * x + y * y);
}

Vec2 Vec2::normalized(){
    float magnitude = this->magnitude();
    return *this/magnitude;
}

float Vec2::Distance(Vec2 a, Vec2 b) {
    return (a-b).magnitude();
}

float Vec2::inclination(){
    return atan2(y,x);
}

float Vec2::Angle(Vec2 a, Vec2 b){
    float angle = atan2(b.y-a.y, b.x-a.x)*180/M_PI;
    return (angle < 0 ? angle + 360: angle);
}

Vec2 Vec2::Rotate(Vec2 a, float angle){
    float rad = angle *M_PI/180;
    return Vec2(a.x*cos(rad) - a.y*sin(rad), a.x*sin(rad) + a.y*cos(rad));
}

float Vec2::lerp(float a, float b, float t){
    return (1.0f-t)*b+t*a;
}
// Vec2 operator=(const Vec2& a){
//     return Vec2(a.x, a.y);
// }