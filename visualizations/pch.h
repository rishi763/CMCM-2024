#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <array>
#include <cstdlib>
#include <sstream>
#include <string>
#include <functional>
#include <ctime>
#include "gnuplot.h"

using std::string;
namespace fs = std::filesystem;
using std::vector;
using std::unique_ptr;
using std::thread;
using std::array;
using std::function;
// #define RAND_MAX 10e9