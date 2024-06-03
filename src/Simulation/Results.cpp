#include "Results.h"
/*
Results::Results(StateMachine &state)
{
  sm = &state;
  std::strcpy(timeString, sm->timeString);
  output_path = sm->output_path;
}

void Results::trackConsumption()
{
}

void Results::trackFoodAmount()
{
}

void Results::writeToFile
(std::vector<float> data, int num_elements, std::string filename)
{
  std::ofstream file (filename);
  if (file.is_open()){
    for (int i=0; i<num_elements; i++){
      file << data[i] <<std::endl;
    }
  }
}

void Results::initDirectory()
{
  if (!std::filesystem::exists(sm->basename_output)){
    std::filesystem::create_directory(sm->basename_output);
  };
  if (!std::filesystem::exists(output_path)){
    std::filesystem::create_directory(output_path);
    std::string config_filename = 
      output_path + "simulation_configuration_" + timeString + ".txt";
    if (!std::filesystem::exists(config_filename)){
      std::filesystem::copy_file(sm->simulation_config_file, config_filename);
    }
#ifdef ANIMATION
    std::string animation_config_filename = 
      output_path + "animation_configuration_" + timeString + ".txt";
    if (!std::filesystem::exists(config_filename)){
      std::filesystem::copy_file(
          sm->animation_config_file, animation_config_filename);
    }
#endif
  }
}

void Results::writeIterationSummary()
{
  std::string filename = output_path + "data_iteration_summary_" + timeString + ".txt";
  std::ofstream file (filename);
  if (file.is_open()){
    file
      << "iteration" << ","
      << "food_uptake_roosters" << ","
      << "food_uptake_non_roosters" << ","
      << "avg_consumption_roosters" << ","
      << "avg_consumption_non_roosters" << ","
      << "total_food_amount" << ","
      << "avg_food_amount" << ","
      << "avg_search_distance_roosters" << ","
      << "avg_search_distance_non_roosters" << ","
      << "avg_approach_distance_roosters" << ","
      << "avg_approach_distance_non_roosters" 
      << std::endl;
    for (int i=0; i<sm->num_iter; i++){
      file 
        << i <<","
        << food_uptake_r_.at(i) << "," 
        << food_uptake_non_r_.at(i) << "," 
        << total_food_amount_.at(i) << ","
        << avg_food_amount_.at(i) << ","
        << std::endl;
    }
  }
}

void Results::writeRoosterSummary()
{
  std::string filename = output_path + "data_individual_summary_roosters_" + timeString + ".txt";
  std::ofstream file (filename);
  if (file.is_open()){
    file
      << "individual" << ","
      << "total_food_consumption_roosters" << ","
      << "total_search_distance_roosters" << ","
      << "total_approach_distance_roosters"
      << std::endl;
    for (int i=0; i<sm->num_roosters; i++){
      file 
        << i <<","
        << std::endl;
    }
  }
}

void Results::writeNonRoosterSummary()
{
  std::string filename = 
    output_path + "data_individual_summary_non_roosters_" + timeString + ".txt";
  std::ofstream file (filename);
  if (file.is_open()){
    file
      << "individual" << ","
      << "total_food_consumption_non_roosters" << ","
      << "total_search_distance_non_roosters" << ","
      << "total_approach_distance_non_roosters"
      << std::endl;
    for (int i=sm->num_roosters; i<sm->num_animals; i++){
      file 
        << i << ","
        << std::endl;
    }
  }
}

void Results::createGraphs()
{
  if (std::filesystem::exists(sm->rscript)){
    std::string name = "parseOutput_";
    std::string scriptname = name + timeString + ".R";
    if (std::filesystem::copy_file(sm->rscript, output_path + scriptname)){
      std::system(
          (
           "cd " + output_path + ";" 
           + "Rscript --vanilla " + scriptname + " " + timeString + " 2&>/dev/null"
          ).c_str());
    }
  }
}

void Results::writeData()
{
  if (!sm->write_results) return;
  Results::initDirectory();
  Results::writeIterationSummary();
  Results::writeRoosterSummary();
  Results::writeNonRoosterSummary();
  if (sm->create_graphs) Results::createGraphs();
}


void Results::setTime()
{
  std::time_t time = std::time({});
  std::strftime(std::data(timeString), std::size(timeString),
                "%FT%TZ", std::gmtime(&time));
}
*/
/* vim: set ts=2 sw=2 tw=0 et :*/
