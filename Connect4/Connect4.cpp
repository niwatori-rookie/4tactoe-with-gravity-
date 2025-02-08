// Connect4.cpp
//
// GP23A035 金野 翔太
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define	YMAX	6	// 縦方向のマス目の数
#define	XMAX	7	// 横方向のマス目の数

int board[XMAX][YMAX];	// 0:未着手 1:先手のコマ● 2:後手のコマ○
int num[XMAX];	// 各列に置かれているコマの数

int choice = 1;	// 0:未選択
				// 1:人間(先手●)対人間(後手〇)
				// 2:人間(先手●)対コンピュータ(後手〇)
				// 3:コンピュータ(先手●)対人間(後手〇)
int turn = 0;	// 先手か後手かを判定するための着手数

void InitBoard(void);
void DispBoard(void);
int CheckFull(void);
int SetXpos(int);
int CheckBoard(int, int, int);
int ComputeXpos(int);
int SearchConnect(int, int);

int main(void)
{
	int xpos;		// 入力したコマの置き場所，またはコンピュータが探したコマの置き場所
	int player;		// 1:先手の手番 2:後手の手番
	int win = 0;	// 0:プレイ中 1:先手の勝ち 2:後手の勝ち 3:引き分け
	int ans;

	while (1)
	{
		srand((unsigned int)time(NULL));	// 乱数を使う場合のseedの初期化

		printf("Connect 4 を開始します！\n");
		for (choice = 0; choice <= 0 || choice > 3; )
		{
			printf("1:人間(先手●)対人間(後手〇)\n");
			printf("2:人間(先手●)対コンピュータ(後手〇)\n");
			printf("3:コンピュータ(先手●)対人間(後手〇)\n");
			printf("どれにしますか？ 1,2,3のいずれかを半角で入力してください: ");
			scanf_s("%d", &choice);
			printf("\n");
		}

		InitBoard();	// 盤を初期化
		DispBoard();	// 盤を表示
		turn = win = 0;	// 着手数，勝敗状態を初期化

		while (win == 0)
		{
			if (CheckFull() == 1)	// 盤がコマで埋め尽くされているかどうか判定
				win = 3;
			else
			{
				player = (++turn % 2) == 1 ? 1 : 2;	// turnが奇数なら先手，偶数なら後手
				xpos = SetXpos(player);	// playerの手の置き場所を入力または計算
				DispBoard();	// 盤を表示
				win = CheckBoard(player, xpos, 4);	// playerの勝ちかどうか判定
			}
		}

		if (win == 1)
			printf("先手● の勝ち\n");
		else if (win == 2)
			printf("後手○ の勝ち\n");
		else
			printf("引き分け\n");

		printf("\n続けますか？(yes=0 or no=1): ");
		scanf_s("%d", &ans);
		printf("\n");
		if (ans == 1)
			return 0;
	}
}

void InitBoard(void)	// 盤を初期化
{
	int x, y;

	for (x = 0; x < XMAX; x++)
	{
		for (y = 0; y < YMAX; y++)
		{
			board[x][y] = 0;
		}
		num[x] = 0;
	}
}

void DispBoard(void)	// 盤を表示
{
	int x, y;

	printf("０１２３４５６ \n");

	for (y = YMAX - 1; y >= 0; y--)
	{
		for (x = 0; x < XMAX; x++)
		{ 
			printf("%s", board[x][y] == 0 ? "・" : board[x][y] == 1 ? "● " : "〇");	
		}
		printf("\n");
	}
	printf("\n");
}

int CheckFull(void)		// 戻り値1:盤がコマで埋め尽くされて置き場所なし 0:置き場所あり
{
	int x;

	for (x = 0; x < XMAX; x++)
	{
		if (num[x] < YMAX)
			return 0;
	}
	return 1;
}

int SetXpos(int player)	// コマの置き場所を入力または探して盤を更新，コマの置き場所(0-6)を返す
{
	int xpos;	// コマの置き場所

	if (choice == 2 && player == 2 || choice == 3 && player == 1)	// コンピュータとの対戦の場合に，コンピュータの手を計算
	{
		xpos = ComputeXpos(player);
		printf("%sの番です\n", player == 1 ? "● " : "〇");
		printf("コンピュータは%dに置きます\n", xpos);
	}
	else	// 人間の手を選択
	{
		while (1)
		{
			printf("%sの番です\n", player == 1 ? "● " : "〇");
			printf("どこに置きますか？ 0-6の数字を半角で入力してください: ");
			scanf_s("%d", &xpos);
			if (xpos >= 0 && xpos < XMAX && num[xpos] < YMAX)	// 置ける場所かどうか判定
				break;
			printf("%dには置けません!\n", xpos);
		}
	}
	board[xpos][num[xpos]++] = player;	// コマをxpos列に置き，xpos列のコマ数を1増やす
	return xpos;
}


int CountPiece(int player, int xpos, int ypos, int dx, int dy)	// playerのコマを(xpos, ypos)に置いたときのdy/dx方向の連続数を返す
{
	int x, y, n = 1;

	for (x = xpos + dx, y = ypos + dy; x >= 0 && x < XMAX && y >= 0 && y < YMAX && board[x][y] == player; x += dx, y += dy)
		n++;
	for (x = xpos - dx, y = ypos - dy; x >= 0 && x < XMAX && y >= 0 && y < YMAX && board[x][y] == player; x -= dx, y -= dy)
		n++;
	return n;
}

int CheckBoard(int player, int xpos, int con)	// 戻り値player:xposに置いたplayerのコマでcon個連続 0:con個未満
{
	
	int ypos = num[xpos] - 1;	// コマが置かれた行

	// ypos行の横方向の並びをチェック
	if (CountPiece(player, xpos, ypos, 1, 0) >= con)
		return player;
	// xpos列の縦方向の並びをチェック
	if (CountPiece(player, xpos, ypos, 0, 1) >= con)
		return player;
	// 斜め右上がりの並びをチェック
	if (CountPiece(player, xpos, ypos, 1, 1) >= con)
		return player;
	// 斜め右下がりの並びをチェック
	if (CountPiece(player, xpos, ypos, 1, -1) >= con)
		return player;

	return 0;
}

int ComputeXpos(int player)	// コンピュータがplayerのときの置き場所を探し，その場所(0-6)を返す
{
	int xpos = 0;
	int enemy;

/*
	enemy = ??????????;

	// SearchConnect関数を使って，playerが勝てる手を探す
	// SearchConnect関数を使って，enemyが勝てる手を探してつぶす
	// SearchConnect関数を使って，enemyが3つになる手を探してつぶす
	// SearchConnect関数を使って，playerが3つになる手を探す
	// ランダムに手を探す
*/
	return xpos;
}

int SearchConnect(int player, int con)	// playerがcon個になる手を探す 戻り値x:見つかった -1:見つからなかった
{
	int x;

	// 置ける場所に順番に置いてみてチェックし，元に戻す
	return -1;
}