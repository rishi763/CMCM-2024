#include "pch.h"

struct State
{
    json parameterList;
    State(const json& _parameterList){
        parameterList = _parameterList;
    }
    
    enum bugType
    {
        femaleEgg,
        femaleChild,
        femaleAdult,
        maleEgg,
        maleChild,
        maleAdult
    };

    struct plant
    {
        array<int, 6> bugCount{0};
        int vineHealth = 100;                                     // 0 to 100
        int grapeClusters = 40;
        // parameterList["grapeClusterAverage"]; // on average a healthy grape vine
    };

    int currentTime = 1;
    int currentMonth = 1;

    vector<vector<plant>> vineyard;
    vector<vector<double>> eggDistribution; 

    void initVineyard();
    void simulate();
    void timeStep();
    void updateEnvironment();
    // void searching();
    void exterminate();
    void updateBug();

    void outputData(fs::path outputPath, int threadId);
    json serializeData();
    string outputMatrix();
    // template<typenameT> void outputMatrix(json& j, int id, string matrixNameLabel);

    // Helper functions
    static double rand0to1()
    {
        return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    }

    static std::pair<int, int> pickLocation(vector<vector<double>> distribution)
    {
        double randomNumber = rand0to1();
        // for (size_t row = 0; row < distribution.size(); row++){
        //     for(size_t col= 0; col <distribution[0].size();col++){
        //         if(randomNumber<distribution[row][col]){
        //             return {row, col};
        //         }
        //         randomNumber-=distribution[row][col];
        //     }
        // }
        return {distribution.size()-1, distribution[0].size()-1};
    }
};
