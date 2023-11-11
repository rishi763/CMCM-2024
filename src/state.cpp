#include "state.h"


void State::initVineyard()
{
    
    vineyard = vector<vector<plant>>(parameterList["vineyardColumns"], vector<plant>(parameterList["vineyardColumnSize"], plant(
        // Anything we want to pass into the plant
    )));


    for(int row = 0; row < vineyard.size(); row++){
        for(int col = 0; col < vineyard[row].size(); col++){
            
        }
    }
}



void State::simulate(){
    initVineyard();

    // calculate change in bug population

    // calculate plant health deterioration from sprays
}


void State::outputData(fs::path outputPath, int threadId){
    json j;
    // j["test1"] = foo;
    // j["test2"] = parameterList["testInput"];
    outputPath /= fs::path("simTrial" + std::to_string(threadId) + ".json");
    std::ofstream fout(outputPath);
    fout << j.dump();
    fout.close();
}