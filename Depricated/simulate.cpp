#include "pch.h"
#include "state.h"

class threadFunctionLibrary{
    State state;
    json parameterList;
    public:

    threadFunctionLibrary(State& _state, json& _parameterList){
        state = _state;
        parameterList = _parameterList;
    }

    
};