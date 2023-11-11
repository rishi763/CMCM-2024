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


void State::timeStep(){

}


void State::simulate(){
    initVineyard();

    // calculate change in bug population

    // calculate plant health deterioration from sprays
}

string state::outputMatrix(){
    
}


json State::serializeData(){
    json j;

    j["OutputTag1"] = 55; //Keep doing this basically.

    return j;
}

void State::outputData(fs::path outputPath, int threadId){
    json j = serializeData();
    outputPath /= fs::path("simTrial" + std::to_string(threadId) + ".json");
    std::ofstream fout(outputPath);
    fout << j.dump();
    fout.close();
}