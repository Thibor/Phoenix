#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

void Init(void)
{
    int i, j, k, l, x, y;
    static const int dirs[4][2] = { {1, -1}, {16, -16}, {17, -17}, {15, -15} };
    static const int p_moves[2][2] = { {15, 17}, {-17, -15} };
    static const int n_moves[8] = { -33, -31, -18, -14, 14, 18, 31, 33 };
    static const int k_moves[8] = { -17, -16, -15, -1, 1, 15, 16, 17 };
    static const int line[8] = { 0, 2, 4, 5, 5, 4, 2, 0 };

    for (i = 0; i < 64; i++) {
        line_mask[0][i] = RANK_1_BB << (i & 070);
        line_mask[1][i] = FILE_A_BB << (i & 007);
        j = File(i) - Rank(i);
        if (j > 0)
            line_mask[2][i] = DIAG_A1H8_BB >> (j * 8);
        else
            line_mask[2][i] = DIAG_A1H8_BB << (-j * 8);
        j = File(i) - (RANK_8 - Rank(i));
        if (j > 0)
            line_mask[3][i] = DIAG_A8H1_BB << (j * 8);
        else
            line_mask[3][i] = DIAG_A8H1_BB >> (-j * 8);
    }
    for (i = 0; i < 4; i++)
        for (j = 0; j < 64; j++)
            for (k = 0; k < 64; k++) {
                attacks[i][j][k] = 0;
                for (l = 0; l < 2; l++) {
                    x = Map0x88(j) + dirs[i][l];
                    while (!Sq0x88Off(x)) {
                        y = Unmap0x88(x);
                        attacks[i][j][k] |= SqBb(y);
                        if ((k << 1) & (1 << (i != 1 ? File(y) : Rank(y))))
                            break;
                        x += dirs[i][l];
                    }
                }
            }
    for (i = 0; i < 2; i++)
        for (j = 0; j < 64; j++) {
            p_attacks[i][j] = 0;
            for (k = 0; k < 2; k++) {
                x = Map0x88(j) + p_moves[i][k];
                if (!Sq0x88Off(x))
                    p_attacks[i][j] |= SqBb(Unmap0x88(x));
            }
        }
    for (i = 0; i < 64; i++) {
        n_attacks[i] = 0;
        for (j = 0; j < 8; j++) {
            x = Map0x88(i) + n_moves[j];
            if (!Sq0x88Off(x))
                n_attacks[i] |= SqBb(Unmap0x88(x));
        }
    }
    for (i = 0; i < 64; i++) {
        k_attacks[i] = 0;
        for (j = 0; j < 8; j++) {
            x = Map0x88(i) + k_moves[j];
            if (!Sq0x88Off(x))
                k_attacks[i] |= SqBb(Unmap0x88(x));
        }
    }
    for (i = 0; i < 64; i++) {
        passed_mask[WC][i] = 0;
        for (j = File(i) - 1; j <= File(i) + 1; j++) {
            if ((File(i) == FILE_A && j == -1) ||
                (File(i) == FILE_H && j == 8))
                continue;
            for (k = Rank(i) + 1; k <= RANK_8; k++)
                passed_mask[WC][i] |= SqBb(Sq(j, k));
        }
    }
    for (i = 0; i < 64; i++) {
        passed_mask[BC][i] = 0;
        for (j = File(i) - 1; j <= File(i) + 1; j++) {
            if ((File(i) == FILE_A && j == -1) ||
                (File(i) == FILE_H && j == 8))
                continue;
            for (k = Rank(i) - 1; k >= RANK_1; k--)
                passed_mask[BC][i] |= SqBb(Sq(j, k));
        }
    }
    for (i = 0; i < 8; i++) {
        adjacent_mask[i] = 0;
        if (i > 0)
            adjacent_mask[i] |= FILE_A_BB << (i - 1);
        if (i < 7)
            adjacent_mask[i] |= FILE_A_BB << (i + 1);
    }
    for (i = 0; i < 64; i++) {
        j = line[File(i)] + line[Rank(i)];
        pst[P][i] = j * 2;
        pst[N][i] = j * 4;
        pst[B][i] = j * 2;
        pst[R][i] = line[File(i)];
        pst[Q][i] = j;
        pst[K][i] = j * 6;
    }
    for (i = 0; i < 64; i++)
        c_mask[i] = 15;
    c_mask[A1] = 13;
    c_mask[E1] = 12;
    c_mask[H1] = 14;
    c_mask[A8] = 7;
    c_mask[E8] = 3;
    c_mask[H8] = 11;
    for (i = 0; i < 12; i++)
        for (j = 0; j < 64; j++)
            zob_piece[i][j] = Random64();
    for (i = 0; i < 16; i++)
        zob_castle[i] = Random64();
    for (i = 0; i < 8; i++)
        zob_ep[i] = Random64();
}

void ReadLine(char* str, int n)
{
	char* ptr;

	if (fgets(str, n, stdin) == NULL)
		exit(0);
	if ((ptr = strchr(str, '\n')) != NULL)
		*ptr = '\0';
}

char* ParseToken(char* string, char* token)
{
	while (*string == ' ')
		string++;
	while (*string != ' ' && *string != '\0')
		*token++ = *string++;
	*token = '\0';
	return string;
}

static void PrintBoard(POS* p) {
	const char* s = "   +---+---+---+---+---+---+---+---+\n";
	const char* t = "     A   B   C   D   E   F   G   H\n";
	printf(t);
	for (int r = 0; r < 8; r++) {
		printf(s);
		printf(" %d |", 8 - r);
		for (int f = 0; f < 8; f++) {
			int sq = Sq(f, 7 - r);
			char c = "AaNnBbRrQqKk "[p->pc[sq]];
			printf(" %c |", c);
		}
		printf(" %d\n", 8 - r);
	}
	printf(s);
	printf(t);
}

void PrintWelcome() {
	printf("%s %s\n", NAME, VERSION);
}

void UciLoop(void) {
	char command[4096], token[80], * ptr;
	POS p[1];
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	SetPosition(p, START_FEN);
	AllocTrans(16);
	PrintWelcome();
	while (1) {
		ReadLine(command, sizeof(command));
		ptr = ParseToken(command, token);
		if (strcmp(token, "uci") == 0) {
			printf("id name %s\n", NAME);
			printf("option name Hash type spin default 16 min 1 max 4096\n");
			printf("option name Clear Hash type button\n");
			printf("uciok\n");
		}
		else if (strcmp(token, "isready") == 0) {
			printf("readyok\n");
		}
		else if (strcmp(token, "setoption") == 0) {
			ParseSetoption(ptr);
		}
		else if (strcmp(token, "position") == 0) {
			ParsePosition(p, ptr);
		}
		else if (strcmp(token, "go") == 0) {
			ParseGo(p, ptr);
		}
		else if (strcmp(token, "print") == 0) {
			PrintBoard(p);
		}
		else if (strcmp(token, "quit") == 0) {
			exit(0);
		}
	}
}

void ParseSetoption(char* ptr) {
	char token[80], name[80] = { 0 }, value[80] = { 0 };

	ptr = ParseToken(ptr, token);
	name[0] = '\0';
	for (;;) {
		ptr = ParseToken(ptr, token);
		if (*token == '\0' || strcmp(token, "value") == 0)
			break;
		strcat(name, token);
		strcat(name, " ");
	}
	name[strlen(name) - 1] = '\0';
	if (strcmp(token, "value") == 0) {
		value[0] = '\0';
		for (;;) {
			ptr = ParseToken(ptr, token);
			if (*token == '\0')
				break;
			strcat(value, token);
			strcat(value, " ");
		}
		value[strlen(value) - 1] = '\0';
	}
	if (strcmp(name, "Hash") == 0) {
		int h = atoi(value);
		AllocTrans(h);
	}
	else if (strcmp(name, "Clear Hash") == 0) {
		ClearTrans();
	}
}

void ParsePosition(POS* p, char* ptr) {
	char token[80], fen[80];
	UNDO u[1];

	ptr = ParseToken(ptr, token);
	if (strcmp(token, "fen") == 0) {
		fen[0] = '\0';
		for (;;) {
			ptr = ParseToken(ptr, token);
			if (*token == '\0' || strcmp(token, "moves") == 0)
				break;
			strcat(fen, token);
			strcat(fen, " ");
		}
		SetPosition(p, fen);
	}
	else {
		ptr = ParseToken(ptr, token);
		SetPosition(p, START_FEN);
	}
	if (strcmp(token, "moves") == 0)
		for (;;) {
			ptr = ParseToken(ptr, token);
			if (*token == '\0')
				break;
			DoMove(p, StrToMove(p, token), u);
			if (p->rev_moves == 0)
				p->head = 0;
		}
}

void ParseGo(POS* pos, char* ptr) {
	char token[80], bestmove_str[6], ponder_str[6];
	int wtime, btime, winc, binc, movestogo, time, inc, pv[MAX_PLY];
	int movetime, movedepth, nodes;
	info.post = 1;
	info.ponder = 0;
	info.nodes = 0;
	info.nodesLimit = 0;
	info.timeLimit = 0;
	info.depthLimit = 255;
	info.ponder = 0;
	wtime = -1;
	btime = -1;
	winc = 0;
	binc = 0;
	movestogo = 40;
	movetime = 0;
	movedepth = 0;
	for (;;) {
		ptr = ParseToken(ptr, token);
		if (*token == '\0')
			break;
		if (strcmp(token, "ponder") == 0) {
			info.ponder = 1;
		}
		else if (strcmp(token, "wtime") == 0) {
			ptr = ParseToken(ptr, token);
			wtime = atoi(token);
		}
		else if (strcmp(token, "btime") == 0) {
			ptr = ParseToken(ptr, token);
			btime = atoi(token);
		}
		else if (strcmp(token, "winc") == 0) {
			ptr = ParseToken(ptr, token);
			winc = atoi(token);
		}
		else if (strcmp(token, "binc") == 0) {
			ptr = ParseToken(ptr, token);
			binc = atoi(token);
		}
		else if (strcmp(token, "movestogo") == 0) {
			ptr = ParseToken(ptr, token);
			movestogo = atoi(token);
		}
		else if (strcmp(token, "movetime") == 0) {
			ptr = ParseToken(ptr, token);
			movetime = atoi(token);
		}
		else if (strcmp(token, "depth") == 0) {
			ptr = ParseToken(ptr, token);
			movedepth = atoi(token);
		}
		else if (strcmp(token, "nodes") == 0) {
			ptr = ParseToken(ptr, token);
			nodes = atoi(token);
		}
	}
	if (movedepth > 0)
		info.depthLimit = movedepth;
	if (nodes > 0)
		info.nodesLimit = nodes;
	if (movetime > 0)
		info.timeLimit = movetime;
	else {
		time = pos->side == WC ? wtime : btime;
		inc = pos->side == WC ? winc : binc;
		if (time >= 0) {
			if (movestogo == 1) time -= Min(1000, time / 10);
			info.timeLimit = (time + inc * (movestogo - 1)) / movestogo;
			if (info.timeLimit > time)
				info.timeLimit = time;
			info.timeLimit -= 10;
			if (info.timeLimit < 0)
				info.timeLimit = 0;
		}
	}
	SearchRoot(pos, pv);
	MoveToStr(pv[0], bestmove_str);
	if (pv[1]) {
		MoveToStr(pv[1], ponder_str);
		printf("bestmove %s ponder %s\n", bestmove_str, ponder_str);
	}
	else
		printf("bestmove %s\n", bestmove_str);
}

int main()
{
  Init();
  UciLoop();
  return 0;
}
