#include "main.h"

void SetPosition(Position *pos, char *epd)
{
  int i, j, pc;
  static const char pc_char[12] = "PpNnBbRrQqKk";

  for (i = 0; i < 2; i++) {
    pos->cl_bb[i] = 0;
    pos->mat[i] = 0;
  }
  for (i = 0; i < 6; i++)
    pos->tp_bb[i] = 0;
  pos->c_flags = 0;
  pos->rev_moves = 0;
  pos->head = 0;
  for (i = 56; i >= 0; i -= 8) {
    j = 0;
    while (j < 8) {
      if (*epd >= '1' && *epd <= '8')
        for (pc = 0; pc < *epd - '0'; pc++) {
          pos->pc[i + j] = PIECE_NB;
          j++;
        }
      else {
        for (pc = 0; pc_char[pc] != *epd; pc++)
          ;
        pos->pc[i + j] = pc;
        pos->cl_bb[Cl(pc)] ^= SqBb(i + j);
        pos->tp_bb[Tp(pc)] ^= SqBb(i + j);
        if (Tp(pc) == K)
          pos->king_sq[Cl(pc)] = i + j;
        pos->mat[Cl(pc)] += tp_value[Tp(pc)];
        j++;
      }
      epd++;
    }
    epd++;
  }
  if (*epd++ == 'w')
    pos->side = WHITE;
  else
    pos->side = BLACK;
  epd++;
  if (*epd == '-')
    epd++;
  else {
    if (*epd == 'K') {
      pos->c_flags |= 1;
      epd++;
    }
    if (*epd == 'Q') {
      pos->c_flags |= 2;
      epd++;
    }
    if (*epd == 'k') {
      pos->c_flags |= 4;
      epd++;
    }
    if (*epd == 'q') {
      pos->c_flags |= 8;
      epd++;
    }
  }
  epd++;
  if (*epd == '-')
    pos->ep_sq = NO_SQ;
  else {
    pos->ep_sq = Sq(*epd - 'a', *(epd + 1) - '1');
    if (!(p_attacks[Opp(pos->side)][pos->ep_sq] & PcBb(pos, pos->side, P)))
      pos->ep_sq = NO_SQ;
  }
  pos->key = Key(pos);
}
