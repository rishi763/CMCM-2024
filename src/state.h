#include "pch.h"

struct State
{
    json parameterList;
    json outputJson;
    vector<vector<vector<int>>> outputVec;

    int chosenPesticide = 0;
    int harvestDay = 270;
    bool errorMessages = false;
    State(const json &_parameterList, int _chosenPesticide)
    {
        parameterList = _parameterList;
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

    static std::pair<int, int> pickLocation(vector<vector<double>> distribution);

    void flatThresholdStrategy(int nymphThreshold, int adultThreshold, int harvestDeadline){
        //  int nymphThreshold = 50;
        //  int adultThreshold = 10;
        //  int harvestDeadline = 270;
        int chosenPesticide=0;



        double adultEfficacy = (double) parameterList["pesticide"][chosenPesticide]["SLFEfficacy"];

        int sprayPreHarvestInterval = (int) parameterList["sprayPreHarvestInterval"];
        for (int x = 0; x < bugCount.size(); x++)
        {
            for (int y = 0; y < bugCount[0].size(); y++)
            {
                if (spraysLeft[x][y] <= 0)
                {
                    return;
                }
                
                if (currentDay >= harvestDeadline - sprayPreHarvestInterval)
                {
                    return;
                }
                if ((bugCount[x][y][maleNymph] + bugCount[x][y][femaleNymph] >= nymphThreshold) || (bugCount[x][y][maleAdult] + bugCount[x][y][femaleAdult] >= adultThreshold))
                {
                    std::cout<<"Spray used"<<std::endl;
                    spraysLeft[x][y]--;
                    bugCount[x][y][maleNymph] = 0;
                    bugCount[x][y][femaleNymph] = 0;
                    int deadMaleAdults = 0;
                    int deadFemaleAdults = 0;
                    for (int i = 0; i < bugCount[x][y][maleAdult]; i++)
                    {
                    if (rand0to1() < adultEfficacy)
                    {
                        deadMaleAdults++;
                    }
                    }
                    for (int i = 0; i < bugCount[x][y][femaleAdult]; i++)
                    {
                    if (rand0to1() < adultEfficacy)
                    {
                        deadFemaleAdults++;
                    }
                    }
                    bugCount[x][y][maleAdult] -= deadMaleAdults;
                    bugCount[x][y][femaleAdult] -= deadFemaleAdults;

                    lastSprayTime[x][y] = currentTime;
                }
            }
        }
    };
};
