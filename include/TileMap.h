#include <string>
#include <vector>
#include <memory>
class GameObject;
class TileSet;
class TileMap {
    public:
        TileMap(GameObject& associated, std::string file, TileSet* tileSet);
        void Load (std::string file);
        void SetTileSet(TileSet* tileSet);
        int& At (int x, int y, int z = 0);
        void Render();
        void Render(int layer);
        int GetWidth();
        int GetHeight();
        int GetDepth();
    private:
        std::vector<int> tileMatrix;
        std::unique_ptr<TileSet> tileSet;
        int mapWidth;
        int mapHeight;
        int mapDepth;
};