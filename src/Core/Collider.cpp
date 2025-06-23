#include "Core/Collider.h"
#include "Core/GameObject.h"
#include "Core/Game.h"
#include <math.h>

#ifdef DEBUG
#include "Core/Camera.h"
#include "Core/Game.h"

#include <SDL2/SDL.h>
#endif // DEBUG

Collider::Collider(GameObject &associated, std::vector<std::string> collisionLayers, Event *event, Vec2 size, Vec2 scale, Vec2 offset, std::string tag, float weight) :   Component(associated),
																																							size(size),
																																							scale(scale),
																																							offset(offset),
																																							m_collisionLayers(collisionLayers),
																																							m_event(event),
																																							m_tag(tag),
																																							weight(weight)
{
	this->box.SetSize(size);
	if(auto s = std::move(Game::GetInstance().GetCurrentState())){
		for (auto &layer : m_collisionLayers)
		{
			s->registerCollider(layer, this);
		}
	}
}

Collider::~Collider()
{
	if(auto s = std::move(Game::GetInstance().GetCurrentState())){
		for (auto &layer : m_collisionLayers)
		{
			s->removeCollider(layer, this);
		}
		this->associated.RemoveComponent(weak_from_this());
	}
}

void Collider::Update(float dt)
{
	Vec2 center = this->associated.box.GetPos();
	Vec2 offsetWithRotation = Vec2::Rotate(offset, associated.angleDeg);
	center = center + offsetWithRotation;
	this->box.RawMove(center);
}

void Collider::Render()
{
#ifdef DEBUG
	Vec2 center(box.center());
	SDL_Point points[5];

	Vec2 point = Vec2::Rotate((Vec2(box.GetPos().x, box.GetPos().y) - center), (associated.angleDeg)) + center - Camera::pos;
	points[0] = {(int)point.x, (int)point.y};
	points[4] = {(int)point.x, (int)point.y};

	point = Vec2::Rotate((Vec2(box.GetPos().x + box.GetSize().x, box.GetPos().y) - center), (associated.angleDeg)) + center - Camera::pos;
	points[1] = {(int)point.x, (int)point.y};

	point = Vec2::Rotate((Vec2(box.GetPos().x + box.GetSize().x, box.GetPos().y + box.GetSize().y) - center), (associated.angleDeg)) + center - Camera::pos;
	points[2] = {(int)point.x, (int)point.y};

	point = Vec2::Rotate((Vec2(box.GetPos().x, box.GetPos().y + box.GetSize().y) - center), (associated.angleDeg)) + center - Camera::pos;
	points[3] = {(int)point.x, (int)point.y};

	// SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255, 0, 0, SDL_ALPHA_OPAQUE);
	for (int i = 0; i < 5; i++)
	{
		GPU_Line(Game::GetInstance().GetGPUTarget(), points[i].x, points[i].y, points[(i + 1) % 5].x, points[(i + 1) % 5].y, {255, 0, 0, 255});
	}
#endif // DEBUG
}

std::string Collider::GetTag(){
	return m_tag;
}

bool Collider::Is(std::string type)
{
	return type == "Collider";
}

void Collider::SetOffset(Vec2 offset)
{
	this->offset = offset;
}

void Collider::SetScale(Vec2 scale)
{
	this->scale = scale;
}