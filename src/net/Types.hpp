#ifndef ARN_NET_TYPES_HPP
#define ARN_NET_TYPES_HPP

#include <stdexcept>

#include "../platform/Types.hpp"

namespace arn {
namespace net {

class Exception : public std::exception {
};

using Port = uint16;

}
}

#endif /* ARN_NET_TYPES_HPP */

