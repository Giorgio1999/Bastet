#include <iostream>
#include <chess.hpp>
#include <climits>

class SearchData
{
  private:
    uint64_t nodes = 0;
    int timeElapsed = 0;
    int score = 0;
    int depth = 0;
    std::string bestmove = "";

  public:
    void
    SetNodes (const uint64_t &_nodes)
    {
        nodes = _nodes;
    }
    void
    AddNodes ()
    {
        nodes++;
    }
    void
    SetTimeElapsed (const int &_timeElapsed)
    {
        timeElapsed = _timeElapsed;
    }
    void
    SetScore (const int &_score)
    {
        score = _score;
    }
    void
    SetDepth (const int &_depth)
    {
        depth = _depth;
    }
    void
    SetBestmove (const chess::consts::move &_bestmove)
    {
        bestmove = chess::moves::move2string (_bestmove);
    }
    void
    Print ()
    {
        std::cout << "info depth " << depth << " score cp " << score << " nodes " << nodes << " nps " << nodes / (timeElapsed == 0 ? 1 : timeElapsed) * 1000 << std::endl;
    }
    std::string
    Bestmove ()
    {
        return "bestmove " + bestmove;
    }
};

int
Evaluate (chess::engine::Engine &engine)
{
    chess::board::Board board = engine.GetBoard ();
    bool whiteToPlay = board.white_to_play ();
    std::array<chess::consts::bitboard, 12> pieceBoards = board.get_piece_boards ();
    int scoreMultiplier = whiteToPlay ? 1 : -1;
    int evaluation = 0;
    std::array<int, 6> material = { 100, 300, 300, 500, 900 };
    for (uint i = 0; i < 6; i++)
        {
            evaluation += scoreMultiplier * material[i] * chess::bitboard_helper::count (pieceBoards[i]);
        }
    for (uint i = 0; i < 6; i++)
        {
            evaluation -= scoreMultiplier * material[i] * chess::bitboard_helper::count (pieceBoards[i + 6]);
        }
    return evaluation;
}

int
NegaMax (chess::engine::Engine &engine, int depth, int alpha, int beta, SearchData &searchData)
{
    if (depth == 0)
        {
            searchData.AddNodes ();
            return Evaluate (engine);
        }
    std::vector<chess::consts::move> legalMoves = engine.GetLegalMoves ();
    bool whiteToPlay = engine.GetBoard ().white_to_play ();
    if (legalMoves.size () == 0 && engine.IsStalemate ())
        {
            return 0;
        }
    int evaluation = -INT_MAX;
    for (const chess::consts::move &move : legalMoves)
        {
            engine.MakeMove (move);
            evaluation = std::max (evaluation, -NegaMax (engine, depth - 1, -beta, -alpha, searchData));
            engine.UndoMove ();
            alpha = std::max (alpha, evaluation);
            if (alpha >= beta)
                {
                    break;
                }
        }
    return evaluation;
}

std::string
Search (chess::engine::Engine &engine)
{
    SearchData searchData;
    int maxDepth = 4;

    std::vector<chess::consts::move> legalMoves = engine.GetLegalMoves ();
    std::vector<int> scores;
    for (const chess::consts::move &move : legalMoves)
        {
            engine.MakeMove (move);
            scores.push_back (-NegaMax (engine, maxDepth, -INT_MAX, INT_MAX, searchData));
            engine.UndoMove ();
        }
    auto bestMoveIndex = std::distance (scores.begin (), std::max_element (scores.begin (), scores.end ()));

    searchData.SetDepth (maxDepth);
    searchData.SetBestmove (legalMoves[bestMoveIndex]);
    searchData.SetTimeElapsed (engine.GetTimer ().GetTimeElapsed ());
    searchData.SetScore (scores[bestMoveIndex]);
    searchData.Print ();

    return searchData.Bestmove ();
}

int
main ()
{
    chess::engine::Engine engine ("Bastet", "Giorgio");
    engine.SetVersion_Major (1);
    engine.SetVersion_Minor (0);
    engine.SetVersion_Patch (0);
    engine.SetSearch (Search);
    chess::engine::Handler handler (engine, std::cin, std::cout);
}
