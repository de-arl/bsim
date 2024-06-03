#ifndef RESULTS_H
#define RESULTS_H
#include "StateMachine.h"
#include <vector>
#include <fstream>
#include <filesystem>
#include <ctime>

template <typename T>
class Results
{
 public:
  Results(StateMachine<T> &state);
  ~Results(){};
  void trackConsumption();
  void trackFoodAmount();
  void writeData();
  std::string output_path;
 
 private:
  StateMachine<T> *sm;
  char timeString[std::size("yyyy-mm-ddThh:mm:ssZ")];
  T accumulator;
  std::vector<T> food_uptake_r_;
  std::vector<T> food_uptake_non_r_;
  std::vector<T> total_food_amount_;
  std::vector<T> avg_food_amount_;

  void writeToFile
    (std::vector<T> data, int num_elements, std::string filename);
  void initDirectory();
  void writeIterationSummary();
  void writeRoosterSummary();
  void writeNonRoosterSummary();
  void createGraphs();
  void setTime(); //RFC 3339 format UTC time

  friend class StateMachine<T>;
};
#endif
