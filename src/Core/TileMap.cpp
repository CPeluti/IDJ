#include "Core/TileMap.h"
#include "Core/GameObject.h"
#include "Core/Camera.h"
#include "Game/Character.h"
#include "Core/Log.h"
#include <fstream>
#include <memory>
#include <string>
#include <iostream>

TileMap::TileMap(GameObject& associated, std::string file, TileSet* tileSet): Component(associated), tileSet(tileSet){
    Load(file);
}

void TileMap::Load(std::string file)
{
    std::string line;
    std::ifstream map(file);
    int counter = 0;
    if(map.is_open()){
        while(std::getline(map, line,',')){
            // cout << "item" << counter
            int number = std::stoi(line);
            switch (counter)
            {
            case 0:
                this->mapWidth = number;
                break;
            case 1:
                this->mapHeight = number;
                break;
            case 2:
                this->mapDepth = number;
                break;
            default:
                tileMatrix.push_back(number);
                break;
            }
            counter++;
        }
    }
}

int TileMap::GetWidth()
{
    return mapWidth;
}
int TileMap::GetHeight()
{
    return mapHeight;
}
int TileMap::GetDepth()
{
    return mapDepth;
}

void TileMap::SetTileSet(TileSet *ts)
{
    tileSet.release();
    tileSet.reset(ts);
}

void TileMap::Start(){
    std::vector<Rect> colliders2BCreated;

    this->associated.box.SetSize({mapWidth*tileSet->GetTileWidth(), mapHeight*tileSet->GetTileHeight()});
    for(int i = 0; i<mapHeight*mapWidth; i++){
        Rect* colliderMetaData = tileSet->getColliderMetaData(tileMatrix[(mapWidth*mapHeight*(mapDepth-1)) + i]);
        if(colliderMetaData){
            int colliderCol = i%(mapWidth); 
            int colliderLine = i/(mapWidth);
            Vec2 colliderOffset = colliderMetaData->GetPos() + Vec2(colliderCol*tileSet->GetTileWidth(), colliderLine*tileSet->GetTileHeight());
            // Collider* collider = new Collider(this->associated, {"phys0"}, new OnCollisionEvent(associated), colliderMetaData->GetSize(), {1,1}, colliderOffset, "phys");
            colliders2BCreated.emplace_back(Rect(colliderOffset, colliderMetaData->GetSize()));
            // this->associated.AddComponent(collider);
        }
    }
    auto collider = colliders2BCreated.begin();
    int counter = 0;
    while (collider != colliders2BCreated.end()) {
        auto collider2 = collider+1;
        int counter2=0;
        while(collider2 != colliders2BCreated.end()){
            
            if(collider2 != collider){
                Vec2 cantoSupEsquerdoCollider = collider->GetPos();
                Vec2 cantoInfEsquerdoCollider = cantoSupEsquerdoCollider+Vec2(.0,collider->GetSize().y);
                Vec2 cantoSupDireitoCollider = cantoSupEsquerdoCollider+Vec2(collider->GetSize().x,.0);
                Vec2 cantoInfDireitoCollider = cantoSupDireitoCollider+Vec2(.0,collider->GetSize().y);

                Vec2 cantoSupEsquerdoCollider2 = collider2->GetPos();
                Vec2 cantoInfEsquerdoCollider2 = cantoSupEsquerdoCollider2+Vec2(.0,collider2->GetSize().y);
                Vec2 cantoSupDireitoCollider2 = cantoSupEsquerdoCollider2+Vec2(collider2->GetSize().x,.0);
                Vec2 cantoInfDireitoCollider2 = cantoSupDireitoCollider2+Vec2(.0,collider2->GetSize().y);
                bool checkIntersectionX = cantoSupDireitoCollider.x >= cantoSupEsquerdoCollider2.x && cantoInfDireitoCollider.x >= cantoInfEsquerdoCollider2.x;
                bool checkSameY = cantoSupDireitoCollider.y == cantoSupDireitoCollider2.y && cantoInfDireitoCollider.y == cantoInfDireitoCollider2.y;
                bool checkIntersectionY = cantoInfEsquerdoCollider.y <= cantoSupEsquerdoCollider2.y && cantoSupDireitoCollider.y <= cantoSupDireitoCollider2.y;
                bool checkSameX = cantoSupEsquerdoCollider.x == cantoInfEsquerdoCollider2.x && cantoSupDireitoCollider.x == cantoInfDireitoCollider2.x;

                if((checkIntersectionX && checkSameY)||(checkIntersectionY && checkSameX)){
                    Vec2 newSize;
                    if(checkIntersectionX && checkSameY){
                        newSize = {cantoSupDireitoCollider2.x-cantoSupEsquerdoCollider.x, collider->GetSize().y};
                    } else{
                        newSize = { collider->GetSize().x, cantoInfEsquerdoCollider2.y-cantoSupEsquerdoCollider.y};
                    }
                    collider->SetSize(newSize);
                    if(collider>collider2){
                        colliders2BCreated.erase(collider2);
                        collider--;
                    } else {
                        colliders2BCreated.erase(collider2);
                        collider2--;
                    }
                }
            }
            collider2++;
            counter2++;
        }
        
        collider++;
        counter++;
    }

    for(auto c : colliders2BCreated){
        std::shared_ptr<Collider> collider = std::make_shared<Collider>(this->associated, std::vector<std::string>{"phys0"}, new OnCollisionEvent(associated), c.GetSize(), Vec2{1,1}, c.GetPos(), "phys");
        m_colliders.emplace_back(collider);
        this->associated.AddComponent(collider);
    }
    std::shared_ptr<Collider> collider1 = std::make_shared<Collider>(this->associated, std::vector<std::string>{"phys0"}, new OnCollisionEvent(associated), Vec2{100,100}, Vec2{1,1}, Vec2{1480,1480}, "phys12");
    m_colliders.emplace_back(collider1);
    this->associated.AddComponent(collider1);
}

int &TileMap::At(int x, int y, int z)
{
    // if (x < mapWidth && y < mapHeight && z < mapDepth){
        int pos = x+(y*GetWidth()) + z * GetHeight()* GetWidth();
    
        return this->tileMatrix[pos];
    // } else {
    // std::cout << "Coordinates exceed tilemap boundary" << std::endl;
    // }
}

void TileMap::RenderLayer(int layer){

    int spaceX = tileSet->GetTileWidth();
    int spaceY = tileSet->GetTileHeight();
    Vec2 offset = Vec2::Zero;
    Vec2 factor = Vec2::Zero;
    // Vec2 factor = layer? Vec2::Zero : Camera::pos*0.5;
    for(int x = 0; x<GetWidth(); x++){
        for(int y=0; y<GetHeight(); y++){
            tileSet->RenderTile(At(x,y,layer),(spaceX*x+offset.x+factor.x), spaceY*y+offset.y+factor.y);
        }
    }
}

void TileMap::Render(){
    // return;
    // Vec2 teste = Character::player->getAssociated()->box.GetPos();
    for(int z = 0; z<GetDepth(); z++){
        RenderLayer(z);
    }
}
void TileMap::Update(float dt){}

bool TileMap::Is(std::string type){
    return type == "TileMap";
}