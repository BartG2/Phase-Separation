#include <chrono>
#include <random>
#include "raylib.h"
#include <iostream>
#include <vector>
#include <array>


//---------------------------------------------------------------------------------------------------------------------------------

std::mt19937 CreateGeneratorWithTimeSeed();
float RandomFloat(float min, float max, std::mt19937& rng);

//---------------------------------------------------------------------------------------------------------------------------------


const int screenWidth = 2440, screenHeight = 1368;

std::mt19937 rng = CreateGeneratorWithTimeSeed();

//---------------------------------------------------------------------------------------------------------------------------------

enum ParticleType{
    OIL,
    WATER
};

class Particle{
    public:
    ParticleType type;
    Color color;
    constexpr static float radius = 4;

    Particle(){

        if(RandomFloat(0,1, rng) < 0.50){
            type = WATER;
            color = DARKBLUE;
        }
        else{
            type = OIL;
            color = GOLD;
        }
    }

    Particle(ParticleType t){
        type = t;
        if(type == WATER){
            color = DARKBLUE;
        }
        if(type == OIL){
            color = GOLD;
        }
    }
};

class Grid{
    public:
    int rows, columns;
    std::vector<std::vector<Particle>> particles;
    float cellWidth, cellHeight;
    Rectangle size;

    Grid(int rows, int columns, std::vector<std::vector<Particle>> particles, float cellWidth, float cellHeight, Rectangle size) : 
        rows(rows), columns(columns), particles(particles), cellWidth(cellWidth), cellHeight(cellHeight), size(size)
    {}

    void draw(){
        drawGridOutline();
        float padding = 0.5;
        if(columns == particles.size() and rows == particles[0].size()){
            for(int i = 0; i < columns; i++){
                for(int j = 0; j < rows; j++){
                    DrawRectangleV({i*cellWidth + padding, j*cellHeight + padding}, {cellWidth - 2*padding, cellHeight - 2*padding}, particles[i][j].color);
                }
            }
        }
        else{
            std::cout << "ERROR" << std::endl;
        }
    }

    void drawGridOutline(){
        //std::cout << numColumns << "\n\n";
        for(int i = 0; i < columns; i++){
            for(int j = 0; j < rows; j++){
                DrawRectangleLinesEx({i*cellWidth, j*cellHeight, cellWidth, cellHeight}, 1, ORANGE);
            }
        }
    }
};

//---------------------------------------------------------------------------------------------------------------------------------

std::mt19937 CreateGeneratorWithTimeSeed() {
    // Get the current time in nanoseconds
    auto now = std::chrono::high_resolution_clock::now();
    auto nanos = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();

    // Create a new mt19937 generator and seed it with the current time in nanoseconds
    std::mt19937 gen(static_cast<unsigned int>(nanos));
    return gen;
}

float RandomFloat(float min, float max, std::mt19937& rng) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}


//---------------------------------------------------------------------------------------------------------------------------------

void initialize(){
    InitWindow(screenWidth, screenHeight, "Simulation");
    SetTargetFPS(100);
}

void drawBackground(){
    ClearBackground(GRAY);
    DrawFPS(screenWidth - 40, 20);
}

Grid createRandomGrid(int numPixels, float cellWidth, float cellHeight, Rectangle size){
    int numRows = numPixels, numColumns = numPixels;
    std::vector<std::vector<Particle>> particles(numRows, std::vector<Particle>(numColumns));

    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numColumns; j++){
            particles[i][j] = Particle();
        }
    }

    return Grid(numRows, numColumns, particles, cellWidth, cellHeight, size);
}

//---------------------------------------------------------------------------------------------------------------------------------

int main() {

    initialize();

    float squareGridSize = screenHeight;
    int numPixels = 144;
    int pixelSize = squareGridSize / numPixels;

    Grid grid = createRandomGrid(numPixels, pixelSize, pixelSize, {0, 0, squareGridSize, squareGridSize});


    //Grid grid = createRandomGrid(squareGridSize,int(squareGridSize*1.777));

    while (!WindowShouldClose()) {

        BeginDrawing();

        drawBackground();
        grid.draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
