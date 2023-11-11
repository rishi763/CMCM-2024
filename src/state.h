#include "pch.h"

struct State
{
    static json parameterList;

    enum bugType {
        femaleEgg,
        femaleChild,
        femaleAdult,
        maleEgg,
        maleChild,
        maleAdult
    };

    struct plant
    {
        int weight = 1;
        array<int, 6> bugCount{0};
        int vineHealth = 100; //0 to 100
        int eggCount = 0;
        int grapeClusters = parameterList["grapeClusterAverage"];//on average a healthy grape vine  
    };

    vector<vector<plant>> vineyard;
    State(){}

    void initVineyard();
    void simulate();
    void outputData(fs::path path, int threadId);
    void timeStep();
    string outputMatrix();

    json serializeData();

    // Helper functions
    static float rand0to1(){
        return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }
};


