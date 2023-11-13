#include <iostream>
#include <fstream>
#include <filesystem>
#include "json.hpp"
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <array>
#include <cstdlib>
#include <sstream>
#include <ctime>
#include <string>
#include <functional>
#include "gnuplot.h"
#include <queue>
#include <mutex>

using json = nlohmann::json;
using std::string;
namespace fs = std::filesystem;
using std::vector;
using std::unique_ptr;
using std::thread;
using std::array;
using std::function;
using std::mutex;
// #define RAND_MAX 10e9