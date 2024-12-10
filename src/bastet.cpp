#include <iostream>
#include <chess.hpp>
#include <climits>
#include <numeric>

class SearchData
{
  private:
    uint64_t nodes = 0;
    int timeElapsed = 0;
    int score = 0;
    int depth = 0;
    std::string bestmove = "";
    int allottedTime = 0;

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
    AddDepth ()
    {
        depth++;
    }
    void
    SubtractDepth ()
    {
        depth--;
    }
    void
    SetBestmove (const chess::consts::move &_bestmove)
    {
        bestmove = chess::moves::move2string (_bestmove);
    }
    void
    SetAlottedTime (const int &_alottedTime)
    {
        allottedTime = _alottedTime;
    }
    int
    GetAlottedTime ()
    {
        return allottedTime;
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
Quiescence (chess::engine::Engine &engine, int alpha, int beta, SearchData &searchData)
{
    int standPat = Evaluate (engine);
    if (standPat >= beta)
        {
            searchData.AddNodes ();
            return standPat;
        }
    if (alpha < standPat)
        {
            alpha = standPat;
        }
    std::vector<chess::consts::move> legalCaptures = engine.GetLegalCaptures ();
    int evaluation = alpha;
    for (const chess::consts::move &move : legalCaptures)
        {
            engine.MakeMove (move);
            evaluation = std::max (evaluation, -Quiescence (engine, -beta, -alpha, searchData));
            engine.UndoMove ();
            if (engine.GetTimer ().GetTimeElapsed () > searchData.GetAlottedTime ())
                {
                    /*return searchData.Bestmove ();*/
                    break;
                }
            alpha = std::max (alpha, evaluation);
            if (alpha >= beta)
                {
                    break;
                }
        }
    return evaluation;
}

int
NegaMax (chess::engine::Engine &engine, int depth, int alpha, int beta, SearchData &searchData)
{
    if (depth == 0)
        {
            /*searchData.AddNodes ();*/
            /*return Evaluate (engine);*/
            return Quiescence (engine, alpha, beta, searchData);
        }
    std::vector<chess::consts::move> legalMoves = engine.GetLegalMoves ();
    if (legalMoves.size () == 0)
        {
            if (engine.IsStalemate ())
                {
                    return 0;
                }
            if (engine.IsMate ())
                {
                    return -INT_MAX;
                }
        }
    if (engine.IsRepetition ())
        {
            return 0;
        }
    int evaluation = -INT_MAX;
    for (const chess::consts::move &move : legalMoves)
        {
            engine.MakeMove (move);
            evaluation = std::max (evaluation, -NegaMax (engine, depth - 1, -beta, -alpha, searchData));
            engine.UndoMove ();
            if (engine.GetTimer ().GetTimeElapsed () > searchData.GetAlottedTime ())
                {
                    /*return searchData.Bestmove ();*/
                    break;
                }
            alpha = std::max (alpha, evaluation);
            if (alpha >= beta)
                {
                    break;
                }
        }
    return evaluation;
}

struct child
{
    chess::consts::move move;
    int score;
};

std::string
Search (chess::engine::Engine &engine)
{
    SearchData searchData;
    int maxDepth = 4;
    int timeAlotment = 50;

    std::vector<chess::consts::move> legalMoves = engine.GetLegalMoves ();
    std::vector<child> children;
    for (const chess::consts::move &move : legalMoves)
        {
            child child;
            child.move = move;
            child.score = 0;
            children.push_back (child);
        }
    searchData.SetBestmove (children[0].move);
    /*for (int localDepth = 1; localDepth <= maxDepth; localDepth++)*/
    int localDepth = 1;
    int allottedTime = 0;
    if (engine.GetBoard ().white_to_play ())
        {
            allottedTime = engine.GetTimer ().GetWtime ();
        }
    else
        {
            allottedTime = engine.GetTimer ().GetBtime ();
        }
    allottedTime /= timeAlotment;
    searchData.SetAlottedTime (allottedTime);
    while (localDepth <= maxDepth)
        {
            searchData.SetDepth (localDepth);
            for (child &child : children)
                {
                    engine.MakeMove (child.move);
                    child.score = -NegaMax (engine, localDepth, -INT_MAX, INT_MAX, searchData);
                    engine.UndoMove ();
                    if (engine.GetTimer ().GetTimeElapsed () > searchData.GetAlottedTime ())
                        {
                            return searchData.Bestmove ();
                            /*break;*/
                        }
                }
            std::sort (children.begin (), children.end (), [] (child A, child B) { return A.score > B.score; });

            searchData.SetBestmove (children[0].move);
            searchData.SetTimeElapsed (engine.GetTimer ().GetTimeElapsed ());
            searchData.SetScore (children[0].score);
            searchData.Print ();
            localDepth++;
        }

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
