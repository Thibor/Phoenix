#include "main.h"

U64 AttacksFrom(Position *pos, int sq)
{
  switch (TpOnSq(pos, sq)) {
  case P:
    return p_attacks[Cl(pos->pc[sq])][sq];
  case N:
    return n_attacks[sq];
  case B:
    return BAttacks(OccBb(pos), sq);
  case R:
    return RAttacks(OccBb(pos), sq);
  case Q:
    return QAttacks(OccBb(pos), sq);
  case K:
    return k_attacks[sq];
  }
  return 0;
}

U64 AttacksTo(Position *pos, int sq)
{
  return (PcBb(pos, WHITE, P) & p_attacks[BLACK][sq]) |
         (PcBb(pos, BLACK, P) & p_attacks[WHITE][sq]) |
         (pos->tp_bb[N] & n_attacks[sq]) |
         ((pos->tp_bb[B] | pos->tp_bb[Q]) & BAttacks(OccBb(pos), sq)) |
         ((pos->tp_bb[R] | pos->tp_bb[Q]) & RAttacks(OccBb(pos), sq)) |
         (pos->tp_bb[K] & k_attacks[sq]);
}

int Attacked(Position *pos, int sq, int side)
{
  return (PcBb(pos, side, P) & p_attacks[Opp(side)][sq]) ||
         (PcBb(pos, side, N) & n_attacks[sq]) ||
         ((PcBb(pos, side, B) | PcBb(pos, side, Q)) & BAttacks(OccBb(pos), sq)) ||
         ((PcBb(pos, side, R) | PcBb(pos, side, Q)) & RAttacks(OccBb(pos), sq)) ||
         (PcBb(pos, side, K) & k_attacks[sq]);
}
