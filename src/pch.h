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

using json = nlohmann::json;
using std::string;
namespace fs = std::filesystem;
using std::vector;
using std::unique_ptr;
using std::thread;
using std::array;
#define RAND_MAX 10e9