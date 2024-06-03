#include "LevySearch.h"

template <typename T>
LevySearch<T>::LevySearch(
    int id,
    std::mt19937 &gen, 
    std::uniform_real_distribution<T> &dist_uni,
    std::uniform_real_distribution<T> &dist_x,
    std::uniform_real_distribution<T> &dist_y,
    std::uniform_real_distribution<T> &dist_z,
    T mu_levy, T c_levy,
    const T worldsize[3])
  : id(id), gen(gen), dist_uni(dist_uni), 
  dist_x(dist_x), dist_y(dist_y), dist_z(dist_z),
  mu_levy(mu_levy), c_levy(c_levy)
{
#ifdef DEBUG
  /*
  std::cout << "LevySearch::LevySearch: \
    Creating a Levy search object" << std::endl;
    */
#endif
  // Set random target
  drawTarget();

  // Copy the world size
  for (int i = 0; i < 3; i++){
    this->worldsize[i] = worldsize[i];
  }
}

template <typename T>
LevySearch<T>::~LevySearch()
{
#ifdef DEBUG
  /*
  std::cout << "LevySearch::~LevySearch: \
    Destroying a Levy search object" << std::endl;
    */
#endif
}

template <typename T>
LevySearch<T>& LevySearch<T>::operator=(const LevySearch<T> &search)
{
#ifdef DEBUG
  /*
  std::cout << "LevySearch::operator=: \
    Assigning a Levy search object" << std::endl;
    */
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
  mu_levy = search.mu_levy;
  c_levy = search.c_levy;
  target = search.target;

  return *this;
}

template <typename T>
T LevySearch<T>::rand()
{
  // see: https://en.wikipedia.org/wiki/L%C3%A9vy_distribution#Random-sample_generation

  // Draw a random number from a Levy distribution
  T p = dist_uni(gen);

  // Calculate the Levy distribution in the required precision
  T r_levy = 0.0;
  if (typeid(T) == typeid(float)){
     r_levy = mu_levy + c_levy /
      std::pow(r4_normal_01_cdf_inverse(1.0f - 0.5f*p), 2.0f);
  }else if (typeid(T) == typeid(double)){
     r_levy = mu_levy + c_levy /
      std::pow(r8_normal_01_cdf_inverse(1.0 - 0.5*p), 2.0);
  }else{
    std::cerr << "LevySearch::draw: \
      Unsupported data type for Levy distribution" << std::endl;
    exit(EXIT_FAILURE);
  }
#ifdef DEBUG
  /*
  std::cout << "LevySearch::rand: \
    Drawing a random number from a Levy distribution: " << r_levy << std::endl;
    */
#endif
  return r_levy;
}

template <typename T>
void LevySearch<T>::startNewSearch(const State<T> &state)
{

  // Draw a random target 
  drawTarget();

  // Calculate the direction to the target
  Point3D<T> &current_position = state.positions[id];
  Point3D<T> direction = current_position.normalizedDirection(target);

  // Draw a random number from a Levy distribution
  T r_levy = rand();

  // Scale the direction by the random number
  direction *= r_levy;

  // Recalculate the target
  target = current_position + direction;

#ifdef DEBUG
  /*
  std::cout << "LevySearch::startNewSearch: \
    Starting a new search" << std::endl;
  std::cout << "LevySearch::startNewSearch: \
    Target: " << target.toString() << std::endl;
  std::cout << "LevySearch::startNewSearch: \
    Position: " << current_position.toString() << std::endl;
  std::cout << "LevySearch::startNewSearch: \
    Direction: " << direction.toString() << std::endl;
  std::cout << "LevySearch::startNewSearch: \
    Random number: " << r_levy << std::endl;
  */
#endif

  // Check if the target is within the world
  if (!isWithinWorld(target)){
#ifdef DEBUG
    /*
    std::cout << "LevySearch::startNewSearch: \
      Target is outside the world" << std::endl;
      */
#endif
    target = current_position;
  }

}

template <typename T>
bool LevySearch<T>::isFinished(const State<T> &state)
{
#ifdef DEBUG
  /*
  std::cout << "LevySearch::isFinished: \
    Checking if the search is over" << std::endl;
  std::cout << "LevySearch::isFinished: \
    Target: " << target.toString() 
    << " Position: " << state.positions[id].toString() << std::endl;
    */
#endif

  // Check if the target is in reach
  if (state.positions[id].distance(target) < state.movespeed[id]){
    return true;
  }else{
    return false;
  }

}

template <typename T>
Point3D<T> LevySearch<T>::getDirection(const State<T> &state)
{
#ifdef DEBUG
  /*
  std::cout << "LevySearch::getDirection: \
    Getting the direction to the target" << std::endl;
    */
#endif

  // Calculate the direction to the target
  const Point3D<T> &current_position = state.positions[id];
  return current_position.normalizedDirection(target);
}

template <typename T>
bool LevySearch<T>::isWithinWorld(const Point3D<T> &point)
{
  // Check if the point is inside the world
  if (point.x < -worldsize[0] || point.x > worldsize[0]) return false;
  if (point.y < -worldsize[1] || point.y > worldsize[1]) return false;
  if (point.z < -worldsize[2] || point.z > worldsize[2]) return false;

  return true;
}

template <typename T>
void LevySearch<T>::drawTarget()
{
  // Draw a random target
  target.x = dist_x(gen);
  target.y = dist_y(gen);
  target.z = dist_z(gen);
}

// Explicit instantiation
template class LevySearch<float>;
template class LevySearch<double>;
/* vim: set ts=2 sw=2 tw=0 et :*/
