#include "Core/TileMap.h"
#include "Core/GameObject.h"
#include "Core/Camera.h"
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
    Vec2 offset = associated.box.GetPos();
    Vec2 factor = layer? Vec2::Zero : Camera::pos*0.5;
    for(int y = 0; y<GetWidth(); y++){
        for(int x=0; x<GetHeight(); x++){
            tileSet->RenderTile(At(x,y,layer),(spaceX*x+offset.x+factor.x), spaceY*y+offset.y+factor.y);
        }
    }
}

void TileMap::Render(){
    // return;
    for(int z = 0; z<GetDepth(); z++){
        RenderLayer(z);
    }
}
void TileMap::Update(float dt){}

bool TileMap::Is(std::string type){
    return type == "TileMap";
}