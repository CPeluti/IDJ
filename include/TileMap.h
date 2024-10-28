#pragma once
#include <string>
#include <vector>
#include <memory>
#include "TileSet.h"
#include "Component.h"
class GameObject;
class TileMap : public Component {
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
    private:
        std::vector<int> tileMatrix;
        std::unique_ptr<TileSet> tileSet;
        int mapWidth;
        int mapHeight;
        int mapDepth;
};