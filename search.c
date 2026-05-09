#include <stdio.h>
#include <string.h>
#include "main.h"

int SearchQuiesce(Position* p, int ply, int alpha, int beta, int* pv){
	int best, score, move, new_pv[MAX_PLY];
	MOVES m[1];
	UNDO u[1];
	if (CheckUp(p))
		return 0;
	*pv = 0;
	if (Repetition(p))
		return 0;
	if (ply >= MAX_PLY - 1)
		return Evaluate(p);
	best = Evaluate(p);
	if (best >= beta)
		return best;
	if (alpha < best)
		alpha = best;
	InitCaptures(p, m);
	while ((move = NextCapture(m))) {
		DoMove(p, move, u);
		if (Illegal(p)) {
			UndoMove(p, move, u);
			continue;
		}
		score = -SearchQuiesce(p, ply + 1, -beta, -alpha, new_pv);
		UndoMove(p, move, u);
		if (info.stop) return 0;
		if (best < score) {
			best = score;
			if (alpha < score) {
				alpha = score;
				if (alpha >= beta)
					return beta;
				BuildPv(pv, new_pv, move);
			}
		}
	}
	return best;
}

int SearchAlpha(Position* p, int ply, int alpha, int beta, int depth, int* pv){
	int score, move, new_depth, new_pv[MAX_PLY];
	MOVES m[1];
	UNDO u[1];
	if (depth < 1)
		return SearchQuiesce(p, ply, alpha, beta, pv);
	if (CheckUp(p))
		return 0;
	if (ply) *pv = 0;
	if (Repetition(p) && ply)
		return 0;
	move = 0;
	if (TransRetrieve(p->key, &move, &score, alpha, beta, depth, ply))
		return score;
	if (ply >= MAX_PLY - 1)
		return Evaluate(p);
	if (depth > 1 && beta <= Evaluate(p) && !InCheck(p) && MayNull(p)) {
		DoNull(p, u);
		score = -SearchAlpha(p, ply + 1, -beta, -beta + 1, depth - 3, new_pv);
		UndoNull(p, u);
		if (info.stop) return 0;
		if (score >= beta) {
			TransStore(p->key, 0, score, LOWER, depth, ply);
			return score;
		}
	}
	int best = -INF;
	InitMoves(p, m, move, ply);
	while ((move = NextMove(m))) {
		DoMove(p, move, u);
		if (Illegal(p)) { UndoMove(p, move, u); continue; }
		new_depth = depth - 1 + InCheck(p);
		if (best == -INF)
			score = -SearchAlpha(p, ply + 1, -beta, -alpha, new_depth, new_pv);
		else {
			score = -SearchAlpha(p, ply + 1, -alpha - 1, -alpha, new_depth, new_pv);
			if (!info.stop && score > alpha && score < beta)
				score = -SearchAlpha(p, ply + 1, -beta, -alpha, new_depth, new_pv);
		}
		UndoMove(p, move, u);
		if (info.stop) return 0;
		if (score >= beta) {
			Hist(p, move, depth, ply);
			TransStore(p->key, move, score, LOWER, depth, ply);
			return score;
		}
		if (score > best) {
			best = score;
			if (score > alpha) {
				alpha = score;
				BuildPv(pv, new_pv, move);
				if (!ply) DisplayPv(depth,score, pv);
			}
		}
	}
	if (best == -INF)
		return InCheck(p) ? -MATE + ply : 0;
	if (*pv) {
		Hist(p, *pv, depth, ply);
		TransStore(p->key, *pv, best, EXACT, depth, ply);
	}
	else
		TransStore(p->key, 0, best, UPPER, depth, ply);
	return best;
}

void SearchRoot(Position* p){
	int pv[MAX_PLY];
	ClearHist();
	tt_date = (tt_date + 1) & 255;
	for (int root_depth = 1; root_depth <= info.depthLimit; root_depth++) {
		SearchAlpha(p, 0, -INF, INF, root_depth, pv);
		if (info.stop)
			break;
	}
	char best_str[6];
	char ponder_str[6];
	MoveToStr(pv[0], best_str);
	if (pv[1]) {
		MoveToStr(pv[1], ponder_str);
		printf("bestmove %s ponder %s\n", best_str, ponder_str);
	}
	else
		printf("bestmove %s\n", best_str);
}

int Repetition(Position* p){
	for (int i = 4; i <= p->rev_moves; i += 2)
		if (p->key == p->rep_list[p->head - i])
			return 1;
	return 0;
}

void DisplayPv(int depth,int score, int* pv){
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

int CheckUp(Position* pos){
	if ((++info.nodes & 0xffff) == 0) {
		if (!info.ponder && info.timeLimit && GetTimeMs() - info.timeStart >= info.timeLimit)
			info.stop = 1;
		if (!info.ponder && info.nodesLimit && info.nodes >= info.nodesLimit)
			info.stop = 1;
		if (InputAvailable()) {
			char command[80];
			ReadLine(command, sizeof(command));
			UciCommand(pos,command);
		}
	}
	return info.stop;
}
