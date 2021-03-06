#ifndef ARN_IO_FILE_STREAM_HPP
#define ARN_IO_FILE_STREAM_HPP

#include <fstream>

#include "Stream.hpp"

namespace arn {
namespace io {

class InFileStream : public InStream, public SeekableStream {
protected:
  mutable std::ifstream strm;

public:
  InFileStream(const std::string &path);
  ~InFileStream();

  void readData(void *data, SizeT len) override;
  PosT tell() const override;
  void seek(OffT, Whence = Set) override;
};

class OutFileStream : public OutStream, public SeekableStream {
protected:
  mutable std::ofstream strm;

public:
  OutFileStream(const std::string &path);
  ~OutFileStream();

  void writeData(const void *data, SizeT len) override;
  PosT tell() const override;
  void seek(OffT, Whence = Set) override;
};

}
}

#endif /* ARN_IO_FILE_STREAM_HPP */
