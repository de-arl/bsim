#ifndef POINT3D_CPP
#define POINT3D_CPP

#include <iostream>
#include <cmath>
#include "helpers.h"

template <typename T>
struct Point3D
{
    
    // Default constructor
    Point3D() : x(0.0), y(0.0), z(0.0) {};

    // Position init constructor
    Point3D(T x, T y, T z) : x(x), y(y), z(z) {};

    // Member variables
    T x, y, z;

    // Print the point
    void print() 
    { std::cout << "x: " << x << " y: " << y << " z: " << z << std::endl; }

    // Print the point to a stream
    void print(std::ostream &os, int dim, std::string sep = " ") 
    { 
      if (dim ==0) {
        os << x;
      } else if (dim == 1) {
        os << y;
      } else if (dim == 2) {
        os << z;
      } else {
        os << x << sep << y << sep << z;
      }
    }

    // Define operator [] to access the point coordinates
    inline T& operator[](int i) 
    { return i == 0 ? x : (i == 1 ? y : z); }

    // Get the point coordinates
    inline void get(T* coords) 
    { coords[0] = x; coords[1] = y; coords[2] = z; } 

    // Set the point coordinates
    inline void set(T* coords) 
    { x = coords[0]; y = coords[1]; z = coords[2]; }

    // Define operator * to scale the point
    inline Point3D operator*(T scale) 
    { return Point3D(x*scale, y*scale, z*scale); }

    // Define operator + to add two points
    inline Point3D operator+(const Point3D<T> &p) 
    { return Point3D(x+p.x, y+p.y, z+p.z); }

    // Define operator - to subtract two points
    inline Point3D operator-(const Point3D<T> &p) 
    { return Point3D(x-p.x, y-p.y, z-p.z); }

    // Define operator += to add a point to another point
    inline Point3D operator+=(const Point3D<T> &p) 
    { x += p.x; y += p.y; z += p.z; return *this; }

    // Define operator += to add a float to every dimension
    inline Point3D operator+=(const float &f)
    { x += f; y += f; z += f; return *this; }

    // Define operator -= to subtract a point from another point
    inline Point3D operator-=(const Point3D<T> &p) 
    { x -= p.x; y -= p.y; z -= p.z; return *this; }

    // Define operator *= to scale the point
    inline Point3D operator*=(T scale) 
    { x *= scale; y *= scale; z *= scale; return *this; }

    // Define operator /= to scale the point
    inline Point3D operator/=(T scale) 
    { x /= scale; y /= scale; z /= scale; return *this; }

    // Define operator = to set the point to another point
    inline Point3D operator=(const Point3D<T> &p) 
    { x = p.x; y = p.y; z = p.z; return *this; }

    // Define operator == to compare two points of floating point type
    // with a tolerance
    inline bool operator==(const Point3D<T> &p) {
      return nearlyEqual(x, p.x) && nearlyEqual(y, p.y) && nearlyEqual(z, p.z);
    }

    // Return the squared distance between two points
    inline T distanceSquared(const Point3D<T>& p) const
    { return (x-p.x)*(x-p.x) + (y-p.y)*(y-p.y) + (z-p.z)*(z-p.z); }

    // Return the distance between two points
    inline T distance(const Point3D<T> &p) 
    { return sqrt((x-p.x)*(x-p.x) + (y-p.y)*(y-p.y) + (z-p.z)*(z-p.z)); }

    // Return the direction from this point to another point
    inline Point3D direction(const Point3D<T> &p) 
    { return Point3D(p.x-x, p.y-y, p.z-z); }

    // Return the squared norm of the point
    inline T squaredNorm() const
    { return x*x + y*y + z*z; }

    // Return the 2-norm of the point
    inline T norm() const
    { return std::sqrt(x*x + y*y + z*z); }

    // Normalize the point
    inline void normalize()
    { 
      T n = norm();
      x /= n; y /= n; z /= n;
    }
    
    // Normalized direction
    inline Point3D normalizedDirection(const Point3D<T> &p) const
    { 
      // Test if the points are the same
      if (nearlyEqual(p.x, x) && nearlyEqual(p.y, y) && nearlyEqual(p.z, z)) {
#ifdef DEBUG
        std::cout << "Point3D::normalizedDirection: \
          The points are the same. \nThis: "<< this->toString() 
          << "\nOther: " << p.toString() << std::endl;
#endif
        return Point3D(0, 0, 0);
      }
      T d = 1.0/sqrt((p.x-x)*(p.x-x) + (p.y-y)*(p.y-y) + (p.z-z)*(p.z-z)); 
      return Point3D((p.x-x)*d, (p.y-y)*d, (p.z-z)*d);
    }

    // Function to move a point for a given distance in the direction 
    // of another point
    inline Point3D move(const Point3D<T> &p, T distance) 
    { 
      T scale = 
        distance/sqrt((p.x-x)*(p.x-x) + (p.y-y)*(p.y-y) + (p.z-z)*(p.z-z)); 
      return Point3D(
          x + (p.x-x)*scale, 
          y + (p.y-y)*scale, 
          z + (p.z-z)*scale);
    }

    // Point to string
    inline std::string toString() const
    { 
      return std::to_string(x) + ", " 
           + std::to_string(y) + ", " 
           + std::to_string(z); 
    }

    // Point to csv string
    inline std::string toCsvString() const
    { 
      return std::to_string(x) + "," 
           + std::to_string(y) + "," 
           + std::to_string(z); 
    }

    // Set zero
    inline void setZero()
    { x = 0.0; y = 0.0; z = 0.0; }

    // Test if the point is zero
    inline bool isZero() const
    { return nearlyEqual(x, (T)0.0) 
          && nearlyEqual(y, (T)0.0) 
          && nearlyEqual(z, (T)0.0); }

};

// Explicit instantiation of the Point3D struct
template struct Point3D<float>;
template struct Point3D<double>;

#endif // POINT3D_CPP


/* vim: set ts=2 sw=2 tw=0 et :*/
