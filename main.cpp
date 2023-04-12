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

    Grid(std::vector<std::vector<Particle>> p, int numRows, int NumColumns){
        rows = numRows;
        columns = NumColumns;
        particles = p;
        cellWidth = screenWidth / NumColumns;
        cellHeight = screenHeight / numRows;
    }

    void draw(){
        for(int i = 0; i < particles.size(); i++){
            for(int j = 0; j < particles[i].size(); j++){
                //DrawCircleV({j*cellWidth, i*cellHeight}, particles[i][j].radius, particles[i][j].color);
                DrawRectangleV({j*cellWidth + cellWidth / 2.0, i*cellHeight + cellHeight / 2.0}, {particles[i][j].radius * 2, particles[i][j].radius * 2}, particles[i][j].color);
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
    DrawFPS(20,20);
}

Grid createRandomGrid(int rows, int columns){
    int numRows = rows, numColumns = columns;
    std::vector<std::vector<Particle>> particles(numRows, std::vector<Particle>(numColumns));

    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numColumns; j++){
            particles[i][j] = Particle();
        }
    }

    return Grid(particles, numRows, numColumns);
}

//---------------------------------------------------------------------------------------------------------------------------------

int main() {

    initialize();

    int squareGridSize = 50;
    Grid grid = createRandomGrid(squareGridSize,int(squareGridSize*1.777));

    while (!WindowShouldClose()) {

        BeginDrawing();

        drawBackground();
        grid.draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
