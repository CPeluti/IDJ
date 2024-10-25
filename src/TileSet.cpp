#include "TileSet.h"

TileSet::TileSet(int tileWidth, int tileHeight, std::string file): tileSet()
{
    this->tileWidth = tileWidth;
    this->tileHeight = tileHeight;
    tileSet.Open(file);
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
        tileSet.Render(x, y, tileSet.GetWidth(), tileSet.GetHeight());
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