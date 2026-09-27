#include "main.h"

void DoMove(Position* pos, int moves, UNDO* u)
{
	int side, fsq, tsq, ftp, ttp;

	side = pos->side;
	fsq = Fsq(moves);
	tsq = Tsq(moves);
	ftp = TpOnSq(pos, fsq);
	ttp = TpOnSq(pos, tsq);
	u->ttp = ttp;
	u->c_flags = pos->c_flags;
	u->ep_sq = pos->ep_sq;
	u->rev_moves = pos->rev_moves;
	u->key = pos->key;
	pos->rep_list[pos->head++] = pos->key;
	if (ftp == P || ttp != PT_NB)
		pos->rev_moves = 0;
	else
		pos->rev_moves++;
	pos->key ^= zob_castle[pos->c_flags];
	pos->c_flags &= c_mask[fsq] & c_mask[tsq];
	pos->key ^= zob_castle[pos->c_flags];
	if (pos->ep_sq != NO_SQ) {
		pos->key ^= zob_ep[File(pos->ep_sq)];
		pos->ep_sq = NO_SQ;
	}
	pos->pc[fsq] = PIECE_NB;
	pos->pc[tsq] = Pc(side, ftp);
	pos->key ^= zob_piece[Pc(side, ftp)][fsq] ^ zob_piece[Pc(side, ftp)][tsq];
	pos->cl_bb[side] ^= SqBb(fsq) | SqBb(tsq);
	pos->tp_bb[ftp] ^= SqBb(fsq) | SqBb(tsq);
	if (ftp == K)
		pos->king_sq[side] = tsq;
	if (ttp != PT_NB) {
		pos->key ^= zob_piece[Pc(Opp(side), ttp)][tsq];
		pos->cl_bb[Opp(side)] ^= SqBb(tsq);
		pos->tp_bb[ttp] ^= SqBb(tsq);
		pos->mat[Opp(side)] -= tp_value[ttp];
	}
	switch (MoveType(moves)) {
	case NORMAL:
		break;
	case CASTLE:
		if (tsq > fsq) {
			fsq += 3;
			tsq -= 1;
		}
		else {
			fsq -= 4;
			tsq += 1;
		}
		pos->pc[fsq] = PIECE_NB;
		pos->pc[tsq] = Pc(side, R);
		pos->key ^= zob_piece[Pc(side, R)][fsq] ^ zob_piece[Pc(side, R)][tsq];
		pos->cl_bb[side] ^= SqBb(fsq) | SqBb(tsq);
		pos->tp_bb[R] ^= SqBb(fsq) | SqBb(tsq);
		break;
	case EP_CAP:
		tsq ^= 8;
		pos->pc[tsq] = PIECE_NB;
		pos->key ^= zob_piece[Pc(Opp(side), P)][tsq];
		pos->cl_bb[Opp(side)] ^= SqBb(tsq);
		pos->tp_bb[P] ^= SqBb(tsq);
		pos->mat[Opp(side)] -= tp_value[P];
		break;
	case EP_SET:
		tsq ^= 8;
		if (p_attacks[side][tsq] & PcBb(pos, Opp(side), P)) {
			pos->ep_sq = tsq;
			pos->key ^= zob_ep[File(tsq)];
		}
		break;
	case N_PROM: case B_PROM: case R_PROM: case Q_PROM:
		ftp = PromType(moves);
		pos->pc[tsq] = Pc(side, ftp);
		pos->key ^= zob_piece[Pc(side, P)][tsq] ^ zob_piece[Pc(side, ftp)][tsq];
		pos->tp_bb[P] ^= SqBb(tsq);
		pos->tp_bb[ftp] ^= SqBb(tsq);
		pos->mat[side] += tp_value[ftp] - tp_value[P];
		break;
	}
	pos->side ^= 1;
	pos->key ^= SIDE_RANDOM;
}

void DoNull(Position* pos, UNDO* u)
{
	u->ep_sq = pos->ep_sq;
	u->key = pos->key;
	pos->rep_list[pos->head++] = pos->key;
	pos->rev_moves++;
	if (pos->ep_sq != NO_SQ) {
		pos->key ^= zob_ep[File(pos->ep_sq)];
		pos->ep_sq = NO_SQ;
	}
	pos->side ^= 1;
	pos->key ^= SIDE_RANDOM;
}
