#include "pch.h"

struct State
{
    json parameterList;
    json outputJson;
    vector<vector<vector<int>>> outputVec;
    

    struct params{
        int nymphThreshold = 50;
        int adultThreshold = 10; 
        int harvestDeadline = 270;
        int chosenPesticide = 0;
        int costFunctionID = 0;
        int stratID = 0;
        params() : 
        nymphThreshold(int _nymphThreshold), adultThreshold(int _adultThreshold), harvestDeadline(int _harvestDeadline),
        chosenPesticide(int _chosenPesticide), costFunctionID(int _costFunctionID), stratID(int _stratID) {}
    }inputParam;
    
    State(const json &_parameterList, params _inputParam)
    {   
        parameterList = _parameterList;
        inputParam = _inputParam;
        // chosenPesticide = _chosenPesticide;
    }

    // Dont change order please
    enum bugType
    {
        femaleEgg,
        maleEgg,
        femaleNymph,
        maleNymph,
        femaleAdult,
        maleAdult
    };


    // struct plant
    // {
    //     array<int, 6> bugCount{0};
    //     int vineHealth = 100;                                     // 0 to 100
    //     int grapeClusters = 40;
    //     // parameterList["grapeClusterAverage"]; // on average a healthy grape vine
    // };

    int currentTime = 1;
    int currentMonth = 1;
    int currentDay = 1;
    int totalGrapes = 0;
    int costAnalysis = 0;

    // Map represtation of our farm
    vector<vector<array<int, 6>>> bugCount;
    vector<vector<float>> vineHealth;
    vector<vector<float>> fungus;
    vector<vector<int>> spraysLeft;
    vector<vector<int>> lastSprayTime;
    vector<vector<int>> friendlyInsectPopulation;
    vector<vector<double>> eggDistribution;
    vector<array<int, 7>> sumBugs;
    vector<int> grapeClusters;
    vector<float> grapeMoney;
    vector<float> sprayMoney;
    vector<float> pnl;
    vector<float> eachtimestepVineHelath;

    // Overarching functions
    void initVineyard();
    void simulate();
    void timeStep();
    void applySprayStrategy(std::string strategy);

    // Properties of bug simulation functions
    void updateEnvironment();
    // void searching();
    void exterminate();
    void updateBug();
    void spawnInBugs(int numberOfEggMasses, int eggMasses, bugType type, bool bothSexes);
    void migrateASpecies(bugType type, function<int(int)> weatherFunc);


    void spawnInUpdate();
    void eggsToNymph();
    void nymphToAdult();
    void migrateBugs();
    void winterKillDay();
    void grapeVineUpdate();
    void fungusPlantHealthUpdate();
    void layEggsDay();


    // Data output Functions
    void serializeData();
    void outputData(fs::path outputPath, int threadId);
    void printData();
    // Helper functions
    static double rand0to1()
    {
        return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    }
    vector<function<float()>> costFunctions{
        // Sum grapes
        [&]()->float{
            return totalGrapes;
        },
        // Sum vineHealth
        [&]()->float{
            return eachtimestepVineHelath.back();
        },
        // Linear Sum of both
        [&]()->float{
            return eachtimestepVineHelath.back() + totalGrapes;
        }
    };
    
    static std::pair<int, int> pickLocation(vector<vector<double>> distribution);

    void flatThresholdStrategy();

};
