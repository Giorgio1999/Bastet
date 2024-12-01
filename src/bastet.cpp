#include <iostream>
#include <chess.hpp>

int
main ()
{
    chess::engine::Engine engine ("test author", "test engine");
    chess::consts::bitboard b = 2134;
    std::cout << chess::bitboard_helper::visualize_bitboard (b) << std::endl;
    chess::engine::Handler handler (engine, std::cin, std::cout);
}
