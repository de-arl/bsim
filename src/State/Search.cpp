#include "Search.h"

template <typename T>
Search<T>::Search(
    int id,
    std::mt19937 &gen, 
    std::uniform_real_distribution<T> &dist_uni,
    std::uniform_real_distribution<T> &dist_x,
    std::uniform_real_distribution<T> &dist_y,
    std::uniform_real_distribution<T> &dist_z,
    T mu_levy, T c_levy, T brownian_search_duration,
    const T worldsize[3])
  : levy_search(id, gen, dist_uni, dist_x, dist_y, dist_z, 
      mu_levy, c_levy, worldsize),
  brownian_search(id, gen, dist_x, dist_y, dist_z, brownian_search_duration), 
  brownian_search_duration(brownian_search_duration),
  id(id),
  gen(gen),
  dist_uni(dist_uni), dist_x(dist_x), dist_y(dist_y), dist_z(dist_z)
{
#ifdef DEBUG
 // std::cout << "Search::Search: Creating a search object" << std::endl;
#endif
  
  // Set the search type
  if (brownian_search_duration > 0.0){
    search_type = SearchType::NUM_SEARCH_TYPES;
  }else{
    setSearchType(SearchType::LEVY);
  }
}

template <typename T>
Search<T>::~Search()
{
#ifdef DEBUG
//  std::cout << "Search::~Search: Destroying a search object" << std::endl;
#endif
}

template <typename T>
Search<T>& Search<T>::operator=(const Search<T> &search)
{
#ifdef DEBUG
  //std::cout << "Search::operator=: Assigning a search object" << std::endl;
#endif

  // Check for self-assignment
  if (this == &search){
    return *this;
  }

  // Assign the search object
  id = search.id;
  gen = search.gen;
  dist_uni = search.dist_uni;
  dist_x = search.dist_x;
  dist_y = search.dist_y;
  dist_z = search.dist_z;
  levy_search = search.levy_search;
  brownian_search = search.brownian_search;
  brownian_search_duration = search.brownian_search_duration;
  search_type = search.search_type;

  return *this;
}

template <typename T>
Point3D<T> Search<T>::getDirection(const State<T> &state)
{
#ifdef DEBUG
//  std::cout << "Search::getDirection: Getting a search direction" << std::endl;
#endif

  // Get a direction based on the search type
  if (search_type == SearchType::LEVY){
    return levy_search.getDirection(state);
  }else if (search_type == SearchType::BROWNIAN){
    return brownian_search.getDirection(state);
  }else{
    exit(EXIT_FAILURE);
  }
}

template <typename T>
void Search<T>::determineSearchType(const State<T> &state)
{
#ifdef DEBUG
  /*
  std::cout 
    << "Search::determineSearchType: Determining the search type" << std::endl;
  */
#endif
  // In the first iteration, the agent starts with a brownian search
  if (search_type == SearchType::NUM_SEARCH_TYPES){
    if (brownian_search_duration > 0.0){
      setSearchType(SearchType::BROWNIAN);
      brownian_search.startNewSearch(state);
    }else{
      setSearchType(SearchType::LEVY);
      levy_search.startNewSearch(state);
    }
  }

  // Test if the Levy target is reached
  if (search_type == SearchType::LEVY){

    if (brownian_search_duration > 0.0){
      if (levy_search.isFinished(state)){
        // If the levy search is finished, start a new brownian search
        setSearchType(SearchType::BROWNIAN);
        brownian_search.startNewSearch(state);
      }
    } else {
      // If the brownian search duration is zero, start a new levy search
      if (levy_search.isFinished(state)){
        levy_search.startNewSearch(state);
      }
    }

  }else if (search_type == SearchType::BROWNIAN){

    if (brownian_search.isFinished(state)){
      setSearchType(SearchType::LEVY);
      levy_search.startNewSearch(state);
    }

  }else{
    handleError("Search::determineSearchType: Unsupported search type", 
        CRITICAL);
  }
}

template <typename T>
void Search<T>::setSearchType(SearchType search_type)
{
#ifdef DEBUG
  //std::cout << "Search::setSearchType: Setting the search type" << std::endl;
#endif

  // Set the search type
  this->search_type = search_type;
}

// Explicit instantiation
template class Search<float>;
template class Search<double>;

/* vim: set ts=2 sw=2 tw=0 et :*/
