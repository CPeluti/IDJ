#pragma once
#include <string>
#include "Sprite.h"
#include "Rect.h"

class TileSet {
    public:
        TileSet(int tileWidth, int tileHeight, std::string file, std::map<int,Rect> colliders = {});
        void RenderTile(unsigned index, float x, float y);
        int GetTileWidth();
        int GetTileHeight();
        inline Rect* getColliderMetaData(int tile){
            if(m_colliders.find(tile) != m_colliders.end()){
                return &m_colliders[tile];
            }
            return nullptr;
        }
    private:
        std::map<int,Rect> m_colliders;
        Sprite tileSet;
        int tileWidth;
        int tileHeight;
};