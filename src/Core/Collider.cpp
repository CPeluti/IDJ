#include "Core/Collider.h"
#include "Core/GameObject.h"
#include <math.h>

#ifdef DEBUG
#include "Core/Camera.h"
#include "Core/Game.h"

#include <SDL3/SDL.h>
#endif // DEBUG

Collider::Collider(GameObject& associated, Vec2 scale, Vec2 offset): 
    Component(associated), 
    scale(scale), 
    offset(offset)
{}

void Collider::Update(float dt){
    this->box = this->associated.box;
    Vec2 newScale = this->box.GetSize();
    this->box.SetSize({newScale.x * scale.x, newScale.y * scale.y});
    Vec2 center = this->associated.box.center();
    Vec2 offsetWithRotation = Vec2::Rotate(offset, associated.angleDeg);
    center = center + offsetWithRotation;
    this->box.Move(center);
}

void Collider::Render() {
#ifdef DEBUG
	Vec2 center( box.center() );
	SDL_FPoint points[5];

	Vec2 point = Vec2::Rotate((Vec2(box.GetPos().x, box.GetPos().y) - center),( associated.angleDeg ))
					+ center - Camera::pos;
	points[0] = {point.x, point.y};
	points[4] = {point.x, point.y};
	
	point = Vec2::Rotate((Vec2(box.GetPos().x + box.GetSize().x, box.GetPos().y) - center),( associated.angleDeg ))
					+ center - Camera::pos;
	points[1] = {point.x, point.y};
	
	point = Vec2::Rotate((Vec2(box.GetPos().x + box.GetSize().x, box.GetPos().y + box.GetSize().y) - center),( associated.angleDeg) )
					+ center - Camera::pos;
	points[2] = {point.x, point.y};
	
	point = Vec2::Rotate((Vec2(box.GetPos().x, box.GetPos().y + box.GetSize().y) - center),( associated.angleDeg ) )
					+ center - Camera::pos;
	points[3] = {point.x, point.y};

	SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderLines(Game::GetInstance().GetRenderer(), points, 5);
#endif // DEBUG
}

bool Collider::Is(std::string type){
    return type == "Collider";
}

void Collider::SetOffset(Vec2 offset){
    this->offset = offset;
}

void Collider::SetScale(Vec2 scale){
    this->scale = scale;
}