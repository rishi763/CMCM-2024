#include "pch.h"
#include "state.h"

string runName = "testing";
const string jsonName = "data.json";
fs::path outputPath = std::filesystem::current_path() /= fs::path("data") /= fs::path(runName);

inline bool operator<(const State &s1, const State &s2)
{
    return s1.costAnalysis > s2.costAnalysis;
}

int main()
{
    auto programStartTime = std::chrono::system_clock::now();
    srand(std::time(NULL));

    std::ifstream f(jsonName);
    json parameterList = json::parse(f);

    fs::create_directory(fs::path("data") /= fs::path(runName));

    vector<vector<int>> Thresholds = parameterList["iterateOverParam"];
    auto data = vector<State>();
    
    // Optimization Function
    if(parameterList["runOptimization"]){
        json outputJson;
        vector<vector<vector<pair<int, float>>>> v;

        for(int r = 0; r < parameterList["trialsForOptimizatoinRun"]; r++){
            std::cout << "Next r trial " << r << "\n"; 
            vector<vector<pair<int, float>>> vv;
            for(int i = 0; i < Thresholds.size(); i++){
                vector<pair<int, float>> vvv;
                for(int j = Thresholds[i][0]; j < Thresholds[i][1]; j+= Thresholds[i][2]){
                    std::cout << "Starting Interval" << j << "\n";
                    vector<int> dataVec = {Thresholds[0][3], Thresholds[1][3], Thresholds[2][3], Thresholds[3][3]};
                    dataVec[i] = j;
                    State s = State(parameterList, State::params{
                        dataVec[0],dataVec[1],dataVec[2],dataVec[3], 0, 0
                    });

                    s.simulate();
                    float out = s.costAnalysis;
                    if(out > Thresholds[i][5]){
                        Thresholds[i][4] = j;
                        Thresholds[i][5] = out;
                    }
                    vvv.push_back({j, out});
                }
                vv.push_back(vvv);
                std::cout << "Finished over one var" << " " << Thresholds[i][4] << " " << Thresholds[i][5] << "\n";
            
                for(int i = 0; i < Thresholds.size(); i++)
                    std::cout << i << " " << Thresholds[i][4] << " " << Thresholds[i][5] << "\n";
            }
            v.push_back(vv);
        }
        vector<vector<vector<pair<int, float>>>> o;
        for(int i = 0; i < Thresholds.size(); i++){
            vector<vector<pair<int, float>>> oo;
            for(int r = 0; r < parameterList["trialsForOptimizatoinRun"]; r++){
                oo.push_back(v[r][i]);
            }
            o.push_back(oo);
        }

        outputJson["outputVec"] = o;
        fs::path newOutputPath = outputPath;
        newOutputPath /= fs::path("optimizationTrialData.json");
        std::ofstream fout(newOutputPath);
        fout << outputJson.dump();
        fout.close();
        std::cout << "Outputted Json full of plot data.\n";
    }else{
        // Multi-threaded
        for(int _nymphThreshold = Thresholds[0][0]; _nymphThreshold <= Thresholds[0][1]; _nymphThreshold+=Thresholds[0][2]){
        for(int _adultThreshold = Thresholds[1][0]; _adultThreshold <= Thresholds[1][1]; _adultThreshold+=Thresholds[1][2]){
        for(int _harvestDeadline = Thresholds[2][0]; _harvestDeadline <= Thresholds[2][1]; _harvestDeadline+=Thresholds[2][2]){
        for(int _chosenPesticide = Thresholds[3][0]; _chosenPesticide <= Thresholds[3][1]; _chosenPesticide+=Thresholds[3][2]){
        for(int _costFunctionID = 0; _costFunctionID < 2; _costFunctionID++){
        for(int _stratID = 0; _stratID < 1; _stratID++){
            for(int i = 0; i<10; i++){
                data.push_back(State(parameterList, State::params{
                // _nymphThreshold, _adultThreshold, _harvestDeadline, _chosenPesticide, 0, 0
                55, 15, 270, 0, 0, 0
                }));
            }
        }}}}}
        }
    }

    // }}
    // for(int _adultThreshold = 5; _adultThreshold < 20; _adultThreshold+=2){
    //     data.push_back(State(parameterList, State::params{
    //         30, _adultThreshold, 270, 0, 0, 0
    //     }));
    // }
    std::cout << "Data size: " << data.size() << "\n";
    int threadCount = parameterList["threadCount"];
    auto threadList = vector<thread>(threadCount);
    auto threadFunction = [&](int threadId)
    {
        for (int i = threadId; i < data.size(); i += threadCount)
        {
            State* s = &data[i];

            auto threadStartTime = std::chrono::system_clock::now();
            s->simulate();
            auto threadEndTime = std::chrono::system_clock::now();
            std::cout << "Thread " << threadId << " finished computing in " << 
            std::chrono::duration_cast<std::chrono::microseconds>(threadEndTime - threadStartTime).count() / 1e6 <<
            " seconds on thing " << i << " with cost function " << s->costAnalysis << std::endl;

            // s->outputData(outputPath, i);
        }
    };

    for (int i = 0; i < parameterList["threadCount"]; i++)
    {
        threadList[i] = thread(threadFunction, i);
        std::cout << "Opening thead " << i << ", Id: " << threadList[i].get_id() << "\n";
    }

    for (auto &x : threadList)
    {
        x.join();
        std::cout << "Closed Thread " << x.get_id() << "\n";
    }

    vector<vector<State *>> V(1);
    for (auto &state : data)
    {
        V[state.inputParam.costFunctionID].push_back(&state);
    }
    for (auto &list : V)
    {
        sort(list.begin(), list.end());
    }
    for (auto &list : V)
    {
        for (int i = 0; i < 10 && i < list.size(); i++)
        {
            list[i]->outputData(outputPath, i);
        }
        int amount1 = 10;
        for (int i = 0, j = 0; j < amount1 && i < list.size(); j++, i += (1.0 / amount1) * list.size())
        {
            list[i]->outputData(outputPath, i);
        }
    }

    auto programEndTime = std::chrono::system_clock::now();
    std::cout << "Simulation Finished in " << std::chrono::duration_cast<std::chrono::microseconds>(programEndTime - programStartTime).count() / 1e6 << " seconds" << std::endl;
}