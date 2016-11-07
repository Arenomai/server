#ifndef ARN_NET_NETWORK_HPP
#define ARN_NET_NETWORK_HPP

#include <exception>
#include <type_traits>

#include <goodform/variant.hpp>

#include "../io/MemoryStream.hpp"

namespace arn {
namespace net {

enum class MessageType : uint8 {
  Null = 0,

  ServerInfo = 220,

  ConnectionParam = 200,

  PlayerJoin = 1,
  PlayerUpdate,
  PlayerQuit,
  ChunkTransfer,
  BlockUpdate,
  Chat,

  NetConnect = 240,
  NetDisconnect
};

class Message : public virtual io::MemoryStream {
protected:
  friend class Host;
  MessageType m_type;
  uint8 m_subtype;

  Message() {}
  Message(MessageType, uint8);
  Message(const Message&) = delete;
  Message& operator=(Message&) = delete;
  Message& operator=(const Message&) = delete;

public:
  static constexpr uint HeaderSize = 2;

  virtual ~Message() {}

  inline MessageType getType() const { return m_type; }
  inline uint8 getSubtype() const { return m_subtype; }
  template<typename T,
           typename = std::enable_if_t<std::is_integral<std::underlying_type_t<T>>::value>>
  inline T getSubtype() const { return static_cast<T>(m_subtype); }
};

class InMessage : public Message, public io::InMemoryStream {
protected:
  friend class Host;
  void setType(MessageType type);
  void fromData(const void *data, SizeT);
  void free();

public:
  InMessage();
  ~InMessage();

  inline const void* getCursorPtr(uint advanceCursor = 0) {
    m_cursor += advanceCursor;
    return &(m_data[m_cursor-advanceCursor]);
  }
  inline const void* getCursorPtr() const {
    return &(m_data[m_cursor]);
  }

  void readMsgpack(goodform::variant&);
};

class OutMessage : public Message, public io::OutMemoryStream {
protected:
  friend class Host;
  mutable uint8 *m_actualData;
  void fit(SizeT) override;

public:
  OutMessage(MessageType t = MessageType::Null, uint8 subtype = 255);
  ~OutMessage();

  inline void setType(MessageType t) { m_type = t; }
  template<typename T>
  inline void setSubtype(T t) { m_subtype = static_cast<uint8>(t); }
  template<typename T>
  inline void setType(MessageType t, T st) { setType(t); setSubtype(st); }

  void writeMsgpack(const goodform::variant&);
};

class Exception : public std::exception {
};

using Port = uint16;

}
}

#endif /* ARN_NET_NETWORK_HPP */
