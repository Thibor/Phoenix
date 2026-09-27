#include "main.h"

int Legal(Position *pos, int moves)
{
  int side, fsq, tsq, ftp, ttp;

  side = pos->side;
  fsq = Fsq(moves);
  tsq = Tsq(moves);
  ftp = TpOnSq(pos, fsq);
  ttp = TpOnSq(pos, tsq);
  if (ftp == PT_NB || Cl(pos->pc[fsq]) != side)
    return 0;
  if (ttp != PT_NB && Cl(pos->pc[tsq]) == side)
    return 0;
  switch (MoveType(moves)) {
  case NORMAL:
    break;
  case CASTLE:
    if (side == WHITE) {
      if (fsq != E1)
        return 0;
      if (tsq > fsq) {
        if ((pos->c_flags & 1) && !(OccBb(pos) & (U64)0x0000000000000060))
          if (!Attacked(pos, E1, BLACK) && !Attacked(pos, F1, BLACK))
            return 1;
      } else {
        if ((pos->c_flags & 2) && !(OccBb(pos) & (U64)0x000000000000000E))
          if (!Attacked(pos, E1, BLACK) && !Attacked(pos, D1, BLACK))
            return 1;
      }
    } else {
      if (fsq != E8)
        return 0;
      if (tsq > fsq) {
        if ((pos->c_flags & 4) && !(OccBb(pos) & (U64)0x6000000000000000))
          if (!Attacked(pos, E8, WHITE) && !Attacked(pos, F8, WHITE))
            return 1;
      } else {
        if ((pos->c_flags & 8) && !(OccBb(pos) & (U64)0x0E00000000000000))
          if (!Attacked(pos, E8, WHITE) && !Attacked(pos, D8, WHITE))
            return 1;
      }
    }
    return 0;
  case EP_CAP:
    if (ftp == P && tsq == pos->ep_sq)
      return 1;
    return 0;
  case EP_SET:
    if (ftp == P && ttp == PT_NB && pos->pc[tsq ^ 8] == PIECE_NB)
      if ((tsq > fsq && side == WHITE) ||
          (tsq < fsq && side == BLACK))
        return 1;
    return 0;
  }
  if (ftp == P) {
    if (side == WHITE) {
      if (Rank(fsq) == RANK_7 && !IsProm(moves))
        return 0;
      if (tsq - fsq == 8)
        if (ttp == PT_NB)
          return 1;
      if ((tsq - fsq == 7 && File(fsq) != FILE_A) ||
          (tsq - fsq == 9 && File(fsq) != FILE_H))
        if (ttp != PT_NB)
          return 1;
    } else {
      if (Rank(fsq) == RANK_2 && !IsProm(moves))
        return 0;
      if (tsq - fsq == -8)
        if (ttp == PT_NB)
          return 1;
      if ((tsq - fsq == -9 && File(fsq) != FILE_A) ||
          (tsq - fsq == -7 && File(fsq) != FILE_H))
        if (ttp != PT_NB)
          return 1;
    }
    return 0;
  }
  if (IsProm(moves))
    return 0;
  return (AttacksFrom(pos, fsq) & SqBb(tsq)) != 0;
}
