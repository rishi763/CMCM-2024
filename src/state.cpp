#include "state.h"

// \frac{e^{2.2\max\left(\left|\frac{x}{50}\right|,\left|\frac{y}{20}\right|\right)}}{2149.44}
// ^I'm cumming



void State::initVineyard()
{
    int vineyardColumns = parameterList["vineyardColumns"];
    int vineyardColumnSize = parameterList["vineyardColumnSize"];
    int initialSLFPopulation = parameterList["initialSLFPopulation"];
    vineyard = vector<vector<plant>>(vineyardColumns, vector<plant>(vineyardColumnSize, plant(
        // Anything we want to pass into the plant
    )));
    eggDistribution = vector<vector<double>>(vineyardColumns, vector<double>(vineyardColumnSize, 0.0));

    for(int row = 0; row < vineyard.size(); row++){
        for(int col = 0; col < vineyard[0].size(); col++){
            double topExponent = 2.2*std::max(
                abs(row-(vineyardColumns-1)/2.0)/(1.0*vineyardColumns),
                abs(col-(vineyardColumnSize-1)/2.0)/(1.0*vineyardColumnSize)
            );
            eggDistribution[vineyardColumns][vineyardColumnSize]= std::exp(topExponent)/2149.44;
        }
    }
    for(int i=0;i<initialSLFPopulation;i++){
        std::pair<int,int> location = pickLocation(eggDistribution);
        std::cout<<location.first<<location.second<<std::endl;
    }
}

void State::updateEnvironment(){
    currentMonth  = (currentTime % 360) / 30;
}

void State::exterminate(){
    
}

void State::updateBug(){

}

void State::timeStep(){
    updateEnvironment();
    // searching();
    exterminate();
    updateBug();
}

void State::simulate(){
    std::cout << "Initalizing" << std::endl;
    initVineyard();
    std::cout << "Finihsed Initalizig" << std::endl;

    // calculate change in bug population
    while (currentTime <= parameterList["runTime"]){
        std::cout << "Stepping" << std::endl;
        timeStep();
        currentTime += (int)parameterList["timeStep"];
    }
    std::cout << "Finished Simulating on this Thread" << std::endl;
}

// template<typename T> void State::outputMatrix(json& j, int id, string matrixNameLabel){
//     vector<vector<T>> outputVector(rows);
//     for(auto &x : )
//     j[matrixNameLabel] = outputVector;
// }


json State::serializeData(){
    json j;
    j["x"] = parameterList["vineyardColumns"];
    j["y"] = parameterList["vineyardColumnSize"];
    std::stringstream strStream;
    for(int x=0;x<j["x"];x++){
        for(int y=0;y<j["y"];y++){
            strStream << std::to_string(eggDistribution[x][y]) + ",";
        }
    }    
    j["weightMap"] = strStream.str(); //Keep doing this basically.
    return j;
}

void State::outputData(fs::path outputPath, int threadId){
    json j = serializeData();
    outputPath /= fs::path("simTrial" + std::to_string(threadId) + ".json");
    std::ofstream fout(outputPath);
    fout << j.dump();
    fout.close();
}