#ifndef COLOR_H
#define COLOR_H

#include <iostream>
       
template <typename T>
struct Color
{
  // Red, green, blue
  float r;
  float g;
  float b;

  // Default constructor
  Color() : r(0.0f), g(0.0f), b(0.0f) {}

  // Constructor
  Color(T r, T g, T b) : r((float)r), g((float)g), b((float)b) {}

  // Copy constructor
  Color(const Color &c) : r(c.r), g(c.g), b(c.b) {}

  // Assignment operator
  Color& operator=(const Color &c)
  {
    r = c.r;
    g = c.g;
    b = c.b;
    return *this;
  }

  // Comparison operator: equal
  bool operator==(const Color &c)
  {
    return (r == c.r && g == c.g && b == c.b);
  }

  // Operator []
  float& operator[](int i)
  {
    if (i == 0) return r;
    if (i == 1) return g;
    return b;
  }

  // Comparison operator: not equal
  bool operator!=(const Color &c)
  {
    return (r != c.r || g != c.g || b != c.b);
  }

  // Template assignment operator
  Color& operator=(const T &c)
  {
    r = (float)c;
    g = (float)c;
    b = (float)c;
    return *this;
  }

  // Destructor
  ~Color() {}

  // Type cast operator
  operator float*() { return &r; }
  operator const float*() const { return &r; }
  operator double*() { return (double*)&r; }
  operator const double*() const { return (double*)&r; }


  // To string
  std::string toString() const
  {
    return std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b);
  }

  // Operator <<
  friend std::ostream& operator<<(std::ostream &os, const Color &c)
  {
    os << c.r << " " << c.g << " " << c.b;
    return os;
  }

  // Scale color (brightness)
  void scale(T s)
  {
    if (s < 0.0f) s = 0.0f;
    if (s > 1.0f) s = 1.0f;
    r *= s;
    g *= s;
    b *= s;
  }

}; 

// Explicit instantiation
template struct Color<float>;
template struct Color<double>;

#endif //COLOR_H
/* vim: set ts=2 sw=2 tw=0 et :*/
