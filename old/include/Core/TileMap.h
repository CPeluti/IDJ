#pragma once
#include <string>
#include <vector>
#include <memory>
#include "TileSet.h"
#include "Component.h"
#include "Collider.h"


class GameObject;
class TileMap : public Component, public std::enable_shared_from_this<TileMap> {
public:
    TileMap(GameObject& associated, std::string file, TileSet* tileSet);
    void Load (std::string file);
    void SetTileSet(TileSet* tileSet);
    int& At (int x, int y, int z = 0);
    void Render();
    void RenderLayer(int layer);
    int GetWidth();
    int GetHeight();
    int GetDepth();
    bool Is(std::string type);
    void Update(float dt);
    void Start();
    inline Vec2 GetPosInTileMap(Vec2 absolutePos) const
    {
        Vec2 pos = absolutePos;
        pos.x /= tileSet->GetTileWidth();
        pos.y /= tileSet->GetTileHeight();
        return pos;
    }

    inline Rect* GetTileMetadata(int code) 
    {
        return tileSet->getColliderMetaData(code);
    }

private:
    std::vector<std::weak_ptr<Collider>> m_colliders;
    std::vector<int> tileMatrix;
    std::unique_ptr<TileSet> tileSet;
    int mapWidth;
    int mapHeight;
    int mapDepth;

};