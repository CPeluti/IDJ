#include "TileMap.h"
#include <fstream>
#include <memory>
#include <string>
#include <iostream>

TileMap::TileMap(GameObject& associated, std::string file, TileSet* tileSet): tileSet(tileSet){
    Load(file);
}

void TileMap::Load(std::string file){
    std::string line;
    std::ifstream map (file);
    int counter = 0;
    if(map.is_open()){
        while(std::getline(map, line,',')){
            int number = std::stoi(line);
            switch(counter){
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
        }
    }
}

void TileMap::SetTileSet(TileSet* ts){
    tileSet.release();
    tileSet.reset(ts);
}

int& TileMap::At(int x, int y, int z = 0){
    if (x < mapWidth && y < mapHeight && z < mapDepth){
        int pos = x*y + z * mapHeight* mapWidth;
    } else {
        std::cout << "Coordinates exceed tilemap boundary" << std::endl;
    }
}