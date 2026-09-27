#include "main.h"

void InitMoves(Position *pos, MOVES *m, int trans_move, int ply)
{
  m->pos = pos;
  m->phase = 0;
  m->trans_move = trans_move;
  m->killer1 = killer[ply][0];
  m->killer2 = killer[ply][1];
}

int NextMove(MOVES *m)
{
  int moves;

  switch (m->phase) {
  case 0:
    moves = m->trans_move;
    if (moves && Legal(m->pos, moves)) {
      m->phase = 1;
      return moves;
    }
  case 1:
    m->last = GenerateCaptures(m->pos, m->moves);
    ScoreCaptures(m);
    m->next = m->moves;
    m->badp = m->bad;
    m->phase = 2;
  case 2:
    while (m->next < m->last) {
      moves = SelectBest(m);
      if (moves == m->trans_move)
        continue;
      if (BadCapture(m->pos, moves)) {
        *m->badp++ = moves;
        continue;
      }
      return moves;
    }
  case 3:
    moves = m->killer1;
    if (moves && moves != m->trans_move &&
        m->pos->pc[Tsq(moves)] == PIECE_NB && Legal(m->pos, moves)) {
      m->phase = 4;
      return moves;
    }
  case 4:
    moves = m->killer2;
    if (moves && moves != m->trans_move &&
        m->pos->pc[Tsq(moves)] == PIECE_NB && Legal(m->pos, moves)) {
      m->phase = 5;
      return moves;
    }
  case 5:
    m->last = GenerateQuiet(m->pos, m->moves);
    ScoreQuiet(m);
    m->next = m->moves;
    m->phase = 6;
  case 6:
    while (m->next < m->last) {
      moves = SelectBest(m);
      if (moves == m->trans_move ||
          moves == m->killer1 ||
          moves == m->killer2)
        continue;
      return moves;
    }
    m->next = m->bad;
    m->phase = 7;
  case 7:
    if (m->next < m->badp)
      return *m->next++;
  }
  return 0;
}

void InitCaptures(Position *pos, MOVES *m)
{
  m->pos = pos;
  m->last = GenerateCaptures(m->pos, m->moves);
  ScoreCaptures(m);
  m->next = m->moves;
}

int NextCapture(MOVES *m)
{
  int moves;

  while (m->next < m->last) {
    moves = SelectBest(m);
    if (BadCapture(m->pos, moves))
      continue;
    return moves;
  }
  return 0;
}

void ScoreCaptures(MOVES *m)
{
  int *movep, *valuep;

  valuep = m->value;
  for (movep = m->moves; movep < m->last; movep++)
    *valuep++ = MvvLva(m->pos, *movep);
}

void ScoreQuiet(MOVES *m)
{
  int *movep, *valuep;

  valuep = m->value;
  for (movep = m->moves; movep < m->last; movep++)
    *valuep++ = history[m->pos->pc[Fsq(*movep)]][Tsq(*movep)];
}

int SelectBest(MOVES *m)
{
  int *movep, *valuep, aux;

  valuep = m->value + (m->last - m->moves) - 1;
  for (movep = m->last - 1; movep > m->next; movep--) {
    if (*valuep > *(valuep - 1)) {
      aux = *valuep;
      *valuep = *(valuep - 1);
      *(valuep - 1) = aux;
      aux = *movep;
      *movep = *(movep - 1);
      *(movep - 1) = aux;
    }
    valuep--;
  }
  return *m->next++;
}

int BadCapture(Position *pos, int moves)
{
  int fsq, tsq;

  fsq = Fsq(moves);
  tsq = Tsq(moves);
  if (tp_value[TpOnSq(pos, tsq)] >= tp_value[TpOnSq(pos, fsq)])
    return 0;
  if (MoveType(moves) == EP_CAP)
    return 0;
  return Swap(pos, fsq, tsq) < 0;
}

int MvvLva(Position *pos, int moves)
{
  if (pos->pc[Tsq(moves)] != PIECE_NB)
    return TpOnSq(pos, Tsq(moves)) * 6 + 5 - TpOnSq(pos, Fsq(moves));
  if (IsProm(moves))
    return PromType(moves) - 5;
  return 5;
}

void ClearHist(void){
  int i, j;
  for (i = 0; i < 12; i++)
    for (j = 0; j < 64; j++)
      history[i][j] = 0;
  for (i = 0; i < MAX_PLY; i++) {
    killer[i][0] = 0;
    killer[i][1] = 0;
  }
}

void Hist(Position *pos, int moves, int depth, int ply)
{
  if (pos->pc[Tsq(moves)] != PIECE_NB || IsProm(moves) || MoveType(moves) == EP_CAP)
    return;
  history[pos->pc[Fsq(moves)]][Tsq(moves)] += depth;
  if (moves != killer[ply][0]) {
    killer[ply][1] = killer[ply][0];
    killer[ply][0] = moves;
  }
}
