#ifndef IMGUISTREAM_H
#define IMGUISTREAM_H

#include "imgui.h"
#include <iostream>
#include <streambuf>
#include <memory>

/* Stream class to redirect a stream to ImGui */
// Buffer
class ImGuiStreamBuf : public std::streambuf
{
 public:

  // Constructor
  ImGuiStreamBuf(int size) : buffer_size(size), buffer_pos(0){
    buffer = std::make_unique<char[]>(buffer_size);
  }

  // Overflow function
  virtual int_type overflow(int_type c)
  {
    // Add character to buffer
    if (c != EOF)
    {
      buffer[buffer_pos++] = c;
      // If newline or buffer full, update text
      if (buffer_pos == buffer_size - 1)
      {
        buffer[buffer_pos] = '\0';
        buffer_pos = 0;
      }
    }
    return c;
  }

  // Get the buffer
  const char *getBuffer() const
  {
    return buffer.get();
  }

 private:
  int buffer_size;
  std::unique_ptr<char[]> buffer;
  int buffer_pos;
};

// Stream
class ImGuiStream : public std::ostream
{
public:

  // Constructor
  ImGuiStream(int size) : std::ostream(&streambuf), streambuf(size)
  {
    // Backup cout buffer
    coutbuf = std::cout.rdbuf();

    // Copy cout buffer to stream buffer
    std::cout.rdbuf(&streambuf);
  }

  // Destructor
  ~ImGuiStream()
  {
    // Restore cout buffer
    std::cout.rdbuf(coutbuf);
  }

  // Redirect cout buffer to stream buffer
  void redirect()
  {
    std::cout.rdbuf(&streambuf);
  }

  // Redirect cout buffer to cout buffer
  void restore()
  {
    std::cout.rdbuf(coutbuf);
  }

  // Get the stream buffer
  const char *getBuffer() const
  {
    return streambuf.getBuffer();
  }

private:
  ImGuiStreamBuf streambuf;
  std::streambuf *coutbuf;
};

#endif // IMGUISTREAM_H
