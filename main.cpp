#include <chrono>
#include <random>
#include "raylib.h"
#include <iostream>
#include <vector>
#include <array>
#include <fstream>


/*
Graph interface length vs time for everything. Include Pictures.
Graph time to minimum interface vs grid size.
Graph time to minimum interface vs percent water. Describe qualitatively.
Edge case where not enough water to form 1 strip along edge.

*/


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

class DoubleGrid{
    public:
    int rows, columns;
    std::vector<std::vector<Particle>> topParticles;
    std::vector<std::vector<Particle>> bottomParticles;
    float cellWidth, cellHeight;
    Rectangle size;

    DoubleGrid(int rows, int columns, std::vector<std::vector<Particle>> top_particles, std::vector<std::vector<Particle>> bottom_particles, float cellWidth, float cellHeight, Rectangle size) :
        rows(rows), columns(columns), topParticles(top_particles), bottomParticles(bottom_particles), cellWidth(cellWidth), cellHeight(cellHeight), size(size)
    {}

    void draw() {
        drawGridOutline();
        float padding = 0.5;
        if (columns == topParticles.size() and rows == topParticles[0].size() and columns == bottomParticles.size() and rows == bottomParticles[0].size()) {
            // draw bottomParticles as the grid
            for (int i = 0; i < columns; i++) {
                for (int j = 0; j < rows; j++) {
                    DrawRectangleV({i*cellWidth + padding, j*cellHeight + padding}, {cellWidth - 2*padding, cellHeight - 2*padding}, bottomParticles[i][j].color);
                }
            }
            // draw topParticles as circles of radius 1/3 cellWidth over the top of the grid
            float radius = cellWidth / 3.0;
            for (int i = 0; i < columns; i++) {
                for (int j = 0; j < rows; j++) {
                    DrawCircleV({i*cellWidth + cellWidth / 2.0f, j*cellHeight + cellHeight / 2.0f}, radius, topParticles[i][j].color);
                    DrawCircleLines(i*cellWidth + cellWidth / 2.0f, j*cellHeight + cellHeight / 2.0f, radius, BLACK);
                }
            }
        } else {
            std::cout << "ERROR" << std::endl;
        }
    }

    void drawGridOutline() {
        for (int i = 0; i < columns; i++) {
            for (int j = 0; j < rows; j++) {
                DrawRectangleLinesEx({i*cellWidth, j*cellHeight, cellWidth, cellHeight}, 1, ORANGE);
                DrawRectangleLinesEx({i*cellWidth, j*cellHeight + cellHeight, cellWidth, cellHeight}, 1, ORANGE);
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
        ParticleType currentLocationType = topParticles[i][j].type;
        ParticleType newLocationType = topParticles[newX][newY].type;

        if(currentLocationType != newLocationType){
            int currentFavorability = checkNeighbors(i, j, currentLocationType);
            int newFavorability = checkNeighbors(newX, newY, currentLocationType);

            if(newFavorability >= currentFavorability){
                topParticles[i][j] = Particle(newLocationType);
                topParticles[newX][newY] = Particle(currentLocationType);
                return true;
            }
            else{
                return false;
            }
        }
        return false;
    }

    int checkNeighbors(int x, int y, ParticleType currentType){
        int numSameNeighbors = 0, numDifferentNeighbors = 0;

        // Check topParticles
        if(y - 1 >= 0){
            if(topParticles[x][y -1].type == currentType){
                numSameNeighbors += 1;
            }
            else{
                numDifferentNeighbors += 1;
            }
        }

        if(y +1 < rows){
            if(topParticles[x][y +1].type == currentType){
                numSameNeighbors += 1;
            }
            else{
                numDifferentNeighbors += 1;
            }
        }

        if(x -1 >= 0){
            if(topParticles[x -1][y].type == currentType){
                numSameNeighbors += 1;
            }
            else{
                numDifferentNeighbors += 1;
            }
        }

        if(x +1 < columns){
            if(topParticles[x +1][y].type == currentType){
                numSameNeighbors += 1;
            }
            else{
                numDifferentNeighbors += 1;
            }
        }

        // Check bottomParticles
        if(y - 1 >= 0){
            if(bottomParticles[x][y -1].type == currentType){
                numSameNeighbors += 1;
            }
            else{
                numDifferentNeighbors += 1;
            }
        }

        if(y +1 < rows){
            if(bottomParticles[x][y +1].type == currentType){
                numSameNeighbors += 1;
            }
            else{
                numDifferentNeighbors += 1;
            }
        }

        if(x -1 >= 0){
            if(bottomParticles[x -1][y].type == currentType){
                numSameNeighbors += 1;
            }
            else{
                numDifferentNeighbors += 1;
            }
        }

        if(x +1 < columns){
            if(bottomParticles[x +1][y].type == currentType){
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
                hasInterface = topParticles[i][j].type != topParticles[i][j+1].type;
                if (hasInterface) {
                    length += cellHeight;
                }
            }
        }

        // check vertical interfaces
        for (int i = 0; i < columns - 1; i++) {
            for (int j = 0; j < rows; j++) {
                hasInterface = topParticles[i][j].type != topParticles[i+1][j].type;
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

/*Grid createRandomGrid(int numPixels, float cellWidth, float cellHeight, Rectangle size){
    int numRows = numPixels, numColumns = numPixels;
    std::vector<std::vector<Particle>> particles(numRows, std::vector<Particle>(numColumns));

    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numColumns; j++){
            particles[i][j] = Particle();
        }
    }

    return Grid(numRows, numColumns, particles, cellWidth, cellHeight, size);
}*/

DoubleGrid createRandomDoubleGrid(int rows, int columns, Rectangle size, float cellWidth, float cellHeight){
    std::vector<std::vector<Particle>> topParticles(columns, std::vector<Particle>(rows));
    std::vector<std::vector<Particle>> bottomParticles(columns, std::vector<Particle>(rows));
    for(int i = 0; i < columns; i++){
        for(int j = 0; j < rows; j++){
            topParticles[i][j] = Particle();
            bottomParticles[i][j] = Particle();
        }
    }
    return DoubleGrid(rows, columns, topParticles, bottomParticles, cellWidth, cellHeight, size);
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

/*int run(int numPixels){
    initialize();

    float squareGridSize = screenHeight;
    int pixelSize = squareGridSize / numPixels;

    Grid grid = createRandomGrid(numPixels, pixelSize, pixelSize, {0, 0, squareGridSize, squareGridSize});

    std::ofstream outFile;
    outFile.open("successRate_vs_frame.csv");
    outFile.close();

    //graph data
    std::vector<double> successRate;
    std::vector<int> frame;
    int minLength = 100000;
    int timeStamp = 0;

    for(int frame = 0; !WindowShouldClose(); frame++) {

        BeginDrawing();

        drawBackground();
        grid.draw();

        EndDrawing();

        int intefaceLength = grid.findInterfaceLength() / pixelSize;

        std::ofstream outFile;
        outFile.open("successRate_vs_frame.csv", std::ofstream::app);
        outFile << frame << ", " << grid.update(200000) << ", " << intefaceLength << "\n";
        outFile.close();

        if(intefaceLength < minLength){
            minLength  = intefaceLength;
            timeStamp = frame;
        }

        if(frame > 100000 or frame - timeStamp > 500 * numPixels/10){
            break;
        }
    }

    CloseWindow();
    return minLength;
}*/

int runDouble(int numPixels){
    initialize();

    float squareGridSize = screenHeight;
    int pixelSize = squareGridSize / numPixels;

    //Grid grid = createRandomGrid(numPixels, pixelSize, pixelSize, {0, 0, squareGridSize, squareGridSize});
    DoubleGrid grid = createRandomDoubleGrid(numPixels, numPixels, {0, 0, squareGridSize, squareGridSize}, pixelSize, pixelSize);

    std::ofstream outFile;
    outFile.open("successRate_vs_frame.csv");
    outFile.close();

    //graph data
    std::vector<double> successRate;
    std::vector<int> frame;
    int minLength = 100000;
    int timeStamp = 0;

    for(int frame = 0; !WindowShouldClose(); frame++) {

        BeginDrawing();

        drawBackground();
        grid.draw();

        EndDrawing();

        int intefaceLength = grid.findInterfaceLength() / pixelSize;

        std::ofstream outFile;
        outFile.open("successRate_vs_frame.csv", std::ofstream::app);
        outFile << frame << ", " << grid.update(200000) << ", " << intefaceLength << "\n";
        outFile.close();

        if(intefaceLength < minLength){
            minLength  = intefaceLength;
            timeStamp = frame;
        }

        if(frame > 100000 or frame - timeStamp > 500 * numPixels/10){
            break;
        }
    }

    CloseWindow();
    return minLength;
}

//---------------------------------------------------------------------------------------------------------------------------------

int main() {
    int maxSize = 16;
    int startSize = 10;
    int gridSizeIncrement = 5;
    int repeatRuns = 2;

    std::ofstream file;
    file.open("test_data.csv");
    file.close();

    for(int i = startSize; i <= maxSize; i+= gridSizeIncrement){
        file.open("test_data.csv", std::ofstream::app);
        file << i << ", " << runDouble(i);
        for(int j = 0; j < repeatRuns; j++){
            file << ", " << runDouble(i);
        }
        file << "\n";
        file.close();
    }

    return 0;
}
