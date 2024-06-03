#ifndef HELPERS_H
#define HELPERS_H

#define INFO 2
#define CRITICAL 1
#define WARNING 0

#include <limits>
#include <typeinfo>
#include <iostream>

#ifdef ANIMATION
#include <omp.h>
#endif

// Function to compare two floating point numbers
template <typename T>
bool static nearlyEqual(T a, T b)
  // see https://floating-point-gui.de/errors/comparison/
{
#ifdef DEBUG
  /*
  std::cout <<
  "nearlyEqual: Comparing floating point numbers: a: " << a 
    << " b: " << b << std::endl;
    */
#endif
  T abs_a, abs_b, diff, eps, min, max;
  min = std::numeric_limits<T>::min();
  max = std::numeric_limits<T>::max();
  eps = std::numeric_limits<T>::epsilon();
  abs_a = std::abs(a); abs_b = std::abs(b); diff = std::abs(a - b);

  if (a==b){ 
#ifdef DEBUG
    //std::cout << "nearlyEqual: The numbers are equal (1)" << std::endl;
#endif
    return true;
  }

  if (a==0 | b==0 || (abs_a + abs_b < min) ){

    if (diff < (eps * min)){
#ifdef DEBUG
      //std::cout << "nearlyEqual: The numbers are equal (2)" << std::endl;
#endif
      return true;

    } else {
#ifdef DEBUG
      //std::cout << "nearlyEqual: The numbers are not equal(1)" << std::endl;
#endif
      return false;
    }

  } else {

    if (diff / std::min((abs_a + abs_b), max) < eps){
#ifdef DEBUG
      //std::cout << "nearlyEqual: The numbers are equal (3)" << std::endl;
#endif
      return true;

    } else {
#ifdef DEBUG
      //std::cout << "nearlyEqual: The numbers are not equal(2)" << std::endl;
#endif
      return false;
    }
  }

}

// Function to compare two arrays of floating point numbers
template <typename T>
bool static nearlyEqualArray(T* a, T* b, int num_elems)
{
  for (int i=0; i<num_elems; i++){
    if (!nearlyEqual(a[i], b[i])) {
      std::cout<<"a:"<<a[i]<<"b:"<<b[i]<<std::endl;
      return false;
    }
  }
  return true;
}

template <typename T> 
void copyArray(T* a, T* b, int num_elems)
{
  for (int i=0; i<num_elems; i++){
   *(b+i)  = *(a+i);
  }
}

// Function to check if a number is a power of two
bool static isPowerOfTwo(int x)
{
#ifdef DEBUG
  /*
  std::cout << "isPowerOfTwo: Checking if the number is a power of two: " 
    << x << std::endl;
    */
#endif
  return (x & (x - 1)) == 0;
}

// Function template to print an array
template <typename T>
void static printArray(const T *array, int n, int m)
{
  if (typeid(T) == typeid(bool)){
    for (int i=0; i<n; i++){
      for (int j=0; j<m; j++){
        // convert bool to int
        std::cout << (int)array[i*m + j] << " ";
      }
      std::cout << std::endl;
    }
    std::cout<<std::endl;
    return;
  }

  for (int i=0; i<n; i++){
    for(int j=0; j<m; j++){
      std::cout << array[i*m + j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout<<std::endl;
}

// Error handling function
void static handleError(std::string message, bool type)
{
  if (type == CRITICAL){
    std::cerr << "Error: " << message << std::endl;
    exit(1);
  } else if (type == WARNING){
    std::cerr << "Warning: " << message << std::endl;
  } else if (type == INFO){
    std::cout << "Info: " << message << std::endl;
  }
}


#ifdef ANIMATION
void static delay(int milliseconds)
{
  double start_time = omp_get_wtime();
  while ((omp_get_wtime() - start_time)*1000 < milliseconds);
  return;
}
#endif  

#endif //HELPERS_H
