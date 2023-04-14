#include <chrono>
#include <random>
#include "raylib.h"
#include <iostream>
#include <vector>
#include <array>
#include <fstream>


//---------------------------------------------------------------------------------------------------------------------------------

std::mt19937 CreateGeneratorWithTimeSeed();
float RandomFloat(float min, float max, std::mt19937& rng);
int RandomInt(int min, int max, std::mt19937& rng);

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
        else if(type == OIL){
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

    double update(double rate){
        double successess = 0;

        for(int i = 0; i < rate; i++){
            if(swapper()){
                successess++;
            }
        }

        return double(successess / rate);
    }

    bool swapper(){
        int i = RandomInt(0, columns - 1, rng);
        int j = RandomInt(0, rows - 1, rng);
        int newX = RandomInt(0, columns - 1, rng);
        int newY = RandomInt(0, rows - 1, rng);
        ParticleType currentLocationType = particles[i][j].type;
        ParticleType newLocationType = particles[newX][newY].type;

        int currentFavorability = checkNeighbors(i, j, currentLocationType);
        int newFavorability = checkNeighbors(newX, newY, currentLocationType);

        if(newFavorability >= currentFavorability){
            particles[i][j] = Particle(newLocationType);
            particles[newX][newY] = Particle(currentLocationType);
            return true;
        }
        else{
            return false;
        }
    }

    void randomize(){
        for(int i = 0; i < columns; i++){
            for(int j = 0; j < rows; j++){
                ParticleType randType;
                if(RandomInt(0,100, rng) >= 50){
                    randType = WATER;
                }
                else{
                    randType = OIL;
                }
                particles[i][j] = Particle(randType);
            }
        }
    }

    int checkNeighbors(int x, int y, ParticleType currentType){
        int numSameNeighbors = 0, numDifferentNeighbors = 0;

        if(y - 1 >= 0){
            if(particles[x][y -1].type == currentType){
                numSameNeighbors += 1;
            }
            else{
                numDifferentNeighbors += 1;
            }
        }

        if(y +1 < rows){
            if(particles[x][y +1].type == currentType){
                numSameNeighbors += 1;
            }
            else{
                numDifferentNeighbors += 1;
            }
        }

        if(x -1 >= 0){
            if(particles[x -1][y].type == currentType){
                numSameNeighbors += 1;
            }
            else{
                numDifferentNeighbors += 1;
            }
        }

        if(x +1 < columns){
            if(particles[x +1][y].type == currentType){
                numSameNeighbors += 1;
            }
            else{
                numDifferentNeighbors += 1;
            }
        }

        return numSameNeighbors - numDifferentNeighbors;
    }

    double findInterfaceLength() {
        double length = 0.0;
        bool hasInterface;

        // check horizontal interfaces
        for (int i = 0; i < columns; i++) {
            for (int j = 0; j < rows - 1; j++) {
                hasInterface = particles[i][j].type != particles[i][j+1].type;
                if (hasInterface) {
                    length += cellHeight;
                }
            }
        }

        // check vertical interfaces
        for (int i = 0; i < columns - 1; i++) {
            for (int j = 0; j < rows; j++) {
                hasInterface = particles[i][j].type != particles[i+1][j].type;
                if (hasInterface) {
                    length += cellWidth;
                }
            }
        }

        return length;
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

int RandomInt(int min, int max, std::mt19937& rng){
    std::uniform_int_distribution<int> dist(min, max);
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

void createLineGraph(Rectangle rect, float x_axis[], float y_axis[], int num_points, Color color)
{
    // Set the axis labels and font size
    const char* x_label = "X-Axis";
    const char* y_label = "Y-Axis";
    int font_size = 10;
    
    // Calculate the scaling factor for the x and y axes
    float x_scale = rect.width / (x_axis[num_points-1] - x_axis[0]);
    float y_scale = rect.height / (y_axis[num_points-1] - y_axis[0]);
    
    // Create a vector to store the points as Vector2
    std::vector<Vector2> points;
    for (int i = 0; i < num_points; i++)
    {
        points.push_back(Vector2{(x_axis[i] - x_axis[0]) * x_scale + rect.x, rect.y + rect.height - (y_axis[i] - y_axis[0]) * y_scale});
    }
    
    // Draw the axes
    DrawLine(rect.x, rect.y + rect.height, rect.x + rect.width, rect.y + rect.height, color);
    DrawLine(rect.x, rect.y + rect.height, rect.x, rect.y, color);
    
    // Draw the x and y axis labels
    DrawText(x_label, rect.x + rect.width/2 - MeasureText(x_label, font_size)/2, rect.y + rect.height + font_size, font_size, color);
    DrawText(y_label, rect.x - MeasureText(y_label, font_size) - font_size, rect.y + rect.height/2 - font_size/2, font_size, color);
    
    // Draw the line graph
    for (int i = 0; i < num_points-1; i++)
    {
        DrawLineEx(points[i], points[i+1], font_size, color);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------

int main() {

    initialize();

    float squareGridSize = screenHeight;
    int numPixels = 100;
    int pixelSize = squareGridSize / numPixels;

    Grid grid = createRandomGrid(numPixels, pixelSize, pixelSize, {0, 0, squareGridSize, squareGridSize});

    std::ofstream outFile;
    outFile.open("successRate_vs_frame.csv");
    outFile.close();

    //graph data
    std::vector<double> successRate;
    std::vector<int> frame;

    for(int frame = 0; !WindowShouldClose(); frame++) {

        BeginDrawing();

        drawBackground();
        grid.draw();

        EndDrawing();

        std::ofstream outFile;
        outFile.open("successRate_vs_frame.csv", std::ofstream::app);
        outFile << frame << ", " << grid.update(10000) << ", " << grid.findInterfaceLength() / pixelSize << "\n";
        outFile.close();
    }

    CloseWindow();

    return 0;
}
