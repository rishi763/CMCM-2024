#include "pch.h"

struct State
{
    json parameterList;
    json outputJson;
    vector<vector<vector<int>>> outputVec;

    State(const json &_parameterList)
    {
        parameterList = _parameterList;
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
    int spraysLeft = 100; // parameterList["sprayMaximumDose"];
    int totalGrapes = 0;

    // Map represtation of our farm
    vector<vector<array<int, 6>>> bugCount;
    vector<vector<int>> vineHealth;
    vector<vector<int>> lastSprayTime;
    vector<vector<int>> grapeClusters;
    vector<vector<int>> friendlyInsectPopulation;
    vector<vector<double>> eggDistribution;

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
    void spawnInBugs(int amount, bugType type, bool bothSexes);
    void migrateASpecies(bugType type, function<int(int)> weatherFunc);

    void migrateBugs();

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

    std::map<string, std::function<void(int nymphThreshold, int adultThreshold, int harvestDeadline)>> strategies = {
    {"flatThresholdStrategy", [&](int nymphThreshold, int adultThreshold, int harvestDeadline)
    {
        //  int nymphThreshold = 50;
        //  int adultThreshold = 10;
        //  int harvestDeadline = 270;

        int sprayPreHarvestInterval = parameterList["sprayPreHarvestInterval"];

        for (int x = 0; x < bugCount.size(); x++)
        {
            for (int y = 0; y < bugCount[0].size(); y++)
            {
                if (spraysLeft <= 0)
                {
                    return;
                }
                if ((currentTime % 360) <= harvestDeadline - sprayPreHarvestInterval)
                {
                    return;
                }
                if ((bugCount[x][y][maleNymph] + bugCount[x][y][femaleNymph] >= nymphThreshold) || (bugCount[x][y][maleAdult] + bugCount[x][y][femaleAdult] >= adultThreshold))
                {
                    bugCount[x][y][maleNymph] = 0;
                    bugCount[x][y][femaleNymph] = 0;
                    int deadMaleAdults = 0;
                    int deadFemaleAdults = 0;
                    for (int i = 0; i < bugCount[x][y][maleAdult]; i++)
                    {
                    if (rand0to1() < 0.6)
                    {
                        deadMaleAdults++;
                    }
                    }
                    for (int i = 0; i < bugCount[x][y][femaleAdult]; i++)
                    {
                    if (rand0to1() < 0.6)
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
    }}};
};
