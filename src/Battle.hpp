#ifndef ARN_BATTLE_HPP
#define ARN_BATTLE_HPP

#include <vector>

#include <arn/polyfill/optional.hpp>

#include "platform/Types.hpp"
#include "Card.hpp"

class Battle {
public:
    struct BoardBox {
        enum ContentType {
          Empty,
          Card
        } contentType;
        std::experimental::optional<::Card> card;
    };

protected:
    uint m_boardWidth, m_boardHeight;
    std::vector<BoardBox> m_boxes;

public:
    uint boardWidth() {
        return m_boardWidth;
    }

    uint boardHeight() {
        return m_boardHeight;
    }
};

#endif // ARN_BATTLE_HPP
