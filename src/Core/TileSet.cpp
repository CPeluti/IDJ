#include "Core/TileSet.h"
#include "Core/Rect.h"
#include <iostream>

TileSet::TileSet(int tileWidth, int tileHeight, std::string file, std::map<int,Rect> colliders): tileSet(file)
{
    this->m_colliders = colliders;
    this->tileWidth = tileWidth;
    this->tileHeight = tileHeight;
    // std::cout << "open" << std::endl;
    // tileSet.Open(file);
    // std::cout << "opened" << std::endl;
    if(tileSet.IsOpen())
    {
        int frameCountW = tileSet.GetWidth() / tileWidth;
        int frameCountH = tileSet.GetHeight() / tileHeight;
        tileSet.SetFrameCount(frameCountW, frameCountH);
    }
}

void TileSet::RenderTile(unsigned index, float x, float y)
{
    if(index < (unsigned int)tileWidth*tileHeight){
        tileSet.SetFrame(index);
        tileSet.Render({x, y}, {tileSet.GetWidth(), tileSet.GetHeight()});
    }
}

int TileSet::GetTileWidth()
{
    return tileWidth;
}

int TileSet::GetTileHeight()
{
    return tileHeight;
}