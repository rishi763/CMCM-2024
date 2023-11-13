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
    // Goihng to generate all the permutations here
    
    std::mutex queueMutex, printMutex;
    auto allocationQueue = std::queue<State*>();
    auto printQueue = std::queue<State*>();
    for(auto &x : data) allocationQueue.push(&x);

    auto threadList = vector<thread>(parameterList["threadCount"]);
    auto threadFunction = [&](int threadId){
        while(!allocationQueue.empty()){
            queueMutex.lock();
            if(!allocationQueue.empty()){

                queueMutex.lock();
                State* s = allocationQueue.front();
                allocationQueue.pop();
                queueMutex.unlock();

                auto threadStartTime = std::chrono::system_clock::now();
                s->simulate();
                auto threadEndTime = std::chrono::system_clock::now();
                std::cout << "Thread " << threadId << " finished computing in " << 
                std::chrono::duration_cast<std::chrono::microseconds>(threadEndTime - threadStartTime).count()/1e6 << 
                " seconds" << std::endl;

                printMutex.lock();
                printQueue.push(s);
                printMutex.unlock();
            }
        }
    };



    for(int i = 0; i < parameterList["threadCount"]; i++){
        threadList[i] = thread(threadFunction, i);
        std::cout << "Opening thead " << i << ", Id: " << threadList[i].get_id() << "\n";
    }


    // auto diskThread = thread([&](){
    //     while(!printQueue.empty()){
    //         printMutex.lock();
    //         if(!printQueue.empty()){

    //             printMutex.lock();
    //             State* s = printQueue.front();
    //             printQueue.pop();
    //             printMutex.unlock();
    //             s->outputData(outputPath, 0);
    //         }
    //     }
    // });

    for(auto &x : threadList){
        x.join();
        std::cout << "Closed Thread " << x.get_id() << "\n";
    }
    
    auto programEndTime = std::chrono::system_clock::now();
    std::cout << "Simulation Finished in " << std::chrono::duration_cast<std::chrono::microseconds>(programEndTime - programStartTime).count()/1e6 << " seconds" << std::endl;
}