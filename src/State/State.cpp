#include "State.h"

template <typename T>
State<T>::State()
  : num_agents_total(0),
  verbosity(0),
  num_classes(0),
  iteration(0),
  run_counter(0),
  time(0),
  positions(NULL),
  energy(NULL),
  energy_consumption_per_distance(NULL),
  energy_consumption_per_time(NULL),
  energy_uptake_rate(NULL),
  movespeed(NULL),
  sizes(NULL),
  status(NULL),
  class_id(NULL),
  strategy(NULL),
  distances_agent_to_agent(NULL),
  distances_agent_to_class(NULL),
  distances_class_to_class(NULL),
  audibility(NULL),
  audibility_agent_to_class(NULL),
  audibility_class_to_class(NULL),
  audibility_threshold(NULL),
  reachability(NULL),
  reachability_agent_to_class(NULL),
  reachability_class_to_class(NULL),
  contact(NULL),
  contact_agent_to_class(NULL),
  contact_class_to_class(NULL),
  initialized(false)

{
#ifdef DEBUG
  std::cout << "State::State: \
    Default constructor of State" << std::endl;
#endif
  
}

template <typename T>
State<T>::State(const State& other)
  : num_agents_total(other.num_agents_total),
  iteration(other.iteration),
  run_counter(other.run_counter),
  num_classes(other.num_classes),
  time(other.time),
  verbosity(other.verbosity),
  initialized(other.initialized)
{
#ifdef DEBUG
  std::cout << "State::State: \
    Copy constructor of State" << std::endl;
#endif

  // Copy pointers and data
  positions = other.positions;
  energy = other.energy;
  energy_consumption_per_distance = other.energy_consumption_per_distance;
  energy_consumption_per_time = other.energy_consumption_per_time;
  energy_uptake_rate = other.energy_uptake_rate;
  movespeed = other.movespeed;
  sizes = other.sizes;
  status = other.status;
  class_id = other.class_id;
  strategy = other.strategy;

  distances_agent_to_agent = other.distances_agent_to_agent;
  distances_agent_to_class = other.distances_agent_to_class;
  distances_class_to_class = other.distances_class_to_class;

  audibility_threshold = other.audibility_threshold;
  audibility = other.audibility;
  audibility_agent_to_class = other.audibility_agent_to_class;
  audibility_class_to_class = other.audibility_class_to_class;

  reachability = other.reachability;
  reachability_agent_to_class = other.reachability_agent_to_class;
  reachability_class_to_class = other.reachability_class_to_class;

  contact = other.contact;
  contact_agent_to_class = other.contact_agent_to_class;
  contact_class_to_class = other.contact_class_to_class;
}


template <typename T>
State<T>::State(State&& other)
{
#ifdef DEBUG
  std::cout << "State::State: \
    Move constructor of State" << std::endl;
#endif
  // Moves resources from other to this
  // Leaves other in a valid but unspecified state
  
  // Free memory
  bool other_initialized = other.initialized;
  int other_num_agents = other.num_agents_total;
  int other_num_classes = other.num_classes;
  int other_iteration = other.iteration;
  int other_run_counter = other.run_counter;
  int other_verbosity = other.verbosity;
  double other_time = other.time;
  other.time = 0;
  other.initialized = false;
  other.num_agents_total = 0;
  other.num_classes = 0;
  other.iteration = 0;
  other.run_counter = 0;
  other.verbosity = 0;
  this->initialized = false;
  this->num_agents_total = 0;
  this->num_classes = 0;
  this->iteration = 0;
  this->run_counter = 0;
  this->time = 0;
  freeMemory();

  // Move pointers
  positions = other.positions;
  energy = other.energy;
  energy_consumption_per_distance = other.energy_consumption_per_distance;
  energy_consumption_per_time = other.energy_consumption_per_time;
  energy_uptake_rate = other.energy_uptake_rate;
  movespeed = other.movespeed;
  sizes = other.sizes;
  status = other.status;
  class_id = other.class_id;
  strategy = other.strategy;

  distances_agent_to_agent = other.distances_agent_to_agent;
  distances_agent_to_class = other.distances_agent_to_class;
  distances_class_to_class = other.distances_class_to_class;

  audibility = other.audibility;
  audibility_agent_to_class = other.audibility_agent_to_class;
  audibility_class_to_class = other.audibility_class_to_class;
  audibility_threshold = other.audibility_threshold;

  reachability = other.reachability;
  reachability_agent_to_class = other.reachability_agent_to_class;
  reachability_class_to_class = other.reachability_class_to_class;

  contact = other.contact;
  contact_agent_to_class = other.contact_agent_to_class;
  contact_class_to_class = other.contact_class_to_class;


  // Reset other
  other.positions = NULL;
  other.energy = NULL;
  other.energy_consumption_per_distance = NULL;
  other.energy_consumption_per_time = NULL;
  other.energy_uptake_rate = NULL;
  other.movespeed = NULL;
  other.sizes = NULL;
  other.status = NULL;
  other.class_id = NULL;
  other.strategy = NULL;

  other.distances_agent_to_agent = NULL;
other.distances_agent_to_class = NULL;
  other.distances_class_to_class = NULL;

  other.audibility = NULL;
  other.audibility_agent_to_class = NULL;
  other.audibility_class_to_class = NULL;
  other.audibility_threshold = NULL;

  other.reachability = NULL;
  other.reachability_agent_to_class = NULL;
  other.reachability_class_to_class = NULL;

  other.contact = NULL;
  other.contact_agent_to_class = NULL;
  other.contact_class_to_class = NULL;

  // Set initialized flag and member variables
  this->num_agents_total = other_num_agents;
  this->num_classes = other_num_classes;
  this->iteration = other_iteration;
  this->run_counter = other_run_counter;
  this->time = other_time;
  this->verbosity = other_verbosity;
  this->initialized = other_initialized;
}

template <typename T>
State<T>::State(int num_agents_total, int num_classes, int verbosity)
  : num_agents_total(num_agents_total), num_classes(num_classes), 
  verbosity(verbosity), iteration(0), run_counter(0), time(0),
  positions(NULL), energy(NULL), energy_consumption_per_distance(NULL),
  energy_consumption_per_time(NULL), energy_uptake_rate(NULL),
  movespeed(NULL), sizes(NULL), status(NULL), class_id(NULL),
  strategy(NULL), distances_agent_to_agent(NULL),
  distances_agent_to_class(NULL), distances_class_to_class(NULL),
  audibility(NULL), audibility_agent_to_class(NULL),
  audibility_class_to_class(NULL), audibility_threshold(NULL),
  reachability(NULL), reachability_agent_to_class(NULL),
  reachability_class_to_class(NULL), contact(NULL),
  contact_agent_to_class(NULL), contact_class_to_class(NULL),
  initialized(false)
{
#ifdef DEBUG
  std::cout << "State::State: \
    Memory allocating constructor of State" << std::endl;
#endif
  
    // Allocate memory
    allocateMemory(num_agents_total, num_classes);
}

template <typename T>
State<T>::~State()
{
#ifdef DEBUG
  if (verbosity > 4){
    std::cout << "State::~State: \
      Destructor of State" << std::endl;
  }
#endif
  
    // Free memory
    freeMemory();
}

template <typename T>
State<T>& State<T>::operator=(State&& other)
{
#ifdef DEBUG
  if (verbosity > 4){
  std::cout << "State::operator=: \
    Move assignment operator of State" << std::endl;
  }
#endif

  // Move assignment operator
  // The order of the following statements is important
  if (this != &other){

    // Free memory
    initialized = false;
    this->num_agents_total = 0;
    this->num_classes = 0;
    this->iteration = 0;
    this->run_counter = 0;
    this->time = 0;
    this->verbosity = 0;
    freeMemory();

    // Reset other
    int other_num_agents = other.num_agents_total;
    int other_num_classes = other.num_classes;
    int other_iteration = other.iteration;
    int other_run_counter = other.run_counter;
    double other_time = other.time;
    int other_verbosity = other.verbosity;
    bool other_initialized = other.initialized;

    other.num_agents_total = 0;
    other.num_classes = 0;
    other.iteration = 0;
    other.run_counter = 0;
    other.time = 0;
    other.verbosity = 0;
    other.initialized = false;

    /* Move data */

    // Distances agent to agent
    if (other.distances_agent_to_agent != NULL) {
      distances_agent_to_agent = other.distances_agent_to_agent;
      other.distances_agent_to_agent = NULL;
    } else{
      distances_agent_to_agent = NULL;
    }

    // Audibility agent to agent
    if (other.audibility != NULL) {
      audibility = other.audibility;
      other.audibility = NULL;
    } else{
      audibility = NULL;
    }

    // Reachability agent to agent
    if (other.reachability != NULL) {
      reachability = other.reachability;
      other.reachability = NULL;
    } else{
      reachability = NULL;
    }

    // Contact agent to agent
    if (other.contact != NULL) {
      contact = other.contact;
      other.contact = NULL;
    } else{
      contact = NULL;
    }

    // Distances agent to class
    if (other.distances_agent_to_class != NULL) {
      distances_agent_to_class = other.distances_agent_to_class;
      other.distances_agent_to_class = NULL;
    } else{
      distances_agent_to_class = NULL;
    }


    // Audibility agent to class
    if (other.audibility_agent_to_class != NULL) {
      audibility_agent_to_class = other.audibility_agent_to_class;
      other.audibility_agent_to_class = NULL;
    } else{
      audibility_agent_to_class = NULL;
    }

    // Reachability agent to class
    if (other.reachability_agent_to_class != NULL) {
      reachability_agent_to_class = other.reachability_agent_to_class;
      other.reachability_agent_to_class = NULL;
    } else{
      reachability_agent_to_class = NULL;
    }

    
    // Positions
    if (other.positions != NULL) {
      positions = other.positions;
      other.positions = NULL;
    } else{
      positions = NULL;
    }

    // Energy
    if (other.energy != NULL) {
      energy = other.energy;
      other.energy = NULL;
    } else{
      energy = NULL;
    }

    // Energy consumption per distance
    if (other.energy_consumption_per_distance != NULL) {
      energy_consumption_per_distance = other.energy_consumption_per_distance;
      other.energy_consumption_per_distance = NULL;
    } else{
      energy_consumption_per_distance = NULL;
    }

    // Energy consumption per time
    if (other.energy_consumption_per_time != NULL) {
      energy_consumption_per_time = other.energy_consumption_per_time;
      other.energy_consumption_per_time = NULL;
    } else{
      energy_consumption_per_time = NULL;
    }

    // Energy uptake rate
    if (other.energy_uptake_rate != NULL) {
      energy_uptake_rate = other.energy_uptake_rate;
      other.energy_uptake_rate = NULL;
    } else{
      energy_uptake_rate = NULL;
    }

    // Sizes
    if (other.sizes != NULL) {
      sizes = other.sizes;
      other.sizes = NULL;
    } else{
      sizes = NULL;
    }

    // Status
    if (other.status != NULL) {
      status = other.status;
      other.status = NULL;
    } else{
      status = NULL;
    }

    // Class ID
    if (other.class_id != NULL) {
      class_id = other.class_id;
      other.class_id = NULL;
    } else{
      class_id = NULL;
    }

    // Strategy
    if (other.strategy != NULL) {
      strategy = other.strategy;
      other.strategy = NULL;
    } else{
      strategy = NULL;
    }

    // Movement speed
    if (other.movespeed != NULL) {
      movespeed = other.movespeed;
      other.movespeed = NULL;
    } else{
      movespeed = NULL;
    }

    // Audibility threshold
    if (other.audibility_threshold != NULL) {
      audibility_threshold = other.audibility_threshold;
      other.audibility_threshold = NULL;
    } else{
      audibility_threshold = NULL;
    }

    // Contact agent to class
    if (other.contact_agent_to_class != NULL) {
      contact_agent_to_class = other.contact_agent_to_class;
      other.contact_agent_to_class = NULL;
    } else{
      contact_agent_to_class = NULL;
    }
    
    // Distances class to class
    if (other.distances_class_to_class != NULL) {
      distances_class_to_class = other.distances_class_to_class;
      other.distances_class_to_class = NULL;
    } else{
      distances_class_to_class = NULL;
    }

    // Audibility class to class
    if (other.audibility_class_to_class != NULL) {
      audibility_class_to_class = other.audibility_class_to_class;
      other.audibility_class_to_class = NULL;
    } else{
      audibility_class_to_class = NULL;
    }

    // Reachability class to class
    if (other.reachability_class_to_class != NULL) {
      reachability_class_to_class = other.reachability_class_to_class;
      other.reachability_class_to_class = NULL;
    } else{
      reachability_class_to_class = NULL;
    }

    // Contact class to class
    if (other.contact_class_to_class != NULL) {
      contact_class_to_class = other.contact_class_to_class;
      other.contact_class_to_class = NULL;
    } else{
      contact_class_to_class = NULL;
    }

    // Set initialized flag
    this->num_agents_total = other_num_agents;
    this->num_classes = other_num_classes;
    this->iteration = other_iteration;
    this->run_counter = other_run_counter;
    this->time = other_time;
    this->verbosity = other_verbosity;
    this->initialized = other_initialized;
  }
  return *this;
}

template <typename T>
State<T>& State<T>::operator=(const State& other)
{
#ifdef DEBUG
  std::cout << "State::operator=: \
    Copy assignment operator of State" << std::endl;
#endif

  // Free memory
  freeMemory();

  // Num agents
  this->num_agents_total = other.num_agents_total;
  this->num_classes = other.num_classes;
  this->iteration = other.iteration;
  this->run_counter = other.run_counter;
  this->time = other.time;
  this->verbosity = other.verbosity;
  this->initialized = other.initialized;

  // Allocate memory
  allocateMemory(num_agents_total, num_classes);

  // Copy data
  for (int i=0; i<num_agents_total; i++){
    positions[i] = other.positions[i];
    energy[i] = other.energy[i];
    energy_consumption_per_distance[i] = 
      other.energy_consumption_per_distance[i];
    energy_consumption_per_time[i] = other.energy_consumption_per_time[i];
    energy_uptake_rate[i] = other.energy_uptake_rate[i];
    movespeed[i] = other.movespeed[i];
    sizes[i] = other.sizes[i];
    status[i] = other.status[i];
    class_id[i] = other.class_id[i];
    strategy[i] = other.strategy[i];
    audibility_threshold[i] = other.audibility_threshold[i];
  }

  for (int i=0; i<num_agents_total*num_agents_total; i++){
    distances_agent_to_agent[i] = other.distances_agent_to_agent[i];
    audibility[i] = other.audibility[i];
    reachability[i] = other.reachability[i];
    contact[i] = other.contact[i];
  }

  for (int i=0; i<num_agents_total*num_classes; i++){
    distances_agent_to_class[i] = other.distances_agent_to_class[i];
    audibility_agent_to_class[i] = other.audibility_agent_to_class[i];
    reachability_agent_to_class[i] = other.reachability_agent_to_class[i];
    contact_agent_to_class[i] = other.contact_agent_to_class[i];
  }

  for (int i=0; i<num_classes*num_classes; i++){
    distances_class_to_class[i] = other.distances_class_to_class[i];
    audibility_class_to_class[i] = other.audibility_class_to_class[i];
    reachability_class_to_class[i] = other.reachability_class_to_class[i];
    contact_class_to_class[i] = other.contact_class_to_class[i];
  }

  return *this;
}

template <typename T>
void State<T>::allocateMemory(int num_agents_total, int num_classes)
{
#ifdef DEBUG
  std::cout << "State::allocateMemory: \
    Allocating memory for the state" << std::endl;
  std::cout << "State::allocateMemory: \
    Number of agents: " << num_agents_total << std::endl;
#endif

  // Free memory
  freeMemory(); 

  // Set the number of agents
  this->num_agents_total = num_agents_total;

  // Set the number of classes
  this->num_classes = num_classes;

  // Alignment for SIMD compiler optimizations in bytes
  size_t alignment = 16;

  int size = num_agents_total;

  // Pad size to be a multiple of alignment
  size = (size + alignment - 1) & ~(alignment - 1);

  // If size is a power of 2, add alignment
  while (isPowerOfTwo(size)){
    size += alignment;
  }

#ifdef DEBUG
  if (verbosity > 4){
    std::cout << "State::allocateMemory: \
      num_agents_total: " << num_agents_total << std::endl;
    std::cout << "State::allocateMemory: \
      Allocated memory size: " << size << std::endl;
  }
#endif

  // Distances agent to agent
  distances_agent_to_agent = 
    (T *)aligned_alloc(alignment, size*size*sizeof(T));

  // Audibility agent to agent
  audibility = (int *)aligned_alloc(alignment, size*size*sizeof(int));

  // Reachability agent to agent
  reachability = (int *)aligned_alloc(alignment, size*size*sizeof(int));

  // Contact agent to agent
  contact = (int *)aligned_alloc(alignment, size*size*sizeof(int));

  // Distances agent to class
  distances_agent_to_class = 
    (T *)aligned_alloc(alignment, size*num_classes*sizeof(T));

  // Audibility agent to class
  audibility_agent_to_class =
    (int *)aligned_alloc(alignment, size*num_classes*sizeof(int));

  // Reachability agent to class
  reachability_agent_to_class =
    (int *)aligned_alloc(alignment, size*num_classes*sizeof(int));

  // Contact agent to class
  contact_agent_to_class =
    (int *)aligned_alloc(alignment, size*num_classes*sizeof(int));

  // Positions
  positions = 
    (Point3D<T> *)aligned_alloc(alignment, size*sizeof(Point3D<T>));

  // Energy
  energy = (T *)aligned_alloc(alignment, size*sizeof(T));

  // Energy consumption per distance
  energy_consumption_per_distance = 
    (T *)aligned_alloc(alignment, size*sizeof(T));

  // Energy consumption per time
  energy_consumption_per_time = 
    (T *)aligned_alloc(alignment, size*sizeof(T));

  // Energy uptake rate
  energy_uptake_rate = 
    (T *)aligned_alloc(alignment, size*sizeof(T));

  // Sizes
  sizes = (T *)aligned_alloc(alignment, size*sizeof(T));

  // Status
  status = (int *)aligned_alloc(alignment, size*sizeof(int));

  // Class ID 
  class_id = (int *)aligned_alloc(alignment, size*sizeof(int));

  // Strategy
  strategy = (int *)aligned_alloc(alignment, size*sizeof(int));

  // Movement speed
  movespeed = 
    (T *)aligned_alloc(alignment, size*sizeof(T));

  // Audibility threshold
  audibility_threshold = (T *)aligned_alloc(alignment, size*sizeof(T));


  // Change padding size to fit the number of classes
  // Pad size to be a multiple of alignment
  size = num_classes;
  size = (size + alignment - 1) & ~(alignment - 1);

  // If size is a power of 2, add alignment
  while (isPowerOfTwo(size)){
    size += alignment;
  }

  // Class to class distances
  distances_class_to_class =
    (T *)aligned_alloc(alignment, num_classes*num_classes*sizeof(T));

  // Class to class audibility
  audibility_class_to_class = 
    (int *)aligned_alloc(alignment, num_classes*num_classes*sizeof(int));

  // Class to class reachability
  reachability_class_to_class = 
    (int *)aligned_alloc(alignment, num_classes*num_classes*sizeof(int));

  // Class to class contact
  contact_class_to_class = 
    (int *)aligned_alloc(alignment, num_classes*num_classes*sizeof(int));

}

template <typename T>
void State<T>::freeMemory()
{
#ifdef DEBUG
  if (verbosity > 4){
    std::cout << "State::freeMemory: \
      Freeing memory of the state" << std::endl;
  }
#endif

  /* Agent to agent variables */

  // Distances agent to agent
  if (distances_agent_to_agent != NULL) {
    free(distances_agent_to_agent);
    distances_agent_to_agent = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing distances agent to agent" << std::endl;
    }
#endif
  }

  // Audibility agent to agent
  if (audibility != NULL) {
    free(audibility);
    audibility = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing audibility" << std::endl;
    }
#endif
  }

  // Reachability agent to agent
  if (reachability != NULL) {
    free(reachability);
    reachability = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing reachability" << std::endl;
    }
#endif
  }

  // Contact agent to agent
  if (contact != NULL) {
    free(contact);
    contact = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing contact" << std::endl;
    }
#endif
  }

  /* Agent to class variables */

  // Distances agent to class
  if (distances_agent_to_class != NULL) {
    free(distances_agent_to_class);
    distances_agent_to_class = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing distances agent to class" << std::endl;
    }
#endif
  }

  // Audibility agent to class
  if (audibility_agent_to_class != NULL) {
    free(audibility_agent_to_class);
    audibility_agent_to_class = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing audibility agent to class" << std::endl;
    }
#endif
  }

  // Reachability agent to class
  if (reachability_agent_to_class != NULL) {
    free(reachability_agent_to_class);
    reachability_agent_to_class = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing reachability agent to class" << std::endl;
    }
#endif
  }

  // Contact agent to class
  if (contact_agent_to_class != NULL) {
    free(contact_agent_to_class);
    contact_agent_to_class = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing contact agent to class" << std::endl;
    }
#endif
  } 


  /* Agent variables */

  // Positions
  if (positions != NULL) {
    free(positions);
    positions = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing positions" << std::endl;
    }
#endif
  }

  // Energy
  if (energy != NULL) {
    free(energy);
    energy = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing energy" << std::endl;
    }
#endif
  }

  // Energy consumption per distance
  if (energy_consumption_per_distance != NULL) {
    free(energy_consumption_per_distance);
    energy_consumption_per_distance = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing energy consumption per distance" << std::endl;
    }
#endif
  }

  // Energy consumption per time
  if (energy_consumption_per_time != NULL) {
    free(energy_consumption_per_time);
    energy_consumption_per_time = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing energy consumption per time" << std::endl;
    }
#endif
  }

  // Energy uptake rate
  if (energy_uptake_rate != NULL) {
    free(energy_uptake_rate);
    energy_uptake_rate = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing energy uptake rate" << std::endl;
    }
#endif
  }

  // Sizes
  if (sizes != NULL) {
    free(sizes);
    sizes = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing sizes" << std::endl;
    }
#endif
  }

  // Status
  if (status != NULL) {
    free(status);
    status = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing status" << std::endl;
    }
#endif
  }

  // Class ID
  if (class_id != NULL) {
    free(class_id);
    class_id = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing class ID" << std::endl;
    }
#endif
  } 

  // Strategy
  if (strategy != NULL) {
    free(strategy);
    strategy = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing strategy" << std::endl;
    }
#endif
  }

  // Movement speed
  if (movespeed != NULL) {
    free(movespeed);
    movespeed = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing movement speed" << std::endl;
    }
#endif
  }

  // Audibility threshold
  if (audibility_threshold != NULL) {
    free(audibility_threshold);
    audibility_threshold = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing audibility threshold" << std::endl;
    }
#endif
  }



  /* Class to class variables */

  // Distances class to class
  if (distances_class_to_class != NULL) {
    free(distances_class_to_class);
    distances_class_to_class = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing distances class to class" << std::endl;
    }
#endif
  }

  // Audibility class to class
  if (audibility_class_to_class != NULL) {
    free(audibility_class_to_class);
    audibility_class_to_class = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing audibility class to class" << std::endl;
    }
#endif
  }

  // Reachability class to class
  if (reachability_class_to_class != NULL) {
    free(reachability_class_to_class);
    reachability_class_to_class = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing reachability class to class" << std::endl;
    }
#endif
  }

  // Contact class to class
  if (contact_class_to_class != NULL) {
    free(contact_class_to_class);
    contact_class_to_class = NULL;
#ifdef DEBUG
    if (verbosity > 6){
      std::cout << "State::freeMemory: \
        Freeing contact class to class" << std::endl;
    }
#endif
  }

}

template <typename T>
void State<T>::zeroMemory()
{
#ifdef DEBUG
  std::cout << "State::zeroMemory: \
    Zeroing memory of the state" << std::endl;
#endif

  for (int i=0; i<num_agents_total; i++){
    positions[i].setZero();
    energy[i] = (T)0.0;
    energy_consumption_per_distance[i] = (T)0.0;
    energy_consumption_per_time[i] = (T)0.0;
    energy_uptake_rate[i] = (T)0.0;
    movespeed[i] = (T)0.0;
    sizes[i] = (T)0.0;
    status[i] = (T)0.0;
    class_id[i] = (T)0.0;
    strategy[i] = (T)0.0;
    audibility_threshold[i] = (T)0.0;
  }

  for (int i=0; i<num_agents_total*num_agents_total; i++){
    distances_agent_to_agent[i] = (T)0.0;
    audibility[i] = (T)0.0;
    reachability[i] = (T)0.0;
    contact[i] = (T)0.0;
  }

  for (int i=0; i<num_agents_total*num_classes; i++){
    distances_agent_to_class[i] = (T)0.0;
    audibility_agent_to_class[i] = (T)0.0;
    reachability_agent_to_class[i] = (T)0.0;
    contact_agent_to_class[i] = (T)0.0;
  }

  for (int i=0; i<num_classes*num_classes; i++){
    distances_class_to_class[i] = (T)0.0;
    audibility_class_to_class[i] = (T)0.0;
    reachability_class_to_class[i] = (T)0.0;
    contact_class_to_class[i] = (T)0.0;
  }

}

template <typename T>
int State<T>::getNumAgentsTotal() const
{
#ifdef DEBUG
  std::cout << "State::getNumAgentsTotal: \
    Number of agents: " << num_agents_total << std::endl;
#endif

  // Get the number of agents
  return num_agents_total;
}

template <typename T>
void State<T>::setNumAgentsTotal(int num_agents_total)
{
#ifdef DEBUG
  std::cout << "State::setNumAgentsTotal: \
    Setting the number of agents" << std::endl;
#endif

  // Set the number of agents
  this->num_agents_total = num_agents_total;
}

template <typename T>
T* State<T>::getPositions() const
{
#ifdef DEBUG
  /*
  std::cout << "State::getPositions: \
    Getting positions" << std::endl;
    */
#endif

  // Get the positions
  return &positions[0].x;
}

template <typename T>
void State<T>::setPositions(Point3D<T>* positions)
{
#ifdef DEBUG
  std::cout << "State::setPositions: \
    Setting positions" << std::endl;
#endif

  // Set the positions
  this->positions = positions;
}

template <typename T>
T* State<T>::getEnergy() const
{
#ifdef DEBUG
  std::cout << "State::getEnergy: \
    Getting energy" << std::endl;
#endif

  // Get the energy
  return energy;
}

template <typename T>
void State<T>::setEnergy(T* energy)
{
#ifdef DEBUG
  std::cout << "State::setEnergy: \
    Setting energy" << std::endl;
#endif

  // Set the energy
  this->energy = energy;
}

template <typename T>
T State<T>::getIteration() const
{
#ifdef DEBUG
  if (verbosity > 6){
    std::cout << "State::getIteration: \
      Getting iteration" << std::endl;
  }
#endif

  // Get the iteration
  return iteration;
}

template <typename T>
void State<T>::setIteration(T iteration)
{
#ifdef DEBUG
  std::cout << "State::setIteration: \
    Setting iteration: " << iteration << std::endl;
#endif

  // Set the iteration
  this->iteration = iteration;
}

template <typename T>
void State<T>::setRunCounter(int run_counter)
{
#ifdef DEBUG
  std::cout << "State::setRunCounter: \
    Setting run counter" << std::endl;
#endif

  // Set the run
  this->run_counter = run_counter;
}

template <typename T>
int State<T>::getRunCounter() const
{
#ifdef DEBUG
  std::cout << "State::getRunCounter: \
    Getting run counter" << std::endl;
#endif

  // Get the run
  return run_counter;
}

template <typename T>
double State<T>::getTimestamp() const
{
#ifdef DEBUG
  if (verbosity > 6){
    std::cout << "State::getTimestamp: \
      Getting time" << std::endl;
  }
#endif

  // Get the time
  return time;
}

template <typename T>
void State<T>::setTimestamp(double time)
{
#ifdef DEBUG
  std::cout << "State::setTimestamp: \
    Setting time" << std::endl;
#endif

  // Set the time
  this->time = time;
}

template <typename T>
int State<T>::getVerbosity() const
{
#ifdef DEBUG
  std::cout << "State::getVerbosity: \
    Getting verbosity" << std::endl;
#endif

  // Get the verbosity
  return verbosity;
}

template <typename T>
void State<T>::setVerbosity(int verbosity)
{
#ifdef DEBUG
  std::cout << "State::setVerbosity: \
    Setting verbosity" << std::endl;
#endif

  // Set the verbosity
  this->verbosity = verbosity;
}

template <typename T>
bool State<T>::getInitialized() const
{
#ifdef DEBUG
  /*
  std::cout << "State::getInitialized: \
    Getting initialized flag = " << initialized << std::endl;
    */
#endif

  // Get the initialized flag
  return initialized;
}

template <typename T>
void State<T>::setInitialized(bool initialized)
{
#ifdef DEBUG
  std::cout << "State::setInitialized: \
    Setting initialized flag = " << initialized << std::endl;
#endif

  // Set the initialized flag
  this->initialized = initialized;
}

template <typename T>
void State<T>::calculateDistances()
{
#ifdef DEBUG
  std::cout << "State::calculateDistances: \
    Calculating distances" << std::endl;
#endif

  // Calculate distances between individual agents
  for (int i=0; i<num_agents_total; i++){
    for (int j=0; j<num_agents_total; j++){
      distances_agent_to_agent[i*num_agents_total+j] = 
        positions[i].distance(positions[j]);
    }
  }
}

template <typename T>
void State<T>::calculateAudibility()
{
#ifdef DEBUG
  std::cout << "State::calculateAudibility: \
    Calculating audibility" << std::endl;
#endif
  // i hears j = true 

  // Calculate audibility
  for (int i=0; i<num_agents_total; i++){
    for (int j=0; j<num_agents_total; j++){
      if (distances_agent_to_agent[i*num_agents_total+j] 
          < audibility_threshold[j]){
        audibility[i*num_agents_total+j] = 1;
      } else{
        audibility[i*num_agents_total+j] = 0;
      }
      if (i == j){
        audibility[i*num_agents_total+j] = 0;
      }
    }
  }
}

template <typename T>
void State<T>::calculateReachability()
{
#ifdef DEBUG
  std::cout << "State::calculateReachability: \
    Calculating reachability" << std::endl;
#endif

  // Calculate reachability
  for (int i=0; i<num_agents_total; i++){
    for (int j=0; j<num_agents_total; j++){
      if (distances_agent_to_agent[i*num_agents_total+j] < movespeed[i]){
        reachability[i*num_agents_total+j] = 1;
      } else{
        reachability[i*num_agents_total+j] = 0;
      }
      if (i == j){
        reachability[i*num_agents_total+j] = 0;
      }
    }
  }
}

template <typename T>
void State<T>::calculateContact()
{
#ifdef DEBUG
  std::cout << "State::calculateContact: \
    Calculating contact" << std::endl;
#endif

  // Calculate contact
  for (int i=0; i<num_agents_total; i++){
    for (int j=0; j<num_agents_total; j++){
      if (distances_agent_to_agent[i*num_agents_total+j] 
          < 0.5 *(sizes[i]+sizes[j])){
        contact[i*num_agents_total+j] = 1;
      } else{
        contact[i*num_agents_total+j] = 0;
      }
      if (i == j){
        contact[i*num_agents_total+j] = 0;
      }
    }
  }
}

template <typename T>
T State<T>::sumEnergy() const
{
#ifdef DEBUG
  std::cout << "State::sumEnergy: \
    Calculating overall sum of energy in the system" << std::endl;
#endif

  T sum = 0.0;
  int start = 0;
  calculateSum(energy, start, num_agents_total, sum);
#ifdef DEBUG
  std::cout << "State::sumEnergy: \
    Sum of energy: " << sum << std::endl;
#endif
  return sum;
}

template <typename T>
void State<T>::calculateAverage(T* data, int start, int end, T& avg) const
{
#ifdef DEBUG
  /*
  std::cout << "State::calculateAverage: \
    Calculating aqverage" << std::endl;
    */
#endif

  // Calculate average
  T sum = 0.0;
//OMP_PLACEHOLDER
#pragma omp for reduction(+:sum) num_threads(4)
  for (int i=start; i<end; i++){
    sum += data[i];
  }
  if (end-start > 0.0){ 
    avg = sum/(T)(end-start);
  } else{
    avg = 0.0;
  }
}

template <typename T>
inline void State<T>::calculateSum(const int* data, 
    const int& start, const int& end, int& sum) const
{
#ifdef DEBUG
 /* 
  std::cout << "State::calculateSum: \
    Calculating sum" << std::endl;
  std::cout << "State::calculateSum: \
    Start: " << start << std::endl;
  std::cout << "State::calculateSum: \
    End: " << end << std::endl;
    */
#endif

  // Calculate sum 
  sum = 0;
#pragma omp for reduction(+:sum) num_threads(4) simd
  for (int i=start; i<end; i++){
    sum += data[i];
  }
}

template <typename T>
inline void State<T>::calculateSum(const T* data, const int& start, const int& end, T& sum) const
{
#ifdef DEBUG
 /* 
  std::cout << "State::calculateSum: \
    Calculating sum" << std::endl;
  std::cout << "State::calculateSum: \
    Start: " << start << std::endl;
  std::cout << "State::calculateSum: \
    End: " << end << std::endl;
    */
#endif

  // Calculate sum 
  sum = 0.0;
#pragma omp for reduction(+:sum) num_threads(4) simd
  for (int i=start; i<end; i++){
    sum += data[i];
  }
}

template <typename T>
void State<T>::countTrue(int* data, int start, int end, int& count) const
{
#ifdef DEBUG
  /*
  std::cout << "State::countTrue: \
    Counting true values" << std::endl;
    */
#endif

  // Count true values
  count = 0;
//OMP_PLACEHOLDER
  for (int i=start; i<end; i++){
    if (data[i]){
      count++;
    }
  }
}

template <typename T>
void State<T>::printPositions(std::ostream &os, int dim, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printPositions: \
    Printing positions" << std::endl;
  std::cout << "State::printPositions: \
    Number of agents: " << num_agents_total << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
#ifdef DEBUG
    std::cout << "State::printPositions: \
      State is not initialized" << std::endl;
#endif
    return;
  }

  // Print positions
  os << iteration << sep;
  for (int i=0; i<num_agents_total; i++){
    positions[i].print(os, dim, sep);
    os << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printEnergy(std::ostream &os, bool append, 
    std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printEnergy: \
    Printing energy" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print energy
    os << "Energy" << std::endl;
  }

  // Print energy
  os << iteration << sep;
  for (int i=0; i<num_agents_total; i++){
    os << energy[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printDistances(std::ostream &os, 
    bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printDistances: \
    Printing distances" << std::endl;
#endif
  
  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print distances
    os << "Distances agent to agent" << std::endl;
  }
  
  // Print distances
  int num_elements = num_agents_total*num_agents_total;
  os << iteration << sep;
  for (int i=0; i<num_elements; i++){
    os << distances_agent_to_agent[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printDistancesAgentToClass(
    std::ostream &os, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printDistancesAgentToClass: \
    Printing distances agent to class" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print distances agent to class
    os << "Distances agent to class" << std::endl;
  }

  // Print distances agent to class
  int num_elements = num_classes*num_agents_total;
  os << iteration << sep;
  for (int i=0; i<num_elements; i++){
    os << distances_agent_to_class[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printDistancesClassToClass(std::ostream &os, 
    bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printDistancesClassToClass: \
    Printing distances class to class" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print distances class to class
    os << "Distances class to class" << std::endl;
  }

  // Print distances class to class
  int num_elements = num_classes*num_classes;
  os << iteration << sep;
  for (int i=0; i<num_elements; i++){
    os << distances_class_to_class[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printAudibility(std::ostream &os, 
    bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printAudibility: \
    Printing audibility" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print audibility
    os << "Audibility" << std::endl;
  }

  // Print audibility
  int num_elements = num_agents_total*num_agents_total;
  os << iteration << sep;
  for (int i=0; i<num_elements; i++){
    os << audibility[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printAudibilityAgentToClass(std::ostream &os, bool append, 
    std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printAudibilityAgentToClass: \
    Printing audibility agent to class" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print audibility agent to class
    os << "Audibility agent to class" << std::endl;
  }

  // Print audibility agent to class
  int num_elements = num_classes*num_agents_total;
  os << iteration << sep;
  for (int i=0; i<num_elements; i++){
    os << audibility_agent_to_class[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printAudibilityClassToClass(std::ostream &os, 
    bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printAudibilityClassToClass: \
    Printing audibility class to class" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print audibility class to class
    os << "Audibility class to class" << std::endl;
  }

  // Print audibility class to class
  int num_elements = num_classes*num_classes;
  os << iteration << sep;
  for (int i=0; i<num_elements; i++){
    os << audibility_class_to_class[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printReachability(std::ostream &os, 
    bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printReachability: \
    Printing reachability" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print reachability
    os << "Reachability" << std::endl;
  }

  // Print reachability
  int num_elements = num_agents_total*num_agents_total;
  os << iteration << sep;
  for (int i=0; i<num_elements; i++){
    os << reachability[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printReachabilityAgentToClass(std::ostream &os, bool append, 
    std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printReachabilityAgentToClass: \
    Printing reachability agent to class" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print reachability agent to class
    os << "Reachability agent to class" << std::endl;
  }

  // Print reachability agent to class
  int num_elements = num_classes*num_agents_total;
  os << iteration << sep;
  for (int i=0; i<num_elements; i++){
    os << reachability_agent_to_class[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printReachabilityClassToClass(std::ostream &os, bool append,
    std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printReachabilityClassToClass: \
    Printing reachability class to class" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print reachability class to class
    os << "Reachability class to class" << std::endl;
  }

  // Print reachability class to class
  int num_elements = num_classes*num_classes;
  os << iteration << sep;
  for (int i=0; i<num_elements; i++){
    os << reachability_class_to_class[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printContact(std::ostream &os, 
    bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printContact: \
    Printing contact" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print contact
    os << "Contact" << std::endl;
  }

  // Print contact
  int num_elements = num_agents_total*num_agents_total;
  os << iteration << sep;
  for (int i=0; i<num_elements; i++){
    os << contact[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printContactAgentToClass(std::ostream &os, bool append, 
    std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printContactAgentToClass: \
    Printing contact agent to class" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print contact agent to class
    os << "Contact agent to class" << std::endl;
  }

  // Print contact agent to class
  int num_elements = num_classes*num_agents_total;
  os << iteration << sep;
  for (int i=0; i<num_elements; i++){
    os << contact_agent_to_class[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printContactClassToClass(std::ostream &os, 
    bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printContactClassToClass: \
    Printing contact class to class" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print contact class to class
    os << "Contact class to class" << std::endl;
  }

  // Print contact class to class
  int num_elements = num_classes*num_classes;
  os << iteration << sep;
  for (int i=0; i<num_elements; i++){
    os << contact_class_to_class[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printSizes(std::ostream &os, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printSizes: \
    Printing sizes" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print sizes
    os << "Sizes" << std::endl;
  }

  // Print sizes
  os << iteration << sep;
  for (int i=0; i<num_agents_total; i++){
    os << sizes[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printStatus(std::ostream &os, 
    bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printStatus: \
    Printing status" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print status
    os << "Agent_status" << std::endl;
  }

  // Print status
  os << iteration << sep;
  for (int i=0; i<num_agents_total; i++){
    os << status[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printClassId(std::ostream &os, 
    bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printClassID: \
    Printing class ID" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print class ID
    os << "Class ID" << std::endl;
  }

  // Print class ID
  os << iteration << sep;
  for (int i=0; i<num_agents_total; i++){
    os << class_id[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printMovespeed(std::ostream &os, 
    bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printMovespeed: \
    Printing movement speed" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print movement speed
    os << "Movement speed" << std::endl;
  }

  // Print movement speed
  os << iteration << sep;
  for (int i=0; i<num_agents_total; i++){
    os << movespeed[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printAudibilityThreshold(std::ostream &os, 
    bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printAudibilityThreshold: \
    Printing audibility threshold" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print audibility threshold
    os << "Audibility threshold" << std::endl;
  }

  // Print audibility threshold
  os << iteration << sep;
  for (int i=0; i<num_agents_total; i++){
    os << audibility_threshold[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printEnergyConsumptionPerDistance(std::ostream &os, bool append,
    std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printEnergyConsumptionPerDistance: \
    Printing energy consumption per distance" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print energy consumption per distance
    os << "Energy consumption per distance" << std::endl;
  }

  // Print energy consumption per distance
  os << iteration << sep;
  for (int i=0; i<num_agents_total; i++){
    os << energy_consumption_per_distance[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printEnergyConsumptionPerTime(std::ostream &os, bool append, 
    std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printEnergyConsumptionPerTime: \
    Printing energy consumption per time" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print energy consumption per time
    os << "Energy consumption per time" << std::endl;
  }

  // Print energy consumption per time
  os << iteration << sep;
  for (int i=0; i<num_agents_total; i++){
    os << energy_consumption_per_time[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printEnergyUptakeRate(std::ostream &os, bool append, 
    std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printEnergyUptakeRate: \
    Printing energy uptake rate" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print energy uptake rate
    os << "Energy uptake rate" << std::endl;
  }

  // Print energy uptake rate
  os << iteration << sep;
  for (int i=0; i<num_agents_total; i++){
    os << energy_uptake_rate[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printClassIdToml(std::ostream &os) const
{
#ifdef DEBUG
  std::cout << "State::printClassIdToml: \
    Printing class ID in TOML format" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  // Print class ID
  os << "  agent_class_id = [ ";
  os << class_id[0];
  for (int i=1; i<num_agents_total; i++){
    os << ", " <<  class_id[i];
  }
  os << " ]" << std::endl;
}

template <typename T>
void State<T>::printStrategyToml(std::ostream &os) const
{
#ifdef DEBUG
  std::cout << "State::printStrategyToml: \
    Printing strategy in TOML format" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  // Print strategy
  os << "  agent_strategy = [ ";
  os << strategy[0];
  for (int i=1; i<num_agents_total; i++){
    os << ", " <<  strategy[i];
  }
  os << " ]" << std::endl;
}

template <typename T>
void State<T>::printStrategy(std::ostream &os, 
    bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "State::printStrategy: \
    Printing strategy" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  if (!append){
    // Print strategy
    os << "Strategy" << std::endl;
  }

  // Print strategy
  for (int i=0; i<num_agents_total; i++){
    os << strategy[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void State<T>::printSizesToml(std::ostream &os) const
{
#ifdef DEBUG
  std::cout << "State::printSizesToml: \
    Printing sizes in TOML format" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  // Print sizes
  os << "  agent_sizes = [ ";
  os << sizes[0];
  for (int i=1; i<num_agents_total; i++){
    os << ", " <<  sizes[i];
  }
  os << " ]" << std::endl;
}

template <typename T>
void State<T>::printStatusToml(std::ostream &os) const
{
#ifdef DEBUG
  std::cout << "State::printStatusToml: \
    Printing status in TOML format" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  // Print status
  os << "  agent_status = [ ";
  os << status[0];
  for (int i=1; i<num_agents_total; i++){
    os << ", " <<  status[i];
  }
  os << " ]" << std::endl;
}

template <typename T>
void State<T>::printMovespeedToml(std::ostream &os) const
{
#ifdef DEBUG
  std::cout << "State::printMovespeedToml: \
    Printing movement speed in TOML format" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  // Print movement speed
  os << "  agent_movespeed = [ ";
  os << movespeed[0];
  for (int i=1; i<num_agents_total; i++){
    os << ", " <<  movespeed[i];
  }
  os << " ]" << std::endl;
}

template <typename T>
void State<T>::printAudibilityThresholdToml(std::ostream &os) const
{
#ifdef DEBUG
  std::cout << "State::printAudibilityThresholdToml: \
    Printing audibility threshold in TOML format" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  // Print audibility threshold
  os << "  agent_audibility_threshold = [ ";
  os << audibility_threshold[0];
  for (int i=1; i<num_agents_total; i++){
    os << ", " <<  audibility_threshold[i];
  }
  os << " ]" << std::endl;
}

template <typename T>
void State<T>::printEnergyConsumptionPerDistanceToml(std::ostream &os) const
{
#ifdef DEBUG
  std::cout << "State::printEnergyConsumptionPerDistanceToml: \
    Printing energy consumption per distance in TOML format" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  // Print energy consumption per distance
  os << "  agent_energy_consumption_per_distance = [ ";
  os << energy_consumption_per_distance[0];
  for (int i=1; i<num_agents_total; i++){
    os << ", " <<  energy_consumption_per_distance[i];
  }
  os << " ]" << std::endl;
}

template <typename T>
void State<T>::printEnergyConsumptionPerTimeToml(std::ostream &os) const
{
#ifdef DEBUG
  std::cout << "State::printEnergyConsumptionPerTimeToml: \
    Printing energy consumption per time in TOML format" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  // Print energy consumption per time
  os << "  agent_energy_consumption_per_time = [ ";
  os << energy_consumption_per_time[0];
  for (int i=1; i<num_agents_total; i++){
    os << ", " <<  energy_consumption_per_time[i];
  }
  os << " ]" << std::endl;
}

template <typename T>
void State<T>::printEnergyUptakeRateToml(std::ostream &os) const
{
#ifdef DEBUG
  std::cout << "State::printEnergyUptakeRateToml: \
    Printing energy uptake rate in TOML format" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  // Print energy uptake rate
  os << "  agent_energy_uptake_rate = [ ";
  os << energy_uptake_rate[0];
  for (int i=1; i<num_agents_total; i++){
    os << ", " <<  energy_uptake_rate[i];
  }
  os << " ]" << std::endl;
}

template <typename T>
void State<T>::printState(std::ostream &os, bool append, 
    std::string sep, int verbosity) const
{
#ifdef DEBUG
  std::cout << "State::printState: \
    Printing state" << std::endl;
  std::cout << "State::printState: \
    Verbosity level: " << verbosity << std::endl;
#endif

  if (!append){
    os << "State" << std::endl;
    os << "Iteration: " << iteration << std::endl;
    os << "Run counter: " << run_counter << std::endl;
    os << "Timestamp: " << time << std::endl;
    os << "Number of agents: " << num_agents_total << std::endl;
    os << "Number of classes: " << num_classes << std::endl;
    os << "Initialized: " << initialized << std::endl;
  }

  // If uninitialized, return
  if (!initialized){
    return;
  }

  // Print state
  if (verbosity > 1){
    if (!append){
      os << "Positions" << std::endl;
    }
    printPositions(os, 3, sep);
    printEnergy(os, append, sep);
    printSizes(os, append, sep);
    printStatus(os, append, sep);
    printClassId(os, append, sep);
    printStrategy(os, append, sep);
    printMovespeed(os, append, sep);
    printAudibilityThreshold(os, append, sep);
    printEnergyConsumptionPerDistance(os, append, sep);
    printEnergyConsumptionPerTime(os, append, sep);
    printEnergyUptakeRate(os, append, sep);
  }

  // Print class to class variables
  if (verbosity > 2){  
    printDistancesClassToClass(os, append, sep);
    printAudibilityClassToClass(os, append, sep);
    printReachabilityClassToClass(os, append, sep);
    printContactClassToClass(os, append, sep);
  }

  // Print agent to class variables
  if (verbosity > 3){
    printDistancesAgentToClass(os, append, sep);
    printAudibilityAgentToClass(os, append, sep);
    printReachabilityAgentToClass(os, append, sep);
    printContactAgentToClass(os, append, sep);
  }

  // Print agent to agent variables
  if (verbosity > 4){
    printDistances(os, append, sep);
    printAudibility(os, append, sep);
    printReachability(os, append, sep);
    printContact(os, append, sep);
  }
    
}

template <typename T>
void State<T>::printStateToml(std::ostream &os) const
{
#ifdef DEBUG
  std::cout << "State::printStateToml: \
    Printing state in TOML format" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  // Print number of agents and classes
  os << "  num_agents_total = " << num_agents_total << std::endl;
  os << "  num_classes = " << num_classes << std::endl;
  os << "  iteration = " << iteration << std::endl;
  os << "  run_counter = " << run_counter << std::endl;
  os << "  time = " << time << std::endl;
  os << "  initialized = " << initialized << std::endl;
  os << std::endl;

  // Print state in TOML format
  printPositionsToml(os);
  printEnergyToml(os);
  printSizesToml(os);
  printStatusToml(os);
  printClassIdToml(os);
  printStrategyToml(os);
  printMovespeedToml(os);
  printAudibilityThresholdToml(os);
  printEnergyConsumptionPerDistanceToml(os);
  printEnergyConsumptionPerTimeToml(os);
  printEnergyUptakeRateToml(os);

}

template <typename T>
void State<T>::loadStateToml(std::string filename)
{
#ifdef DEBUG
  std::cout << "State::loadStateToml: \
    Loading state from TOML file: " << filename << std::endl;
#endif

  // open file
  toml::table stored_state;
  try {
    stored_state = toml::parse_file(filename);
#ifdef DEBUG
    std::cout << "State::loadStateToml: \
      File loaded successfully" << std::endl;
    //std::cout << stored_state << std::endl;
#endif
  } catch (const toml::parse_error &e) {
    std::cerr << e.what() << std::endl;
    throw(e);
  }

  // If no state is stored, return
  if (!stored_state.contains("simulation_state")){
    handleError("State::loadStateToml: \
      No state stored in file", WARNING);
    return;
  }

  // Set number of agents and classes
  num_agents_total = 
    stored_state["simulation_state"]["num_agents_total"].value_or(0);
  num_classes = 
    stored_state["simulation_state"]["num_classes"].value_or(0);
  iteration = 
    stored_state["simulation_state"]["iteration"].value_or(0);
  run_counter = 
    stored_state["simulation_state"]["run_counter"].value_or(0);
  time = 
    stored_state["simulation_state"]["time"].value_or(0);
  initialized = 
    stored_state["simulation_state"]["initialized"].value_or(0);

  // Allocate memory
  allocateMemory(num_agents_total, num_classes);

  // Load state
  loadPositions(stored_state);
  loadEnergy(stored_state);
  loadSizes(stored_state);
  loadStatus(stored_state);
  loadClassId(stored_state);
  loadStrategy(stored_state);
  loadMovespeed(stored_state);
  loadAudibilityThreshold(stored_state);
  loadEnergyConsumptionPerDistance(stored_state);
  loadEnergyConsumptionPerTime(stored_state);
  loadEnergyUptakeRate(stored_state);

}

template <typename T>
void State<T>::printPositionsToml(std::ostream &os) const
{
#ifdef DEBUG
  std::cout << "State::printPositionsToml: \
    Printing positions in TOML format" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  // Print positions
  os << "  agent_positions_x = [";
  os << positions[0].x;
  for (int i=1; i<num_agents_total; i++){
    os << ", " <<  positions[i].x;
  }
  os << "]" << std::endl;

  os << "  agent_positions_y = [";
  os << positions[0].y;
  for (int i=1; i<num_agents_total; i++){
    os << ", " <<  positions[i].y;
  }
  os << " ]" << std::endl;

  os << "  agent_positions_z = [ ";
  os << positions[0].z;
  for (int i=1; i<num_agents_total; i++){
    os << ", " <<  positions[i].z;
  }
  os << "]" << std::endl;
}

template <typename T>
void State<T>::printEnergyToml(std::ostream &os) const
{
#ifdef DEBUG
  std::cout << "State::printEnergyToml: \
    Printing energy in TOML format" << std::endl;
#endif

  // If uninitialized, return
  if (!initialized){
    return;
  }

  // Print energy
  os << "  agent_energy = [ ";
  os << energy[0];
  for (int i=1; i<num_agents_total; i++){
    os << ", " <<  energy[i];
  }
  os << " ]" << std::endl;
}

template <typename T>
void State<T>::loadPositions(const toml::table &table)
{
#ifdef DEBUG
  std::cout << "State::loadPositions: \
    Loading positions" << std::endl;
#endif
  // Test if the arrays are contained in the table
  if (!table.contains("simulation_state")){
    handleError("State::loadPositions: \
      No positions stored in file", CRITICAL);
    return;
  }

  if (!table["simulation_state"].as_table()->contains("agent_positions_x")){
    handleError("State::loadPositions: \
      No x positions stored in file", CRITICAL);
    return;
  }

  if (!table["simulation_state"].as_table()->contains("agent_positions_y")){
    handleError("State::loadPositions: \
      No y positions stored in file", CRITICAL);
    return;
  }

  if (!table["simulation_state"].as_table()->contains("agent_positions_z")){
    handleError("State::loadPositions: \
      No z positions stored in file", CRITICAL);
    return;
  }

  // Load positions
  for (int i=0; i<num_agents_total; i++){
    T x = table["simulation_state"]["agent_positions_x"][i].value_or(0.0f);
    T y = table["simulation_state"]["agent_positions_y"][i].value_or(0.0f);
    T z = table["simulation_state"]["agent_positions_z"][i].value_or(0.0f);
    this->positions[i] = Point3D<T>(x, y, z);
  }
}

template <typename T>
void State<T>::loadEnergy(const toml::table &table)
{
#ifdef DEBUG
  std::cout << "State::loadEnergy: \
    Loading energy" << std::endl;
#endif
  // Test if agent_energy is contained in the table
  if (!table.contains("simulation_state")){
    handleError("State::loadEnergy: \
      No energy stored in file", CRITICAL);
    return;
  }

  if (!table["simulation_state"].as_table()->contains("agent_energy")){
    handleError("State::loadEnergy: \
      No energy stored in file", CRITICAL);
    return;
  }

  // Load energy
  for (int i=0; i<num_agents_total; i++){
    this->energy[i] = 
      table["simulation_state"]["agent_energy"][i].value_or(0.0);
  }
}

template <typename T>
void State<T>::loadSizes(const toml::table &table)
{
#ifdef DEBUG
  std::cout << "State::loadSizes: \
    Loading sizes" << std::endl;
#endif

  // Load sizes
  for (int i=0; i<num_agents_total; i++){
    this->sizes[i] = table["simulation_state"]["agent_sizes"][i].value_or(0.0);
  }
} 

template <typename T>
void State<T>::loadStatus(const toml::table &table)
{
#ifdef DEBUG
  std::cout << "State::loadStatus: \
    Loading status" << std::endl;
#endif

  // Load status
  for (int i=0; i<num_agents_total; i++){
    this->status[i] = 
      table["simulation_state"]["agent_status"][i].value_or(0);
  }
}

template <typename T>
void State<T>::loadClassId(const toml::table &table)
{
#ifdef DEBUG
  std::cout << "State::loadClassId: \
    Loading class ID" << std::endl;
#endif

  // Load class ID
  for (int i=0; i<num_agents_total; i++){
    this->class_id[i] = 
      table["simulation_state"]["agent_class_id"][i].value_or(0);
  }
}

template <typename T>
void State<T>::loadStrategy(const toml::table &table)
{
#ifdef DEBUG
  std::cout << "State::loadStrategy: \
    Loading strategy" << std::endl;
#endif

  // Load strategy
  for (int i=0; i<num_agents_total; i++){
    this->strategy[i] = 
      table["simulation_state"]["agent_strategy"][i].value_or(0);
  }
}

template <typename T>
void State<T>::loadMovespeed(const toml::table &table)
{
#ifdef DEBUG
  std::cout << "State::loadMovespeed: \
    Loading movement speed" << std::endl;
#endif

  // Load movement speed
  for (int i=0; i<num_agents_total; i++){
    this->movespeed[i] = 
      table["simulation_state"]["agent_movespeed"][i].value_or(0.0);
  }
}

template <typename T>
void State<T>::loadAudibilityThreshold(const toml::table &table)
{
#ifdef DEBUG
  std::cout << "State::loadAudibilityThreshold: \
    Loading audibility threshold" << std::endl;
#endif

  // Load audibility threshold
  for (int i=0; i<num_agents_total; i++){
    this->audibility_threshold[i] = 
      table["simulation_state"]["agent_audibility_threshold"][i].value_or(0.0);
  }
}

template <typename T>
void State<T>::loadEnergyConsumptionPerDistance(const toml::table &table)
{
#ifdef DEBUG
  std::cout << "State::loadEnergyConsumptionPerDistance: \
    Loading energy consumption per distance" << std::endl;
#endif

  // Load energy consumption per distance
  for (int i=0; i<num_agents_total; i++){
    this->energy_consumption_per_distance[i] = 
      table["simulation_state"]["agent_energy_consumption_per_distance"][i].value_or(0.0);
  }
}

template <typename T>
void State<T>::loadEnergyConsumptionPerTime(const toml::table &table)
{
#ifdef DEBUG
  std::cout << "State::loadEnergyConsumptionPerTime: \
    Loading energy consumption per time" << std::endl;
#endif

  // Load energy consumption per time
  for (int i=0; i<num_agents_total; i++){
    this->energy_consumption_per_time[i] = 
      table["simulation_state"]["agent_energy_consumption_per_time"][i].value_or(0.0);
  }
}

template <typename T>
void State<T>::loadEnergyUptakeRate(const toml::table &table)
{
#ifdef DEBUG
  std::cout << "State::loadEnergyUptakeRate: \
    Loading energy uptake rate" << std::endl;
#endif

  // Load energy uptake rate
  for (int i=0; i<num_agents_total; i++){
    this->energy_uptake_rate[i] = 
      table["simulation_state"]["agent_energy_uptake_rate"][i].value_or(0.0);
  }
}

// Explicit instantiation
template struct State<float>;
template struct State<double>;

/* vim: set ts=2 sw=2 tw=0 et :*/
