#pragma once

#include "Move.h"

// For formatting Board and BitBoard in output streams

namespace BoardFormat {
    struct BoardFormat {
        Colour Orientation = White;
        bool Coordinates = true;
    };

    inline BoardFormat s_BoardFormat;

    template <typename CharT, typename Traits>
    std::basic_ostream<CharT, Traits>& BoardCoordinates(std::basic_ostream<CharT, Traits>& os) {
        s_BoardFormat.Coordinates = true;
        return os;
    }

    template <typename CharT, typename Traits>
    std::basic_ostream<CharT, Traits>& NoBoardCoordinates(std::basic_ostream<CharT, Traits>& os) {
        s_BoardFormat.Coordinates = false;
        return os;
    }

    template <typename CharT, typename Traits>
    std::basic_ostream<CharT, Traits>& OrientationWhite(std::basic_ostream<CharT, Traits>& os) {
        s_BoardFormat.Orientation = White;
        return os;
    }

    template <typename CharT, typename Traits>
    std::basic_ostream<CharT, Traits>& OrientationBlack(std::basic_ostream<CharT, Traits>& os) {
        s_BoardFormat.Orientation = Black;
        return os;
    }
}
