#include "main.h"

void UndoMove(Position *pos, int moves, UNDO *u)
{
  int side, fsq, tsq, ftp, ttp;

  side = Opp(pos->side);
  fsq = Fsq(moves);
  tsq = Tsq(moves);
  ftp = TpOnSq(pos, tsq);
  ttp = u->ttp;
  pos->c_flags = u->c_flags;
  pos->ep_sq = u->ep_sq;
  pos->rev_moves = u->rev_moves;
  pos->key = u->key;
  pos->head--;
  pos->pc[fsq] = Pc(side, ftp);
  pos->pc[tsq] = PIECE_NB;
  pos->cl_bb[side] ^= SqBb(fsq) | SqBb(tsq);
  pos->tp_bb[ftp] ^= SqBb(fsq) | SqBb(tsq);
  if (ftp == K)
    pos->king_sq[side] = fsq;
  if (ttp != PT_NB) {
    pos->pc[tsq] = Pc(Opp(side), ttp);
    pos->cl_bb[Opp(side)] ^= SqBb(tsq);
    pos->tp_bb[ttp] ^= SqBb(tsq);
    pos->mat[Opp(side)] += tp_value[ttp];
  }
  switch (MoveType(moves)) {
  case NORMAL:
    break;
  case CASTLE:
    if (tsq > fsq) {
      fsq += 3;
      tsq -= 1;
    } else {
      fsq -= 4;
      tsq += 1;
    }
    pos->pc[tsq] = PIECE_NB;
    pos->pc[fsq] = Pc(side, R);
    pos->cl_bb[side] ^= SqBb(fsq) | SqBb(tsq);
    pos->tp_bb[R] ^= SqBb(fsq) | SqBb(tsq);
    break;
  case EP_CAP:
    tsq ^= 8;
    pos->pc[tsq] = Pc(Opp(side), P);
    pos->cl_bb[Opp(side)] ^= SqBb(tsq);
    pos->tp_bb[P] ^= SqBb(tsq);
    pos->mat[Opp(side)] += tp_value[P];
    break;
  case EP_SET:
    break;
  case N_PROM: case B_PROM: case R_PROM: case Q_PROM:
    pos->pc[fsq] = Pc(side, P);
    pos->tp_bb[P] ^= SqBb(fsq);
    pos->tp_bb[ftp] ^= SqBb(fsq);
    pos->mat[side] += tp_value[P] - tp_value[ftp];
    break;
  }
  pos->side ^= 1;
}

void UndoNull(Position *pos, UNDO *u)
{
  pos->ep_sq = u->ep_sq;
  pos->key = u->key;
  pos->head--;
  pos->rev_moves--;
  pos->side ^= 1;
}
