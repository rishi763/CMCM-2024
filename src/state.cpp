#include "state.h"

// \frac{e^{2.2\max\left(\left|\frac{x}{50}\right|,\left|\frac{y}{20}\right|\right)}}{2149.44}
// ^I'm cumming

void State::spawnInBugs(int numberOfEggMasses, int eggMasses, bugType type, bool bothSexes)
{
    for (int i = 0; i < numberOfEggMasses; i++)
    {
        std::pair<int, int> location = pickLocation(eggDistribution);
        for (int j = 0; j < eggMasses; j++)
        {
            if (bothSexes)
                bugCount[location.first][location.second][((int)type / 2) + (rand0to1() > 0.5)]++;
            else
                bugCount[location.first][location.second][type]++;
        }
    }
}

void State::initVineyard()
{
    int vineyardColumns = parameterList["vineyardColumns"];
    int vineyardColumnSize = parameterList["vineyardColumnSize"];
    int initialSLFEggMasses = parameterList["initialSLFEggMasses"];
    int SLFEggMassSize = parameterList["SLFEggMassSize"];

    bugCount = vector<vector<array<int, 6>>>(vineyardColumns, vector<array<int, 6>>(vineyardColumnSize, array<int, 6>{0}));
    vineHealth = vector<vector<int>>(vineyardColumns, vector<int>(vineyardColumnSize, 100));
    grapeClusters = vector<vector<int>>(vineyardColumns, vector<int>(vineyardColumnSize, 0));
    lastSprayTime = vector<vector<int>>(vineyardColumns, vector<int>(vineyardColumnSize, -1));
    friendlyInsectPopulation = vector<vector<int>>(vineyardColumns, vector<int>(vineyardColumnSize, 0));
    eggDistribution = vector<vector<double>>(vineyardColumns, vector<double>(vineyardColumnSize, 0.0));

    // Initalize egg distribtion
    for (int row = 0; row < eggDistribution.size(); row++)
    {
        for (int col = 0; col < eggDistribution[0].size(); col++)
        {
            long double topExponent = 2.2 * std::max(
                                                abs(row - (vineyardColumns - 1) / 2.0) / (long double)(vineyardColumns),
                                                abs(col - (vineyardColumnSize - 1) / 2.0) / (long double)(vineyardColumnSize));
            eggDistribution[row][col] = std::exp(topExponent) / 2067.86;
        }
    }

    // Initalize egg count, preprotional to eggDistribtion weights
    spawnInBugs(initialSLFEggMasses, SLFEggMassSize, femaleEgg, true);
}

void State::simulate()
{
    std::cout << "Initializing" << std::endl;
    initVineyard();
    std::cout << "Finished Initializing" << std::endl;
    printData();
    // calculate change in bug population
    while (currentTime <= parameterList["runTime"])
    {
        // std::cout << "Stepping" << std::endl;
        timeStep();
        currentTime += (int)parameterList["timeStep"];
    }
    std::cout << "Finished Simulating on this Thread" << std::endl;
}

void State::timeStep()
{
    updateEnvironment();
    updateBug();
}

void State::updateEnvironment()
{
    currentMonth = (currentTime % 360) / 30;
    currentDay = currentTime % 360;
}

void State::migrateASpecies(bugType type, function<int(int)> weatherFunc)
{
    int vineyardColumns = parameterList["vineyardColumns"];
    int vineyardColumnSize = parameterList["vineyardColumnSize"];

    int checkSum = 0;

    // Migrate nymphs
    // up, down, left, right
    auto updateDist = vector<vector<array<int, 4>>>(vineyardColumns, vector<array<int, 4>>(vineyardColumnSize, array<int, 4>{0}));
    // Store in each nymph    
    for (int x = 0; x < updateDist.size(); x++)
    {
        for (int y = 0; y < updateDist[0].size(); y++)
        {
            // Put into each area, also where jumping has different probabilties and
            // the part about last 3 weeks little ones wont come comes from.
            int total = weatherFunc(bugCount[x][y][type]);
            checkSum += bugCount[x][y][type];
            bugCount[x][y][type] -= total;
            // Distribuite to each updaate dist but with the right probabilties
            int totalHor = total * (double)parameterList["SLFHorJmpRateRelToVertical"];
            int totalVert = total - totalHor;
            // TODO Graviate: change random integer dist as tiem goes on and fav trees
            updateDist[x][y][0] = totalVert * rand0to1();
            updateDist[x][y][1] = totalVert - updateDist[x][y][0];
            updateDist[x][y][2] = totalHor * rand0to1();
            updateDist[x][y][3] = totalHor - updateDist[x][y][2];
        


            // Check for edge case at border, if so copy the value to the other side (always one vertical / 1 horiztonal wille exist)
            if (y == 0)
            {
                updateDist[x][y][0] += updateDist[x][y][1];
                updateDist[x][y][1] = 0;
            }
            if (y == updateDist[0].size() - 1)
            {
                updateDist[x][y][1] += updateDist[x][y][0];
                updateDist[x][y][0] = 0;
            }
            if (x == 0)
            {
                updateDist[x][y][3] += updateDist[x][y][2];
                updateDist[x][y][2] = 0;
            }
            if (x == updateDist.size() - 1)
            {
                updateDist[x][y][2] += updateDist[x][y][3];
                updateDist[x][y][3] = 0;
            }
        }
    }

    int checkSum0 = 0;
    int checkSum1 = 0;
    int checkSum2 = 0;
    int checkSum3 = 0;
    int checkSum4 = 0;
    for (int x = 0; x < updateDist.size(); x++)
    {
        for (int y = 0; y < updateDist[0].size(); y++)
        {
            checkSum0 += updateDist[x][y][0];
            checkSum1 += updateDist[x][y][1];
            checkSum2 += updateDist[x][y][2];
            checkSum3 += updateDist[x][y][3];
            checkSum4 += bugCount[x][y][type];
        }

    }
    int checkSum5 = checkSum0 + checkSum1 + checkSum2 + checkSum3 + checkSum4;
    std::cout << "CheckSum: " << checkSum0 << " " << checkSum1 << " " << checkSum2 <<
     " " << checkSum3 << " " << checkSum4 << " " << checkSum5 << " " << checkSum << "\n";

    for (int x = 0; x < updateDist.size(); x++)
    {
        for (int y = 0; y < updateDist[0].size(); y++)
        {
            if (y != 0)
                bugCount[x][y - 1][type] += updateDist[x][y][0];
            if (y != updateDist[0].size() - 1)
                bugCount[x][y + 1][type] += updateDist[x][y][1];
            if (x != 0)
                bugCount[x - 1][y][type] += updateDist[x][y][2];
            if (x != updateDist.size() - 1)
                bugCount[x + 1][y][type] += updateDist[x][y][3];
        }
    }
}

// This is a heavy ocmputation loop. We should prolly update at some point if we can
void State::migrateBugs()
{

    function<int(int)> a = [&](int size)
    { return size; };
    function<int(int)> b = [&](int size)
    { return size * (int)(parameterList["SLFAdultTransferRate"]); };
    // TEMP
    migrateASpecies(femaleEgg, a);
    migrateASpecies(maleEgg, a);

    migrateASpecies(femaleNymph, a);
    migrateASpecies(maleNymph, a);
    // TODO: create the probability fucntion
    migrateASpecies(femaleAdult, b);
    migrateASpecies(maleAdult, b);
}

void State::updateBug()
{
    // Spawn in Function
    // int amountToSpawn = 10; //TODO: SEASON DEPENDENT
    // spawnInBugs(amountToSpawn, femaleAdult, true);

    // Renormalize population

    // TODO:Graudal Death

    // Eggs->Nymph
    int hatchEggsDay = (int)parameterList["hatchEggsDay"];
    if (hatchEggsDay <= currentDay && currentDay <= hatchEggsDay + 30)
    {
        double probability = 0.0398942280401 * exp(-0.5 * (pow((currentTime - hatchEggsDay - 15) / 5.0, 2)));
        for (auto &x : bugCount)
        {
            for (auto &y : x)
            {
                int newMaleNymph = 0;
                int newFemaleNymph = 0;
                for (int i = 0; i < y[0]; i++)
                {
                    if (rand0to1() < probability)
                    {
                        newFemaleNymph++;
                    }
                }
                for (int i = 0; i < y[1]; i++)
                {
                    if (rand0to1() < probability)
                    {
                        newMaleNymph++;
                    }
                }
                y[0] -= newFemaleNymph;
                y[2] += newFemaleNymph;

                y[1] -= newMaleNymph;
                y[3] += newMaleNymph;
                if (currentDay == hatchEggsDay + 30)
                {
                    y[0] = 0;
                    y[1] = 0;
                }
            }
        }
    }

    // Nymph->Adult
    int turnToAdultDay = (int)parameterList["turnToAdultDay"];
    if (turnToAdultDay <= currentDay && currentDay <= turnToAdultDay + 60)
    {
        double probability = 0.0398942280401 * exp(-0.5 * (pow((currentTime - hatchEggsDay - 30) / 10.0, 2)));
        for (auto &x : bugCount)
        {
            for (auto &y : x)
            {
                int newMaleAdult = 0;
                int newFemaleAdult = 0;
                for (int i = 0; i < y[2]; i++)
                {
                    if (rand0to1() < probability)
                    {
                        newFemaleAdult++;
                    }
                }
                for (int i = 0; i < y[3]; i++)
                {
                    if (rand0to1() < probability)
                    {
                        newMaleAdult++;
                    }
                }
                y[2] -= newFemaleAdult;
                y[4] += newFemaleAdult;

                y[3] -= newMaleAdult;
                y[5] += newMaleAdult;
                if (currentDay == turnToAdultDay + 60)
                {
                    y[2] = 0;
                    y[3] = 0;
                }
            }
        }
    }

    // Kill by enviromnttal factors
    if (currentTime == (int)parameterList["winterKillDay"])
    {
        for (auto &x : bugCount)
        {
            for (auto &y : x)
            {
                for (int i = 2; i < 6; i++)
                {
                    y[i] = 0;
                }
            }
        }
    }

    // Migrate
    migrateBugs();

    // Update Grape/Vine properties
    // TODO: Fungus

    // Harvest (if it is time of year)
    // if(currentTime == harvestDay){
    //     for(auto& i : grapeClusters){
    //         for(auto &j : i){
    //             totalGrapes += j;
    //             j = 0;
    //         }
    //     }
    // }

    // Lay eggs
    if (currentTime == (int)parameterList["layEggsDay"])
    {
        for (auto &x : bugCount)
        {
            for (auto &y : x)
            {
                y[0] = rand0to1() * y[4] * (int)parameterList["SLFEggMassSize"] / 2;
                y[1] = rand0to1() * y[4] * (int)parameterList["SLFEggMassSize"] / 2;
            }
        }
    }
    printData();
}

void State::printData()
{
    // Print Data
    int vineyardColumns = parameterList["vineyardColumns"];
    int vineyardColumnSize = parameterList["vineyardColumnSize"];
    array<int, 6> eggList{0};
    vector<vector<int>> V(vineyardColumns, vector<int>(vineyardColumnSize, 0));
    for (int i = 0; i < vineyardColumns; i++)
    {
        for (int j = 0; j < vineyardColumnSize; j++)
        {
            for (int k = 0; k < 6; k++)
                eggList[k] += bugCount[i][j][k];
            V[i][j] = bugCount[i][j][maleNymph] + bugCount[i][j][femaleNymph];
        }
    }
    outputVec.push_back(V);
    std::cout << currentTime;
    for (auto &x : eggList)
        std::cout << " " << x;
    std::cout << std::endl;
}

void State::serializeData()
{
    std::cout << "Time To Seralize!" << std::endl;
    outputJson["x"] = parameterList["vineyardColumns"];
    outputJson["y"] = parameterList["vineyardColumnSize"];
    outputJson["distribtionMap"] = outputVec;
    // std::stringstream strStream;
    // vector<vector<int>> population
    // for (int x = 0; x < (int)outputJson["x"]; x++)
    // {
    //     for (int y = 0; y < (int)outputJson["y"]; y++)
    //     {
    //         strStream << std::to_string(bugCount[x][y][femaleEgg] + bugCount[x][y][maleEgg]) + ",";
    //     }
    // }
    // outputJson["weightMap"] = strStream.str(); // Keep doing this basically.
}

void State::outputData(fs::path outputPath, int threadId)
{
    serializeData();
    outputPath /= fs::path("simTrial" + std::to_string(threadId) + ".json");
    std::ofstream fout(outputPath);
    fout << outputJson.dump();
    fout.close();
}

std::pair<int, int> State::pickLocation(vector<vector<double>> distribution)
{
    double randomNumber = rand0to1();
    for (size_t row = 0; row < distribution.size(); row++)
    {
        for (size_t col = 0; col < distribution[0].size(); col++)
        {
            if (randomNumber < distribution[row][col])
            {
                return {row, col};
            }
            randomNumber -= distribution[row][col];
        }
    }
    return {distribution.size() - 1, distribution[0].size() - 1};
}

