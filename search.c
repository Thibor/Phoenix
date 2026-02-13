#include <stdio.h>
#include <string.h>
#include "main.h"

void SearchRoot(Position* p, int* pv)
{
	ClearHist();
	tt_date = (tt_date + 1) & 255;
	info.nodes = 0;
	info.stop = 0;
	info.timeStart = GetTimeMs();
	for (int root_depth = 1; root_depth <= info.depthLimit; root_depth++) {
		SearchAlpha(p, 0, -INF, INF, root_depth, pv);
		if (info.stop)
			break;
	}
}

int SearchAlpha(Position* p, int ply, int alpha, int beta, int depth, int* pv)
{
	int best, score, move, new_depth, new_pv[MAX_PLY];
	MOVES m[1];
	UNDO u[1];

	if (depth <= 0)
		return SearchQuiesce(p, ply, alpha, beta, pv);
	CheckUp();
	if (info.stop) return 0;
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
	best = -INF;
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

int SearchQuiesce(Position* p, int ply, int alpha, int beta, int* pv)
{
	int best, score, move, new_pv[MAX_PLY];
	MOVES m[1];
	UNDO u[1];

	CheckUp();
	if (info.stop) return 0;
	*pv = 0;
	if (Repetition(p))
		return 0;
	if (ply >= MAX_PLY - 1)
		return Evaluate(p);
	best = Evaluate(p);
	if (best >= beta)
		return best;
	if (best > alpha)
		alpha = best;
	InitCaptures(p, m);
	while ((move = NextCapture(m))) {
		DoMove(p, move, u);
		if (Illegal(p)) { UndoMove(p, move, u); continue; }
		score = -SearchQuiesce(p, ply + 1, -beta, -alpha, new_pv);
		UndoMove(p, move, u);
		if (info.stop) return 0;
		if (score >= beta)
			return score;
		if (score > best) {
			best = score;
			if (score > alpha) {
				alpha = score;
				BuildPv(pv, new_pv, move);
			}
		}
	}
	return best;
}

int Repetition(Position* p)
{
	int i;

	for (i = 4; i <= p->rev_moves; i += 2)
		if (p->key == p->rep_list[p->head - i])
			return 1;
	return 0;
}

void DisplayPv(int depth,int score, int* pv)
{
	char* type, pv_str[512];

	type = "mate";
	if (score < -MAX_EVAL)
		score = (-MATE - score) / 2;
	else if (score > MAX_EVAL)
		score = (MATE - score + 1) / 2;
	else
		type = "cp";
	PvToStr(pv, pv_str);
	printf("info depth %d time %d nodes %d score %s %d hashfull %d pv %s\n", depth, GetTimeMs() - info.timeStart, info.nodes, type, score, TransPermill(), pv_str);
}

void CheckUp(void)
{
	char command[80];

	if (++info.nodes & 0xffff)
		return;
	if (InputAvailable()) {
		ReadLine(command, sizeof(command));
		if (strcmp(command, "stop") == 0)
			info.stop = 1;
		else if (strcmp(command, "ponderhit") == 0)
			info.ponder = 0;
	}
	if (!info.ponder && info.timeLimit && GetTimeMs() - info.timeStart >= info.timeLimit)
		info.stop = 1;
	if (!info.ponder && info.nodesLimit && info.nodes >= info.nodesLimit)
		info.stop = 1;
}
