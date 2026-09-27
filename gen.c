#include "main.h"

int* GenerateCaptures(Position* pos, int* list)
{
	U64 pieces, moves;
	int side, from, to;

	side = pos->side;
	if (side == WHITE) {
		moves = ((PcBb(pos, WHITE, P) & ~FILE_A_BB & RANK_7_BB) << 7) & pos->cl_bb[BLACK];
		while (moves) {
			to = FirstOne(moves);
			*list++ = (Q_PROM << 12) | (to << 6) | (to - 7);
			*list++ = (R_PROM << 12) | (to << 6) | (to - 7);
			*list++ = (B_PROM << 12) | (to << 6) | (to - 7);
			*list++ = (N_PROM << 12) | (to << 6) | (to - 7);
			moves &= moves - 1;
		}
		moves = ((PcBb(pos, WHITE, P) & ~FILE_H_BB & RANK_7_BB) << 9) & pos->cl_bb[BLACK];
		while (moves) {
			to = FirstOne(moves);
			*list++ = (Q_PROM << 12) | (to << 6) | (to - 9);
			*list++ = (R_PROM << 12) | (to << 6) | (to - 9);
			*list++ = (B_PROM << 12) | (to << 6) | (to - 9);
			*list++ = (N_PROM << 12) | (to << 6) | (to - 9);
			moves &= moves - 1;
		}
		moves = ((PcBb(pos, WHITE, P) & RANK_7_BB) << 8) & UnoccBb(pos);
		while (moves) {
			to = FirstOne(moves);
			*list++ = (Q_PROM << 12) | (to << 6) | (to - 8);
			*list++ = (R_PROM << 12) | (to << 6) | (to - 8);
			*list++ = (B_PROM << 12) | (to << 6) | (to - 8);
			*list++ = (N_PROM << 12) | (to << 6) | (to - 8);
			moves &= moves - 1;
		}
		moves = ((PcBb(pos, WHITE, P) & ~FILE_A_BB & ~RANK_7_BB) << 7) & pos->cl_bb[BLACK];
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | (to - 7);
			moves &= moves - 1;
		}
		moves = ((PcBb(pos, WHITE, P) & ~FILE_H_BB & ~RANK_7_BB) << 9) & pos->cl_bb[BLACK];
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | (to - 9);
			moves &= moves - 1;
		}
		if ((to = pos->ep_sq) != NO_SQ) {
			if (((PcBb(pos, WHITE, P) & ~FILE_A_BB) << 7) & SqBb(to))
				*list++ = (EP_CAP << 12) | (to << 6) | (to - 7);
			if (((PcBb(pos, WHITE, P) & ~FILE_H_BB) << 9) & SqBb(to))
				*list++ = (EP_CAP << 12) | (to << 6) | (to - 9);
		}
	}
	else {
		moves = ((PcBb(pos, BLACK, P) & ~FILE_A_BB & RANK_2_BB) >> 9) & pos->cl_bb[WHITE];
		while (moves) {
			to = FirstOne(moves);
			*list++ = (Q_PROM << 12) | (to << 6) | (to + 9);
			*list++ = (R_PROM << 12) | (to << 6) | (to + 9);
			*list++ = (B_PROM << 12) | (to << 6) | (to + 9);
			*list++ = (N_PROM << 12) | (to << 6) | (to + 9);
			moves &= moves - 1;
		}
		moves = ((PcBb(pos, BLACK, P) & ~FILE_H_BB & RANK_2_BB) >> 7) & pos->cl_bb[WHITE];
		while (moves) {
			to = FirstOne(moves);
			*list++ = (Q_PROM << 12) | (to << 6) | (to + 7);
			*list++ = (R_PROM << 12) | (to << 6) | (to + 7);
			*list++ = (B_PROM << 12) | (to << 6) | (to + 7);
			*list++ = (N_PROM << 12) | (to << 6) | (to + 7);
			moves &= moves - 1;
		}
		moves = ((PcBb(pos, BLACK, P) & RANK_2_BB) >> 8) & UnoccBb(pos);
		while (moves) {
			to = FirstOne(moves);
			*list++ = (Q_PROM << 12) | (to << 6) | (to + 8);
			*list++ = (R_PROM << 12) | (to << 6) | (to + 8);
			*list++ = (B_PROM << 12) | (to << 6) | (to + 8);
			*list++ = (N_PROM << 12) | (to << 6) | (to + 8);
			moves &= moves - 1;
		}
		moves = ((PcBb(pos, BLACK, P) & ~FILE_A_BB & ~RANK_2_BB) >> 9) & pos->cl_bb[WHITE];
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | (to + 9);
			moves &= moves - 1;
		}
		moves = ((PcBb(pos, BLACK, P) & ~FILE_H_BB & ~RANK_2_BB) >> 7) & pos->cl_bb[WHITE];
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | (to + 7);
			moves &= moves - 1;
		}
		if ((to = pos->ep_sq) != NO_SQ) {
			if (((PcBb(pos, BLACK, P) & ~FILE_A_BB) >> 9) & SqBb(to))
				*list++ = (EP_CAP << 12) | (to << 6) | (to + 9);
			if (((PcBb(pos, BLACK, P) & ~FILE_H_BB) >> 7) & SqBb(to))
				*list++ = (EP_CAP << 12) | (to << 6) | (to + 7);
		}
	}
	pieces = PcBb(pos, side, N);
	while (pieces) {
		from = FirstOne(pieces);
		moves = n_attacks[from] & pos->cl_bb[Opp(side)];
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | from;
			moves &= moves - 1;
		}
		pieces &= pieces - 1;
	}
	pieces = PcBb(pos, side, B);
	while (pieces) {
		from = FirstOne(pieces);
		moves = BAttacks(OccBb(pos), from) & pos->cl_bb[Opp(side)];
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | from;
			moves &= moves - 1;
		}
		pieces &= pieces - 1;
	}
	pieces = PcBb(pos, side, R);
	while (pieces) {
		from = FirstOne(pieces);
		moves = RAttacks(OccBb(pos), from) & pos->cl_bb[Opp(side)];
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | from;
			moves &= moves - 1;
		}
		pieces &= pieces - 1;
	}
	pieces = PcBb(pos, side, Q);
	while (pieces) {
		from = FirstOne(pieces);
		moves = QAttacks(OccBb(pos), from) & pos->cl_bb[Opp(side)];
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | from;
			moves &= moves - 1;
		}
		pieces &= pieces - 1;
	}
	moves = k_attacks[KingSq(pos, side)] & pos->cl_bb[Opp(side)];
	while (moves) {
		to = FirstOne(moves);
		*list++ = (to << 6) | KingSq(pos, side);
		moves &= moves - 1;
	}
	return list;
}

int* GenerateQuiet(Position* pos, int* list)
{
	U64 pieces, moves;
	int side, from, to;

	side = pos->side;
	if (side == WHITE) {
		if ((pos->c_flags & 1) && !(OccBb(pos) & (U64)0x0000000000000060))
			if (!Attacked(pos, E1, BLACK) && !Attacked(pos, F1, BLACK))
				*list++ = (CASTLE << 12) | (G1 << 6) | E1;
		if ((pos->c_flags & 2) && !(OccBb(pos) & (U64)0x000000000000000E))
			if (!Attacked(pos, E1, BLACK) && !Attacked(pos, D1, BLACK))
				*list++ = (CASTLE << 12) | (C1 << 6) | E1;
		moves = ((((PcBb(pos, WHITE, P) & RANK_2_BB) << 8) & UnoccBb(pos)) << 8) & UnoccBb(pos);
		while (moves) {
			to = FirstOne(moves);
			*list++ = (EP_SET << 12) | (to << 6) | (to - 16);
			moves &= moves - 1;
		}
		moves = ((PcBb(pos, WHITE, P) & ~RANK_7_BB) << 8) & UnoccBb(pos);
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | (to - 8);
			moves &= moves - 1;
		}
	}
	else {
		if ((pos->c_flags & 4) && !(OccBb(pos) & (U64)0x6000000000000000))
			if (!Attacked(pos, E8, WHITE) && !Attacked(pos, F8, WHITE))
				*list++ = (CASTLE << 12) | (G8 << 6) | E8;
		if ((pos->c_flags & 8) && !(OccBb(pos) & (U64)0x0E00000000000000))
			if (!Attacked(pos, E8, WHITE) && !Attacked(pos, D8, WHITE))
				*list++ = (CASTLE << 12) | (C8 << 6) | E8;
		moves = ((((PcBb(pos, BLACK, P) & RANK_7_BB) >> 8) & UnoccBb(pos)) >> 8) & UnoccBb(pos);
		while (moves) {
			to = FirstOne(moves);
			*list++ = (EP_SET << 12) | (to << 6) | (to + 16);
			moves &= moves - 1;
		}
		moves = ((PcBb(pos, BLACK, P) & ~RANK_2_BB) >> 8) & UnoccBb(pos);
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | (to + 8);
			moves &= moves - 1;
		}
	}
	pieces = PcBb(pos, side, N);
	while (pieces) {
		from = FirstOne(pieces);
		moves = n_attacks[from] & UnoccBb(pos);
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | from;
			moves &= moves - 1;
		}
		pieces &= pieces - 1;
	}
	pieces = PcBb(pos, side, B);
	while (pieces) {
		from = FirstOne(pieces);
		moves = BAttacks(OccBb(pos), from) & UnoccBb(pos);
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | from;
			moves &= moves - 1;
		}
		pieces &= pieces - 1;
	}
	pieces = PcBb(pos, side, R);
	while (pieces) {
		from = FirstOne(pieces);
		moves = RAttacks(OccBb(pos), from) & UnoccBb(pos);
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | from;
			moves &= moves - 1;
		}
		pieces &= pieces - 1;
	}
	pieces = PcBb(pos, side, Q);
	while (pieces) {
		from = FirstOne(pieces);
		moves = QAttacks(OccBb(pos), from) & UnoccBb(pos);
		while (moves) {
			to = FirstOne(moves);
			*list++ = (to << 6) | from;
			moves &= moves - 1;
		}
		pieces &= pieces - 1;
	}
	moves = k_attacks[KingSq(pos, side)] & UnoccBb(pos);
	while (moves) {
		to = FirstOne(moves);
		*list++ = (to << 6) | KingSq(pos, side);
		moves &= moves - 1;
	}
	return list;
}
