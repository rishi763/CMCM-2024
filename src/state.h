#include "pch.h"
// #ifndef state
// #define state

struct State
{
    enum SEX{
        FEMALE,
        MALE
    };

    struct slf{
        bool isAdult;
        SEX sex;
        //Health attributes?
    };

    struct plant
    {
        vector<slf> bugList;
        int vineHealth = 100; //0 to 100
        int grapeClusters = parameterList["grapeClusterAverage"];//on average a healthy grape vine  
    };


    vector<vector<plant>> vineyard;
    static json parameterList;
    // int foo = 0;
    // void setFoo(int _foo){
    //     foo = -1;
    //     foo = _foo;
    // }

    State(){}
    // State(const json &_j)
    // {
    //     parameterList = _j;
    // }

    void initVineyard();
    void simulate();
    void outputData(fs::path path, int threadId);
    void timeStep();

};


// #endif