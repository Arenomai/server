#ifndef ARN_NET_MSGTYPES_MSGTYPE_HPP
#define ARN_NET_MSGTYPES_MSGTYPE_HPP

#include "../Network.hpp"

namespace arn {
namespace net {
namespace MsgTypes {

struct MsgType {
  virtual ~MsgType() = default;

  virtual void writeToMsg(OutMessage&) const = 0;
  virtual void readFromMsg(InMessage&) = 0;
};

}
}
}

#endif /* ARN_NET_MSGTYPES_MSGTYPE_HPP */
