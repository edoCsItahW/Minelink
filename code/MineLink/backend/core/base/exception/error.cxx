/**
 * @file error.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/14 22:02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module base:error;

import std;

export namespace minelink {

    enum class WireError : std::uint8_t { Truncated, VarIntTooLong, VarIntOverflow };

}
