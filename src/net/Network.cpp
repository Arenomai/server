#include "Network.hpp"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <memory>

#include <goodform/msgpack.hpp>

namespace arn {
namespace net {

Message::Message(MessageType t, uint8 s) :
    MemoryStream(nullptr, 0),
    m_type(t),
    m_subtype(s) {
}


InMessage::InMessage() :
    Message(MessageType::Null, 0) {
}

InMessage::~InMessage() {
    free();
}

void InMessage::setType(MessageType type) {
    free();
    m_type = type;
    m_subtype = m_length = m_cursor = 0;
    m_data = nullptr;
}

void InMessage::fromData(const void *data, SizeT len) {
    if (len < HeaderSize) {
        throw std::invalid_argument("Message length is smaller than message header");
    }
    const Header *const hdr = static_cast<const Header*>(data);
    free();
    m_cursor = 0;
    m_length = len - HeaderSize;
    m_type = hdr->type;
    m_subtype = hdr->subtype;
    // m_data/bytes is guaranteed never to be written to, so we can const_cast it
    m_data = reinterpret_cast<uint8*>(const_cast<void*>(data)) + HeaderSize;
}

void InMessage::free() {
    if (m_data != nullptr) {
        delete[] (m_data - HeaderSize);
    }
    m_type = MessageType::Null;
    m_subtype = m_length = m_cursor = 0;
    m_data = nullptr;
}

void InMessage::readMsgpack(goodform::variant &var) {
    uint32 len = readU32();
    if (len > remaining()) {
        throw std::runtime_error("Not enough bytes available for reported msgpack length");
    }
    InMemoryStream ims(getCursorPtr(len), len);
    goodform::msgpack::deserialize(ims, var);
}


OutMessage::OutMessage(MessageType t, uint8 subtype) :
    Message(t, subtype),
    m_actualData(nullptr) {
}

OutMessage::~OutMessage() {
    std::free(m_actualData);
    m_data = nullptr;
}

const static int OutMessage_AllocStep = 1024;
void OutMessage::fit(SizeT len) {
    if (len <= m_allocated)
        return;
    SizeT targetSize = ((len + OutMessage_AllocStep - 1) /
        OutMessage_AllocStep)*OutMessage_AllocStep; // Round up
    using DataT = decltype(m_actualData);
    DataT newActualData = static_cast<DataT>(
        std::realloc(m_actualData, HeaderSize + targetSize));
    if (newActualData == nullptr)
        throw std::bad_alloc();
    m_actualData = newActualData;
    m_data = newActualData + HeaderSize;
    m_allocated = targetSize;
}

void OutMessage::writeMsgpack(const goodform::variant &var) {
    PosT pos = tell();
    writeU32(0);
    goodform::msgpack::serialize(var, *this);
    PosT posWritten = tell();
    seek(pos);
    writeU32(static_cast<uint32>(posWritten - (pos + sizeof(uint32))));
    seek(posWritten);
}


}
}
