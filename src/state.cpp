#include "state.h"

// \frac{e^{2.2\max\left(\left|\frac{x}{50}\right|,\left|\frac{y}{20}\right|\right)}}{2149.44}

void State::spawnInBugs(int numberOfEggMasses, int eggMasses, bugType type, bool bothSexes)
{
    for (int i = 0; i < numberOfEggMasses; i++)
    {
        std::pair<int, int> location = pickLocation(eggDistribution);
        for (int j = 0; j < eggMasses; j++)
        {
            if (bothSexes){
                int newBugType=((int)type / 2) + (rand0to1() > 0.5);
                bugCount[location.first][location.second][newBugType]++;
            }
            else{
                bugCount[location.first][location.second][type]++;
            }
        }
    }
}

void State::initVineyard()
{
    int vineyardColumns = parameterList["vineyardColumns"];
    int vineyardColumnSize = parameterList["vineyardColumnSize"];
    int initialSLFEggMasses = parameterList["initialSLFEggMasses"];
    int SLFEggMassSize = parameterList["SLFEggMassSize"];
    int plantHealth = parameterList["plantHealth"];

    
    bugCount = vector<vector<array<int, 6>>>(vineyardColumns, vector<array<int, 6>>(vineyardColumnSize, array<int, 6>{0}));
    vineHealth = vector<vector<float>>(vineyardColumns, vector<float>(vineyardColumnSize, plantHealth));
    fungus = vector<vector<float>>(vineyardColumns, vector<float>(vineyardColumnSize, 0));
    lastSprayTime = vector<vector<int>>(vineyardColumns, vector<int>(vineyardColumnSize, -1000));
    friendlyInsectPopulation = vector<vector<int>>(vineyardColumns, vector<int>(vineyardColumnSize, 0));
    eggDistribution = vector<vector<double>>(vineyardColumns, vector<double>(vineyardColumnSize, 0.0));
    sumBugs = vector<array<int, 7>>();
    grapeClusters = vector<int>();
    grapeMoney = vector<float>();
    sprayMoney = vector<float>();
    eachTimeStepVineHealth = vector<float>();
    pnl = vector<float>();
    int maximumSprayDose = (int) parameterList["pesticide"][inputParam.chosenPesticide]["maximumSprayDose"];
    spraysLeft = vector<vector<int>>(vineyardColumns, vector<int>(vineyardColumnSize, maximumSprayDose));
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
    // std::cout << "Initializing" << std::endl;
    initVineyard();
    // std::cout << "Finished Initializing" << std::endl;
    // printData();
    // calculate change in bug population
    while (currentTime <= parameterList["runTime"])
    {
        // std::cout << "Stepping" << std::endl;
        timeStep();
        currentTime += (int)parameterList["timeStep"];
    }
    function<float()> costFunc = costFunctions[inputParam.costFunctionID];
    costAnalysis = costFunc();
    // std::cout << "Finished Simulating on this Thread" << std::endl;
}

void State::timeStep()
{
    updateEnvironment();
    updateBug();
    flatThresholdStrategy();
    printData();
}

void State::updateEnvironment()
{
    currentMonth = (currentTime % 360) / 30;
    currentDay = currentTime % 360;
}

void State::updateBug()
{
    // Renormalize population
    eggsToNymph();
    nymphToAdult();

    // enviromntal Factors
    winterKillDay();

    // Migrate
    migrateBugs();
    grapeVineUpdate();
    fungusPlantHealthUpdate();
    layEggsDay();
    // std::cout<<eachTimeStepVineHealth.back()<<std::endl;

}

// This is a heavy ocmputation loop. We should prolly update at some point if we can
void State::migrateBugs()
{

    int vineyardColumns = (int)parameterList["vineyardColumns"];
    int vineyardColumnSize = (int)parameterList["vineyardColumnSize"];

    double adultLongevity = (double) parameterList["pesticide"][inputParam.chosenPesticide]["adultLongevity"];
    double nymphLongevity = (double) parameterList["pesticide"][inputParam.chosenPesticide]["nymphLongevity"];

    int nymphSprayThreshold = (int) parameterList["bugReturnAfterSpray"];
    double adultJump = (double)(parameterList["SLFAdultTransferRate"]);


    double vertJump = (double)parameterList["SLFHorJmpRateRelToVertical"];
    double horJump = 1.0 - vertJump;

    auto newBugCount = vector<vector<array<int, 6>>>(vineyardColumns, vector<array<int, 6>>(vineyardColumnSize, array<int, 6>{0}));

    for (int x = 0; x < bugCount.size(); x++)
    {
        for (int y = 0; y < bugCount[0].size(); y++)
        {
            newBugCount[x][y][femaleEgg] = bugCount[x][y][femaleEgg];
            newBugCount[x][y][maleEgg] = bugCount[x][y][maleEgg];

            for (int i = 0; i < bugCount[x][y][femaleNymph]; i++)
            {

                double vertRandom = rand0to1();
                double horRandom = rand0to1();

                double up = (x != vineyardColumns - 1 && currentTime - lastSprayTime[x + 1][y] > nymphLongevity) ? vertJump * vertRandom : 0.0;
                double right = (y != vineyardColumnSize - 1 && currentTime - lastSprayTime[x][y + 1] > nymphLongevity) ? horJump * horRandom : 0.0;
                double down = (x != 0 && currentTime - lastSprayTime[x - 1][y] > nymphLongevity) ? vertJump * (1 - vertRandom) : 0.0;
                double left = (y != 0 && currentTime - lastSprayTime[x][y - 1] > nymphLongevity) ? horJump * (1 - horRandom) : 0.0;

                if (up + down + left + right == 0.0)
                {
                    newBugCount[x][y][femaleNymph] = 0;
                    continue;
                }

                double randomDirection = rand0to1() * (up + down + right + left);

                if (randomDirection < up)
                {
                    newBugCount[x + 1][y][femaleNymph]++;
                    continue;
                }

                randomDirection -= up;
                if (randomDirection < right)
                {
                    newBugCount[x][y + 1][femaleNymph]++;
                    continue;
                }
                randomDirection -= right;
                if (randomDirection < down)
                {
                    newBugCount[x - 1][y][femaleNymph]++;
                    continue;
                }

                newBugCount[x][y - 1][femaleNymph]++;
            }

            for (int i = 0; i < bugCount[x][y][maleNymph]; i++)
            {

                double vertRandom = rand0to1();
                double horRandom = rand0to1();

                double up = (x != vineyardColumns - 1 && currentTime - lastSprayTime[x + 1][y] > nymphLongevity) ? vertJump * vertRandom : 0.0;
                double right = (y != vineyardColumnSize - 1 && currentTime - lastSprayTime[x][y + 1] > nymphLongevity) ? horJump * horRandom : 0.0;
                double down = (x != 0 && currentTime - lastSprayTime[x - 1][y] > nymphLongevity) ? vertJump * (1 - vertRandom) : 0.0;
                double left = (y != 0 && currentTime - lastSprayTime[x][y - 1] > nymphLongevity) ? horJump * (1 - horRandom) : 0.0;

                if (up + down + left + right == 0.0)
                {
                    newBugCount[x][y][maleNymph] = 0;
                    continue;
                }

                double randomDirection = rand0to1() * (up + down + right + left);

                if (randomDirection < up)
                {
                    newBugCount[x + 1][y][maleNymph]++;
                    continue;
                }
                randomDirection -= up;
                if (randomDirection < right)
                {
                    newBugCount[x][y + 1][maleNymph]++;
                    continue;
                }
                randomDirection -= right;
                if (randomDirection < down)
                {
                    newBugCount[x - 1][y][maleNymph]++;
                    continue;
                }
                newBugCount[x][y - 1][maleNymph]++;
            }

            for (int i = 0; i < bugCount[x][y][femaleAdult]; i++)
            {

                double vertRandom = rand0to1();
                double horRandom = rand0to1();

                double up = (x != vineyardColumns - 1 && currentTime - lastSprayTime[x + 1][y] > adultLongevity) ? adultJump * vertJump * vertRandom : 0.0;
                double right = (y != vineyardColumnSize - 1 && currentTime - lastSprayTime[x][y + 1] > adultLongevity)? adultJump * horJump * horRandom : 0.0;
                double down = (x != 0 && currentTime - lastSprayTime[x - 1][y] > adultLongevity) ? adultJump * vertJump * (1 - vertRandom) : 0.0;
                double left = (y != 0 && currentTime - lastSprayTime[x][y - 1] > adultLongevity) ? adultJump * horJump * (1 - horRandom) : 0.0;

                double randomDirection = rand0to1() * (up + down + right + left);

                if (randomDirection < up)
                {
                    newBugCount[x + 1][y][femaleAdult]++;
                    continue;
                }
                randomDirection -= up;
                if (randomDirection < right)
                {
                    newBugCount[x][y + 1][femaleAdult]++;
                    continue;
                }
                randomDirection -= right;
                if (randomDirection < down)
                {
                    newBugCount[x - 1][y][femaleAdult]++;
                    continue;
                }
                randomDirection -= down;
                if (randomDirection < left)
                {
                    newBugCount[x][y - 1][femaleAdult]++;
                    continue;
                }
                newBugCount[x][y][femaleAdult]++;
            }

            for (int i = 0; i < bugCount[x][y][maleAdult]; i++)
            {

                double vertRandom = rand0to1();
                double horRandom = rand0to1();

                double up = (x != vineyardColumns - 1 && currentTime - lastSprayTime[x + 1][y] > adultLongevity) ? adultJump * vertJump * vertRandom : 0.0;
                double right = (y != vineyardColumnSize - 1 && currentTime - lastSprayTime[x][y + 1] > adultLongevity) ? adultJump * horJump * horRandom : 0.0;
                double down = (x != 0 && currentTime - lastSprayTime[x - 1][y] > adultLongevity) ? adultJump * vertJump * (1 - vertRandom) : 0.0;
                double left = (y != 0 && currentTime - lastSprayTime[x][y - 1] > adultLongevity) ? adultJump * horJump * (1 - horRandom) : 0.0;

                double randomDirection = rand0to1() * (up + down + right + left);

                if (randomDirection < up)
                {
                    newBugCount[x + 1][y][maleAdult]++;
                    continue;
                }
                randomDirection -= up;
                if (randomDirection < right)
                {
                    newBugCount[x][y + 1][maleAdult]++;
                    continue;
                }
                randomDirection -= right;
                if (randomDirection < down)
                {
                    newBugCount[x - 1][y][maleAdult]++;
                    continue;
                }
                randomDirection -= down;
                if (randomDirection < left)
                {
                    newBugCount[x][y - 1][maleAdult]++;
                    continue;
                }
                newBugCount[x][y][maleAdult]++;
            }
        }
    }
    bugCount = newBugCount;
}

void State::eggsToNymph()
{
    // Eggs->Nymph
    int hatchEggsDay = (int)parameterList["hatchEggsDay"];
    if (hatchEggsDay <= currentDay && currentDay <= hatchEggsDay + 30)
    {
        double probability = 0.0797884561 * exp(-0.5 * (pow((currentDay - (hatchEggsDay + 15)) / 1.0, 2.0))); // ~20% survive

        int skip = 1.0/probability;

        int femalesSoFar = 0;
        int malesSoFar = 0;


        for (auto &x : bugCount)
        {
            for (auto &y : x)
            {
                int newFemaleNymph =  floor((femalesSoFar+y[femaleEgg])/(1.0*skip))-ceil(femalesSoFar/(1.0*skip))+1;
                int newMaleNymph = floor((malesSoFar+y[maleEgg])/(1.0*skip))-ceil(malesSoFar/(1.0*skip))+1;

                femalesSoFar+=y[femaleEgg];
                malesSoFar+=y[maleEgg];

                y[femaleEgg] -= newFemaleNymph;
                y[femaleNymph] += newFemaleNymph;

                y[maleEgg] -= newMaleNymph;
                y[maleNymph] += newMaleNymph;


                if (currentDay == hatchEggsDay + 30)
                {
                    y[femaleEgg] = 0;
                    y[maleEgg] = 0;

                }
            }
        }
    }
}

void State::nymphToAdult()
{
    // Nymph->Adult
    int turnToAdultDay = (int)parameterList["turnToAdultDay"];

    if (turnToAdultDay <= currentDay && currentDay <= turnToAdultDay + 60)
    {
        double probability = 0.179524026181 * exp(-0.5 * (pow((currentDay - (turnToAdultDay + 30)) / 2.0, 2))); // 90% survive
        int skip = 1.0/probability;

        int femalesSoFar = 0;
        int malesSoFar = 0;

        for (auto &x : bugCount)
        {
            for (auto &y : x)
            {
                int newFemaleAdult = floor((femalesSoFar+y[femaleNymph])/(1.0*skip))-ceil(femalesSoFar/(1.0*skip))+1  ;
                int newMaleAdult = floor((malesSoFar+y[maleNymph])/(1.0*skip))-ceil(malesSoFar/(1.0*skip))+1;
                
                femalesSoFar+=y[femaleNymph];
                malesSoFar+=y[maleNymph];

                y[femaleNymph] -= newFemaleAdult;
                y[femaleAdult] += newFemaleAdult;

                y[maleNymph] -= newMaleAdult;
                y[maleAdult] += newMaleAdult;


                if (currentDay == turnToAdultDay + 60)
                {
                    y[femaleNymph] = 0;
                    y[maleNymph] = 0;
                }
            }
        }
    }
}

void State::winterKillDay()
{
    if (currentDay == (int)parameterList["winterKillDay"])
    {
        for (int x = 0; x < bugCount.size(); x++)
        {
            for (int y = 0; y < bugCount[0].size(); y++)
            {
                for (int i = 2; i < 6; i++)
                {
                    bugCount[x][y][i] = 0;
                    
                }
                // Reset Sprays
                lastSprayTime[x][y] = -1000;
            }
        }
        // Update spray values because no more bugs exist
        int sprayMaximumDose = parameterList["pesticide"][inputParam.chosenPesticide]["maximumSprayDose"];
        float costPerSpray = parameterList["pesticide"][inputParam.chosenPesticide]["costPerSpray"];
        int usedSprays=0;
        for(auto &row : spraysLeft){
            for(auto& entry: row){
                usedSprays += sprayMaximumDose-entry;
                entry = sprayMaximumDose;
            }
        }
        float sprayCost = usedSprays * costPerSpray;
        sprayMoney.push_back(sprayCost);
        pnl.push_back(grapeMoney.back() - sprayCost);
    }
}

void State::grapeVineUpdate()
{
    if (currentDay == inputParam.harvestDeadline)
    {
        int grapesMade = 0;
        float fungusThreshold = parameterList["fungusThreshold"];
        int grapeClusterAverage = parameterList["grapeClusterAverage"];
        int redZone = parameterList["redZone"];
        int PLI = parameterList["pesticide"][inputParam.chosenPesticide]["PHI"];

        for (int x = 0; x < bugCount.size(); x++)
        {
            for (int y = 0; y < bugCount[0].size(); y++)
            {
                if (!(fungus[x][y] >= fungusThreshold))
                {
                    totalGrapes += grapeClusterAverage*vineHealth[x][y]/100.0;
                    grapesMade += grapeClusterAverage*vineHealth[x][y]/100.0;


                }
            }
        }
        double grapesMoneyMade = grapesMade * (double)parameterList["grapesToDollars"];
        grapeMoney.push_back(grapesMoneyMade);
        grapeClusters.push_back(grapesMade);
    }
}

void State::fungusPlantHealthUpdate()
{
    float fungusThreshold = parameterList["fungusThreshold"];
    int grapeClusterAverage = parameterList["grapeClusterAverage"];
    int fungusYIntercept = parameterList["fungusYIntercept"];
    float fungusSlope = parameterList["fungusSlope"];
    int plantHealthThreshold = parameterList["plantHealthThreshold"];
    float plantHealthSlope = parameterList["plantHealthSlope"];
    float nymphLowDamageMultiplier = parameterList["nymphLowDamageMultiplier"];
    
    double vineHealthSum = 0;

    for (int x = 0; x < bugCount.size(); x++)
    {
        for (int y = 0; y < bugCount[0].size(); y++)
        {
            // Inc dec fungus depending on how many bugs are there
            int bugCnt = 0;
            for (int i = 2; i < 6; i++)
            {
                bugCnt += bugCount[x][y][i];
            }
            fungus[x][y] = std::max(0.0f, fungus[x][y] + fungusSlope * (bugCnt - fungusYIntercept));

            // Decrease plant health if more than 70 bugs
            int healthCnt = (int)(nymphLowDamageMultiplier*(bugCount[x][y][2] + bugCount[x][y][3])) + bugCount[x][y][4] + bugCount[x][y][5];
            if (healthCnt >= plantHealthThreshold)
            {
                vineHealth[x][y] = std::max(0.0f, vineHealth[x][y] - (plantHealthSlope * (healthCnt - plantHealthThreshold)));
            }
            vineHealthSum += vineHealth[x][y];
            
        }
    }
    float vineHealthAvg = vineHealthSum/(1000.0);
    eachTimeStepVineHealth.push_back(vineHealthAvg);
}

void State::layEggsDay()
{
    int eggMassSize = (int)parameterList["SLFEggMassSize"]; 
    double SLFFemaleEggLayRate = (double) parameterList["SLFFemaleEggLayRate"];
    double SLFFemaleEggDoubleLayRate =  (double) parameterList["SLFFemaleEggDoubleLayRate"];
    // Lay eggs
    if (currentDay == (int)parameterList["layEggsDay"])
    {
        for (auto &x : bugCount)
        {
            for (auto &y : x)
            {
                double randNum = rand0to1();
                if(randNum<SLFFemaleEggLayRate){
                    int numberOfEggs = (20*rand0to1()-10)+eggMassSize;

                    
                    y[femaleEgg] += (0.5) * y[femaleAdult] * numberOfEggs;
                    y[maleEgg] += (0.5) * y[femaleAdult] * numberOfEggs;

                    if(randNum<SLFFemaleEggLayRate*SLFFemaleEggDoubleLayRate){
                        int numberOfEggs = (20*rand0to1()-10)+eggMassSize;

                        y[femaleEgg] += (0.5) * y[femaleAdult] * numberOfEggs;
                        y[maleEgg] += (0.5) * y[femaleAdult] * numberOfEggs;
                    } 

                }

            }
        }
    }
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
    array<int, 7> eggSum;
    for (int i = 0; i < 6; i++)
        eggSum[i] = eggList[i];
    for (auto &x : eggList)
        eggSum[6] += x;
    sumBugs.push_back(eggSum);
    // outputVec.push_back(V);
    if((bool)parameterList["printStatements"]){
        std::cout << currentTime;
        for (auto &x : eggList)
            std::cout << " " << x;
        std::cout << std::endl;
    }
}

void State::serializeData()
{   
    // std::cout << "Time To Seralize!" << std::endl;
    outputJson["x"] = parameterList["vineyardColumns"];
    outputJson["y"] = parameterList["vineyardColumnSize"];
    outputJson["grapeClusters"] = grapeClusters;
    outputJson["grapeMoney"] = grapeMoney;
    outputJson["sprayMoney"] = sprayMoney;
    outputJson["pnl"] = pnl;
    outputJson["vineHealth"] = vineHealth;
    outputJson["pesticideStrat"] = inputParam.chosenPesticide;
    outputJson["sumBugs"] = sumBugs;
    outputJson["eachTimeStepVineHealth"] = eachTimeStepVineHealth;
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


void State::flatThresholdStrategy(){

    double adultEfficacy = (double) parameterList["pesticide"][inputParam.chosenPesticide]["SLFEfficacy"];
    int sprayPreHarvestInterval = (int) parameterList["pesticide"][inputParam.chosenPesticide]["PHI"];
    
    for (int x = 0; x < bugCount.size(); x++)
    {
        for (int y = 0; y < bugCount[0].size(); y++)
        {
            if (spraysLeft[x][y] <= 0)
            {
                return;
            }
            
            if (currentDay >= inputParam.harvestDeadline - sprayPreHarvestInterval)
            {
                return;
            }
            if ((bugCount[x][y][maleNymph] + bugCount[x][y][femaleNymph] >= inputParam.nymphThreshold) || (bugCount[x][y][maleAdult] + bugCount[x][y][femaleAdult] >= inputParam.adultThreshold))
            {
                // std::cout<<"Spray Used!"<<" "<<spraysLeft[x][y]--<<std::endl;
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
}