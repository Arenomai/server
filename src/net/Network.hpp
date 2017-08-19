#ifndef ARN_NET_NETWORK_HPP
#define ARN_NET_NETWORK_HPP

#include <exception>
#include <limits>
#include <type_traits>

#include <goodform/variant.hpp>

#include "../io/MemoryStream.hpp"
#include "Types.hpp"

namespace arn {
namespace net {

    enum class MessageType : uint8 {
        Null = 0,
        Auth = 1,
        Inventory = 2,
        Event = 3,
        UserAccount = 4,
        //ItemDef = 5, will be added later
        PosUpdate =6,
    };

    enum class AuthSubType : uint8 {
        Denied = 0,
        Request = 1,
        Response = 2,
    };

    enum class UserAccountSubType : uint8 {
        PassModify = 1,
        AccountModify = 2,
        InfoRequest = 3,
        InfoResponse = 4,
    };

    enum class EventSubType : uint8 {
        Battle = 0,
        Meeting = 1,
    };

    enum class Inventory : uint8 {
        Request = 0,
        Response = 1
    };

    enum class PosUpdateSubType : uint8
    {
        EventGet = 0,
        EventGetResponse = 1,
    };

class Message : public virtual io::MemoryStream {
protected:
    friend class TCPConnection;
    MessageType m_type;
    uint8 m_subtype;

    Message() {}
    Message(MessageType, uint8);
    Message(const Message&) = delete;
    Message& operator=(Message&) = delete;
    Message& operator=(const Message&) = delete;

public:
    struct [[gnu::packed]] Header {
      uint16 data_length;
      MessageType type;
      uint8 subtype;
    };
    static constexpr uint HeaderSize = sizeof(Header);
    static constexpr uint MaxTotalSize = std::numeric_limits<uint16>::max();
    static constexpr uint MaxDataSize = MaxTotalSize - HeaderSize;

    virtual ~Message() {}

    inline MessageType getType() const { return m_type; }
    inline uint8 getSubtype() const { return m_subtype; }
    template<typename T,
             typename = std::enable_if_t<std::is_integral<std::underlying_type_t<T>>::value>>
    inline T getSubtype() const { return static_cast<T>(m_subtype); }
};

class InMessage : public Message, public io::InMemoryStream {
protected:
    friend class TCPConnection;
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

    std::string dump() const;
};

class OutMessage : public Message, public io::OutMemoryStream {
protected:
    friend class TCPConnection;
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

}
}

#endif /* ARN_NET_NETWORK_HPP */
