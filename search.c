#include <stdio.h>
#include <string.h>
#include "main.h"

int SearchQuiesce(Position* pos, int ply, int alpha, int beta, int* pv) {
	int best, score, moves, new_pv[MAX_PLY];
	MOVES m[1];
	UNDO u[1];
	if (CheckUp(pos))
		return 0;
	*pv = 0;
	if (Repetition(pos))
		return 0;
	if (ply >= MAX_PLY - 1)
		return Evaluate(pos);
	best = Evaluate(pos);
	if (best >= beta)
		return best;
	if (alpha < best)
		alpha = best;
	InitCaptures(pos, m);
	while ((moves = NextCapture(m))) {
		DoMove(pos, moves, u);
		if (Illegal(pos)) {
			UndoMove(pos, moves, u);
			continue;
		}
		score = -SearchQuiesce(pos, ply + 1, -beta, -alpha, new_pv);
		UndoMove(pos, moves, u);
		if (info.stop) return 0;
		if (best < score) {
			best = score;
			if (alpha < score) {
				alpha = score;
				if (alpha >= beta)
					return beta;
				BuildPv(pv, new_pv, moves);
			}
		}
	}
	return best;
}

int SearchAlpha(Position* pos, int ply, int alpha, int beta, int depth, int* pv) {
	int score, moves, new_depth, new_pv[MAX_PLY];
	MOVES m[1];
	UNDO u[1];
	if (depth < 1)
		return SearchQuiesce(pos, ply, alpha, beta, pv);
	if (CheckUp(pos))
		return 0;
	if (ply) *pv = 0;
	if (Repetition(pos) && ply)
		return 0;
	moves = 0;
	if (TransRetrieve(pos->key, &moves, &score, alpha, beta, depth, ply))
		return score;
	if (ply >= MAX_PLY - 1)
		return Evaluate(pos);
	if (depth > 1 && beta <= Evaluate(pos) && !InCheck(pos) && MayNull(pos)) {
		DoNull(pos, u);
		score = -SearchAlpha(pos, ply + 1, -beta, -beta + 1, depth - 3, new_pv);
		UndoNull(pos, u);
		if (info.stop) return 0;
		if (score >= beta) {
			TransStore(pos->key, 0, score, LOWER, depth, ply);
			return score;
		}
	}
	int best = -INF;
	InitMoves(pos, m, moves, ply);
	while ((moves = NextMove(m))) {
		DoMove(pos, moves, u);
		if (Illegal(pos)) { UndoMove(pos, moves, u); continue; }
		new_depth = depth - 1 + InCheck(pos);
		if (best == -INF)
			score = -SearchAlpha(pos, ply + 1, -beta, -alpha, new_depth, new_pv);
		else {
			score = -SearchAlpha(pos, ply + 1, -alpha - 1, -alpha, new_depth, new_pv);
			if (!info.stop && score > alpha && score < beta)
				score = -SearchAlpha(pos, ply + 1, -beta, -alpha, new_depth, new_pv);
		}
		UndoMove(pos, moves, u);
		if (info.stop) return 0;
		if (score >= beta) {
			Hist(pos, moves, depth, ply);
			TransStore(pos->key, moves, score, LOWER, depth, ply);
			return score;
		}
		if (score > best) {
			best = score;
			if (score > alpha) {
				alpha = score;
				BuildPv(pv, new_pv, moves);
				if (!ply && info.post) DisplayPv(depth, score, pv);
			}
		}
	}
	if (best == -INF)
		return InCheck(pos) ? -MATE + ply : 0;
	if (*pv) {
		Hist(pos, *pv, depth, ply);
		TransStore(pos->key, *pv, best, EXACT, depth, ply);
	}
	else
		TransStore(pos->key, 0, best, UPPER, depth, ply);
	return best;
}

void SearchRoot(Position* pos) {
	int pv[MAX_PLY];
	ClearHist();
	ClearTrans();
	tt_date = (tt_date + 1) & 255;
	for (int root_depth = 1; root_depth <= info.depthLimit; root_depth++) {
		SearchAlpha(pos, 0, -INF, INF, root_depth, pv);
		if (info.stop)
			break;
	}
	char best_str[6];
	char ponder_str[6];
	MoveToStr(pv[0], best_str);
	if (info.post)
		if (pv[1]) {
			MoveToStr(pv[1], ponder_str);
			printf("bestmove %s ponder %s\n", best_str, ponder_str);
		}
		else
			printf("bestmove %s\n", best_str);
}

int Repetition(Position* pos) {
	for (int i = 4; i <= pos->rev_moves; i += 2)
		if (pos->key == pos->rep_list[pos->head - i])
			return 1;
	return 0;
}

void DisplayPv(int depth, int score, int* pv) {
	char* type, pv_str[512];
	type = "mate";
	if (score < -MAX_EVAL)
		score = (-MATE - score) / 2;
	else if (score > MAX_EVAL)
		score = (MATE - score + 1) / 2;
	else
		type = "cp";
	PvToStr(pv, pv_str);
	printf("info depth %d time %llu nodes %llu score %s %d hashfull %d pv %s\n", depth, GetTimeMs() - info.timeStart, info.nodes, type, score, TransPermill(), pv_str);
}

int CheckUp(Position* pos) {
	if ((++info.nodes & 0xffff) == 0) {
		if (!info.ponder && info.timeLimit && GetTimeMs() - info.timeStart >= info.timeLimit)
			info.stop = 1;
		if (!info.ponder && info.nodesLimit && info.nodes >= info.nodesLimit)
			info.stop = 1;
		if (InputAvailable()) {
			char command[80];
			ReadLine(command, sizeof(command));
			UciCommand(pos, command);
		}
	}
	return info.stop;
}
