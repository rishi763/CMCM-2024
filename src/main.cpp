#include "pch.h"
#include "state.h"

string runName = "testing";
const string jsonName = "data.json";
fs::path outputPath = std::filesystem::current_path() /= fs::path("data") /= fs::path(runName);

int main(){ 
    
    auto programStartTime = std::chrono::system_clock::now();
    srand(std::time(NULL));
    
    std::ifstream f(jsonName);
    json parameterList = json::parse(f);

    fs::create_directory(fs::path("data")/=fs::path(runName));
    
    GnuplotPipe gp;
    


    // State::parameterList = parameterList;
    // State::params
    auto data = vector<State>();
    for(int _nymphThreshold = 20; _nymphThreshold < 90; _nymphThreshold++){
    for(int _adultThreshold = 5; _adultThreshold < 20; _adultThreshold++){
    for(int _harvestDeadline = 315; _harvestDeadline < 316; _harvestDeadline++){
    for(int _chosenPesticide = 0; _chosenPesticide < 3; _chosenPesticide++){
    for(int _costFunctionID = 0; _costFunctionID < 2; _costFunctionID++){
    for(int _stratID = 0; _stratID < 1; _stratID++){
        data.push_back(State(parameterList, State::params{
            _nymphThreshold, _adultThreshold, _harvestDeadline, _chosenPesticide, _costFunctionID, _stratID
        }));
    }}}}}}
   



    State* first = &data.front();
    // Goihng to generate all the permutations here
    // for(every state)
    
    std::mutex queueMutex;
    auto allocationQueue = std::queue<State*>();
    for(auto &x : data) allocationQueue.push(&x);

    auto threadList = vector<thread>(parameterList["threadCount"]);
    auto threadFunction = [&](int threadId){
        while(true){
            queueMutex.lock();
            if(allocationQueue.empty()){
                queueMutex.unlock();
                break;
            }
            State* s = allocationQueue.front();
            allocationQueue.pop();
            queueMutex.unlock();

            auto threadStartTime = std::chrono::system_clock::now();
            s->simulate();
            auto threadEndTime = std::chrono::system_clock::now();
            std::cout << "Thread " << threadId << " finished computing in " << 
            std::chrono::duration_cast<std::chrono::microseconds>(threadEndTime - threadStartTime).count()/1e6 << 
            " seconds" << std::endl;
            s->outputData(outputPath, std::distance(first, s));
        }
    };

    for(int i = 0; i < parameterList["threadCount"]; i++){
        threadList[i] = thread(threadFunction, i);
        std::cout << "Opening thead " << i << ", Id: " << threadList[i].get_id() << "\n";
    }

    for(auto &x : threadList){
        x.join();
        std::cout << "Closed Thread " << x.get_id() << "\n";
    }
    
    auto programEndTime = std::chrono::system_clock::now();
    std::cout << "Simulation Finished in " << std::chrono::duration_cast<std::chrono::microseconds>(programEndTime - programStartTime).count()/1e6 << " seconds" << std::endl;
}