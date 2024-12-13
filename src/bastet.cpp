#include <iostream>
#include <chess.hpp>
#include <climits>
#include <cmath>
#include <numeric>

extern std::array<std::array<int, 64>, 10> psts;
extern std::array<std::array<int, 64>, 4> king_psts;

class SearchData
{
  private:
    uint64_t nodes = 0;
    int timeElapsed = 0;
    int score = 0;
    int depth = 0;
    std::string bestmove = "";
    int allottedTime = 0;
    std::chrono::time_point<std::chrono::steady_clock> start;

  public:
    void
    StartClock ()
    {
        start = std::chrono::steady_clock::now ();
    }
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
    int
    GetTimeElapsed ()
    {
        auto now = std::chrono::steady_clock::now ();
        return std::chrono::duration_cast<std::chrono::milliseconds> (now - start).count ();
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
        /*std::cout << "info depth " << depth << " score cp " << score << " nodes " << nodes << " nps " << nodes / (timeElapsed == 0 ? 1 : timeElapsed) * 1000 << std::endl;*/
        std::cout << "info depth " << depth;
        std::cout << " score cp " << score;
        std::cout << " nodes " << nodes;
        std::cout << " nps " << nodes / (GetTimeElapsed () == 0 ? 1 : GetTimeElapsed ()) * 1000 << std::endl;
    }
    std::string
    Bestmove ()
    {
        return "bestmove " + bestmove;
    }
};

const float SQRT2 = std::sqrt (2);
const std::array<int, 5> material = { 100, 300, 300, 500, 900 };
const int endgameMaterialCutoff = 16 * material[0] + 2 * material[1] + 2 * material[2] + 2 * material[3] + 0 * material[4];
const int mobilityBonus = 10;
const int mopUpMaterialCutoff = 1 * material[3] - 10;
const int mopUpBonus = 10;
int
Evaluate (chess::engine::Engine &engine)
{
    chess::board::Board board = engine.GetBoard ();
    bool whiteToPlay = board.white_to_play ();
    std::array<chess::consts::bitboard, 12> pieceBoards = board.get_piece_boards ();
    int scoreMultiplier = whiteToPlay ? 1 : -1;
    int evaluation = 0;
    chess::consts::bitboard whiteAttacks = engine.GetAttacks (true);
    chess::consts::bitboard blackAttacks = engine.GetAttacks (false);
    int whiteMaterialRemaining = 0;
    int blackMaterialRemaining = 0;
    for (uint i = 0; i < 5; i++)
        {
            int material_count = material[i] * chess::bitboard_helper::count (pieceBoards[i]);
            evaluation += scoreMultiplier * material_count;
            whiteMaterialRemaining += material_count;
            while (pieceBoards[i] > 0)
                {
                    int square = chess::bitboard_helper::pop_lsb (pieceBoards[i]);
                    evaluation += scoreMultiplier * psts[i][square];
                }
        }
    for (uint i = 0; i < 5; i++)
        {
            int material_count = material[i] * chess::bitboard_helper::count (pieceBoards[i + 6]);
            evaluation -= scoreMultiplier * material_count;
            blackMaterialRemaining += material_count;
            while (pieceBoards[i + 6] > 0)
                {
                    int square = chess::bitboard_helper::pop_lsb (pieceBoards[i + 6]);
                    evaluation -= scoreMultiplier * psts[i + 5][square];
                }
        }
    int whiteKingSquare = chess::bitboard_helper::pop_lsb (pieceBoards[5]);
    int blackKingSquare = chess::bitboard_helper::pop_lsb (pieceBoards[11]);
    int totalMaterialRemaining = whiteMaterialRemaining + blackMaterialRemaining;
    int materialAdvantage = std::abs (whiteMaterialRemaining - blackMaterialRemaining);
    evaluation += scoreMultiplier * (totalMaterialRemaining < endgameMaterialCutoff ? king_psts[2][whiteKingSquare] : king_psts[0][whiteKingSquare]);
    evaluation -= scoreMultiplier * (totalMaterialRemaining < endgameMaterialCutoff ? king_psts[3][blackKingSquare] : king_psts[1][blackKingSquare]);
    if (materialAdvantage >= mopUpMaterialCutoff)
        {
            int wk_file = whiteKingSquare & 7;
            int wk_rank = whiteKingSquare >> 3;
            int bk_file = blackKingSquare & 7;
            int bk_rank = blackKingSquare >> 3;
            int distance = 0;
            if (whiteToPlay)
                {
                    int dfile = std::min (std::abs (bk_file - 0), std::abs (bk_file - 7));
                    int drank = std::min (std::abs (bk_rank - 0), std::abs (bk_rank - 7));
                    distance = SQRT2 * std::min (dfile, drank) + (std::max (dfile, drank) - std::min (dfile, drank));
                }
            else
                {
                    int dfile = std::min (std::abs (wk_file - 0), std::abs (wk_file - 7));
                    int drank = std::min (std::abs (wk_rank - 0), std::abs (wk_rank - 7));
                    distance = SQRT2 * std::min (dfile, drank) + (std::max (dfile, drank) - std::min (dfile, drank));
                }
            evaluation -= mopUpBonus * distance * (evaluation > 0 ? 1 : -1);
        }
    evaluation += scoreMultiplier * mobilityBonus * chess::bitboard_helper::count (whiteAttacks);
    evaluation -= scoreMultiplier * mobilityBonus * chess::bitboard_helper::count (blackAttacks);
    /*8/8/8/3K4/8/8/8/6rk b - - 0 1*/
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
            /*if (engine.GetTimer ().GetTimeElapsed () > searchData.GetAlottedTime ())*/
            if (searchData.GetTimeElapsed () > searchData.GetAlottedTime ())
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
            /*if (engine.GetTimer ().GetTimeElapsed () > searchData.GetAlottedTime ())*/
            if (searchData.GetTimeElapsed () > searchData.GetAlottedTime ())
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
    searchData.StartClock ();
    int maxDepth = 20;
    int timeAlotment = 30;

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
            allottedTime = engine.GetTimer ().GetWtime () + engine.GetTimer ().GetWinc ();
        }
    else
        {
            allottedTime = engine.GetTimer ().GetBtime () + engine.GetTimer ().GetBinc ();
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
                    /*if (engine.GetTimer ().GetTimeElapsed () > searchData.GetAlottedTime ())*/
                    if (searchData.GetTimeElapsed () > searchData.GetAlottedTime ())
                        {
                            return searchData.Bestmove ();
                            /*break;*/
                        }
                }
            std::sort (children.begin (), children.end (), [] (child A, child B) { return A.score > B.score; });

            searchData.SetBestmove (children[0].move);
            searchData.SetScore (children[0].score);
            searchData.Print ();
            localDepth++;
            if (children[0].score == INT_MAX)
                {
                    break;
                }
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

std::array<std::array<int, 64>, 10> psts = {
    // alls pstss for middle game pieces
    // https://www.chessprogramming.org/Simplified_Evaluation_Function
    std::array<int, 64>{
        // white pawn
        0,  0,  0,   0,   0,   0,   0,  0,  //
        50, 50, 50,  50,  50,  50,  50, 50, //
        10, 10, 20,  30,  30,  20,  10, 10, //
        5,  5,  10,  25,  25,  10,  5,  5,  //
        0,  0,  0,   20,  20,  0,   0,  0,  //
        5,  -5, -10, 0,   0,   -10, -5, 5,  //
        5,  10, 10,  -20, -20, 10,  10, 5,  //
        0,  0,  0,   0,   0,   0,   0,  0   //
    },
    std::array<int, 64>{
        // white knight
        -50, -40, -30, -30, -30, -30, -40, -50, //
        -40, -20, 0,   0,   0,   0,   -20, -40, //
        -30, 0,   10,  15,  15,  10,  0,   -30, //
        -30, 5,   15,  20,  20,  15,  5,   -30, //
        -30, 0,   15,  20,  20,  15,  0,   -30, //
        -30, 5,   10,  15,  15,  10,  5,   -30, //
        -40, -20, 0,   5,   5,   0,   -20, -40, //
        -50, -40, -30, -30, -30, -30, -40, -50, //
    },
    std::array<int, 64>{
        // white bishop
        -20, -10, -10, -10, -10, -10, -10, -20, //
        -10, 0,   0,   0,   0,   0,   0,   -10, //
        -10, 0,   5,   10,  10,  5,   0,   -10, //
        -10, 5,   5,   10,  10,  5,   5,   -10, //
        -10, 0,   10,  10,  10,  10,  0,   -10, //
        -10, 10,  10,  10,  10,  10,  10,  -10, //
        -10, 5,   0,   0,   0,   0,   5,   -10, //
        -20, -10, -10, -10, -10, -10, -10, -20, //
    },
    std::array<int, 64>{
        // white rook
        0,  0,  0,  0,  0,  0,  0,  0,  //
        5,  10, 10, 10, 10, 10, 10, 5,  //
        -5, 0,  0,  0,  0,  0,  0,  -5, //
        -5, 0,  0,  0,  0,  0,  0,  -5, //
        -5, 0,  0,  0,  0,  0,  0,  -5, //
        -5, 0,  0,  0,  0,  0,  0,  -5, //
        -5, 0,  0,  0,  0,  0,  0,  -5, //
        0,  0,  0,  5,  5,  0,  0,  0   //
    },
    std::array<int, 64>{
        // white Queen
        -20, -10, -10, -5, -5, -10, -10, -20, //
        -10, 0,   0,   0,  0,  0,   0,   -10, //
        -10, 0,   5,   5,  5,  5,   0,   -10, //
        -5,  0,   5,   5,  5,  5,   0,   -5,  //
        0,   0,   5,   5,  5,  5,   0,   -5,  //
        -10, 5,   5,   5,  5,  5,   0,   -10, //
        -10, 0,   5,   0,  0,  0,   0,   -10, //
        -20, -10, -10, -5, -5, -10, -10, -20  //
    },
    std::array<int, 64>{
        // black pawn

        0,  0,  0,   0,   0,   0,   0,  0,  //
        5,  10, 10,  -20, -20, 10,  10, 5,  //
        5,  -5, -10, 0,   0,   -10, -5, 5,  //
        0,  0,  0,   20,  20,  0,   0,  0,  //
        5,  5,  10,  25,  25,  10,  5,  5,  //
        10, 10, 20,  30,  30,  20,  10, 10, //
        50, 50, 50,  50,  50,  50,  50, 50, //
        0,  0,  0,   0,   0,   0,   0,  0   //
    },
    std::array<int, 64>{
        // black knight
        -50, -40, -30, -30, -30, -30, -40, -50, //
        -40, -20, 0,   5,   5,   0,   -20, -40, //
        -30, 5,   10,  15,  15,  10,  5,   -30, //
        -30, 0,   15,  20,  20,  15,  0,   -30, //
        -30, 5,   15,  20,  20,  15,  5,   -30, //
        -30, 0,   10,  15,  15,  10,  0,   -30, //
        -40, -20, 0,   0,   0,   0,   -20, -40, //
        -50, -40, -30, -30, -30, -30, -40, -50, //

    },
    std::array<int, 64>{
        // black bishop
        -20, -10, -10, -10, -10, -10, -10, -20, //
        -10, 5,   0,   0,   0,   0,   5,   -10, //
        -10, 10,  10,  10,  10,  10,  10,  -10, //
        -10, 0,   10,  10,  10,  10,  0,   -10, //
        -10, 5,   5,   10,  10,  5,   5,   -10, //
        -10, 0,   5,   10,  10,  5,   0,   -10, //
        -10, 0,   0,   0,   0,   0,   0,   -10, //
        -20, -10, -10, -10, -10, -10, -10, -20, //

    },
    std::array<int, 64>{
        // black rook
        0,  0,  0,  5,  5,  0,  0,  0,  //
        -5, 0,  0,  0,  0,  0,  0,  -5, //
        -5, 0,  0,  0,  0,  0,  0,  -5, //
        -5, 0,  0,  0,  0,  0,  0,  -5, //
        -5, 0,  0,  0,  0,  0,  0,  -5, //
        -5, 0,  0,  0,  0,  0,  0,  -5, //
        5,  10, 10, 10, 10, 10, 10, 5,  //
        0,  0,  0,  0,  0,  0,  0,  0,  //

    },
    std::array<int, 64>{
        // black Queen
        -20, -10, -10, -5, -5, -10, -10, -20, //
        -10, 0,   5,   0,  0,  0,   0,   -10, //
        -10, 5,   5,   5,  5,  5,   0,   -10, //
        0,   0,   5,   5,  5,  5,   0,   -5,  //
        -5,  0,   5,   5,  5,  5,   0,   -5,  //
        -10, 0,   5,   5,  5,  5,   0,   -10, //
        -10, 0,   0,   0,  0,  0,   0,   -10, //
        -20, -10, -10, -5, -5, -10, -10, -20  //
    },
};

std::array<std::array<int, 64>, 4> king_psts = {
    // all kings pstss white-mg,black-mg,white-eg,black-eg
    // https://www.chessprogramming.org/Simplified_Evaluation_Function
    std::array<int, 64>{
        // white-mg
        -30, -40, -40, -50, -50, -40, -40, -30, //
        -30, -40, -40, -50, -50, -40, -40, -30, //
        -30, -40, -40, -50, -50, -40, -40, -30, //
        -30, -40, -40, -50, -50, -40, -40, -30, //
        -20, -30, -30, -40, -40, -30, -30, -20, //
        -10, -20, -20, -20, -20, -20, -20, -10, //
        20,  20,  0,   0,   0,   0,   20,  20,  //
        20,  30,  10,  0,   0,   10,  30,  20   //
    },
    std::array<int, 64>{
        // black-mg
        20,  30,  10,  0,   0,   10,  30,  20,  //
        20,  20,  0,   0,   0,   0,   20,  20,  //
        -10, -20, -20, -20, -20, -20, -20, -10, //
        -20, -30, -30, -40, -40, -30, -30, -20, //
        -30, -40, -40, -50, -50, -40, -40, -30, //
        -30, -40, -40, -50, -50, -40, -40, -30, //
        -30, -40, -40, -50, -50, -40, -40, -30, //
        -30, -40, -40, -50, -50, -40, -40, -30, //
    },
    std::array<int, 64>{
        // white-eg
        -50, -40, -30, -20, -20, -30, -40, -50, //
        -30, -20, -10, 0,   0,   -10, -20, -30, //
        -30, -10, 20,  30,  30,  20,  -10, -30, //
        -30, -10, 30,  40,  40,  30,  -10, -30, //
        -30, -10, 30,  40,  40,  30,  -10, -30, //
        -30, -10, 20,  30,  30,  20,  -10, -30, //
        -30, -30, 0,   0,   0,   0,   -30, -30, //
        -50, -30, -30, -30, -30, -30, -30, -50  //
    },
    std::array<int, 64>{
        // black-eg
        -50, -30, -30, -30, -30, -30, -30, -50, //
        -30, -30, 0,   0,   0,   0,   -30, -30, //
        -30, -10, 20,  30,  30,  20,  -10, -30, //
        -30, -10, 30,  40,  40,  30,  -10, -30, //
        -30, -10, 30,  40,  40,  30,  -10, -30, //
        -30, -10, 20,  30,  30,  20,  -10, -30, //
        -30, -20, -10, 0,   0,   -10, -20, -30, //
        -50, -40, -30, -20, -20, -30, -40, -50, //
    },
};
