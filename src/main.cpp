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
    auto data = vector<State>(100, State(parameterList, 0));
    auto queue = std::queue<State*>();
    for(auto &x : data) queue.push(&x);

    
    auto threadList = vector<thread>(parameterList["threadCount"]);
    auto threadFunction = [](State* s, int threadId){
        auto threadStartTime = std::chrono::system_clock::now();
        s->simulate();

        auto threadEndTime = std::chrono::system_clock::now();
        std::cout << "Thread " << threadId << " finished computing in " << 
        std::chrono::duration_cast<std::chrono::microseconds>(threadEndTime - threadStartTime).count()/1e6 << 
        " seconds" << std::endl;
        s->outputData(outputPath, threadId);
    };

    for(int i = 0; i < parameterList["threadCount"]; i++){
        threadList[i] = thread(threadFunction, &data[i], i);
        std::cout << "Opening thead " << i << ", Id: " << threadList[i].get_id() << "\n";
    }
    for(auto &x : threadList){
        x.join();
        std::cout << "Closed Thread " << x.get_id() << "\n";
    }
    auto programEndTime = std::chrono::system_clock::now();
    std::cout << "Simulation Finished in " << std::chrono::duration_cast<std::chrono::microseconds>(programEndTime - programStartTime).count()/1e6 << " seconds" << std::endl;
}