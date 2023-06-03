#include <iostream>
#include <Windows.h>
#include "quadtree.h"

// This might be the client rect of the game window
RECT ClientRect = { 0, 0, 1366, 768 };

MapCoordinate ScreenToWorld(int MouseX, int MouseY, MapCoordinate& CameraTransform)
{
    MapCoordinate result = CameraTransform;

    // Calculate the world space coordinates based on the mouse position and camera transform
    result.Pos.x = static_cast<float>(MouseX) + CameraTransform.Pos.x - static_cast<float>(ClientRect.right) / 2.0f;
    result.Pos.y = static_cast<float>(MouseY) + CameraTransform.Pos.y - static_cast<float>(ClientRect.bottom) / 2.0f;

    // Adjust the grid coordinates if necessary
    while (result.Pos.x < 0.0f)
    {
        result.Pos.x += 160.0f;
        result.Grid.x--;
    }
    while (result.Pos.x > 160.0f)
    {
        result.Pos.x -= 160.0f;
        result.Grid.x++;
    }

    while (result.Pos.y < 0.0f)
    {
        result.Pos.y += 160.0f;
        result.Grid.y--;
    }
    while (result.Pos.y > 160.0f)
    {
        result.Pos.y -= 160.0f;
        result.Grid.y++;
    }

    return result;
}

int main()
{
    unsigned int mouseX, mouseY;
    std::cout << "Enter mouse coordinates (X and Y): ";
    std::cin >> mouseX >> mouseY;

    MapCoordinate cameraPos;
    std::cout << "Enter camera position (GridX, GridY, PosX, PosY): ";
    std::cin >> cameraPos.Grid.x >> cameraPos.Grid.y >> cameraPos.Pos.x >> cameraPos.Pos.y;
    
    MapCoordinate gameCoord = ScreenToWorld(mouseX, mouseY, cameraPos);

    std::cout << "Game coordinates:\n";
    std::cout << "GridX: " << static_cast<int>(gameCoord.Grid.x) << std::endl;
    std::cout << "GridY: " << static_cast<int>(gameCoord.Grid.y) << std::endl;
    std::cout << "Pos: [" << gameCoord.Pos.x << ", " << gameCoord.Pos.y << "]\n";

	Quadtree Qt({0, 0, 255, 255}, 1);
    Qt.Insert(cameraPos);
    Qt.Insert(gameCoord);

    std::vector<const MapCoordinate*> Coords;
    Qt.CollectCoords({ {0.0f, 0.0f}, {118, 118} }, Coords);
    
    return 0;
}