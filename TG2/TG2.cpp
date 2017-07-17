/*	Título: Jogo de Aventura - Trabalho de Grupo 2
Autores: Daniel Paulino & João Filipe
Versão: 18/01/2016
*/

#include "stdafx.h"
#include "stdlib.h"
#include "locale.h"
#include "string.h"
#include "stdio.h"
#include "time.h"
#include "iostream"
#include "fstream"
#include <sstream>
#include <windows.h>
#include <fstream>
#include <stdio.h>

//Player Options
#define MAX_PLAYER_NAME 100
#define MAX_PLAYER_ENERGY 100
#define MAX_SU_PLAYER_ENERGY 1000
#define PLAYER_INITIAL_CELL 0
#define NO_TREASURE 0

//Monster Options
#define MAX_NAME 100
#define MAX_ENERGY 100
#define MONSTER_CELL 16

//Map Options
#define MAX_CELL 17
#define MAX_CELL_DESCRIPTION 1000
#define TRESURE_CELL 9
#define MAX_LINE 100
#define PROMPT_STR 10
#define CELL_0_DESCRIPTION "Encontraste na entrada da floresta. Não se ouve nada, o ambiente está demasiado calmo ... (N - Entrar)\n"

//Pipe
#define PIPE_NAME
#define BUF_SIZE 512
#define MAX_MSG 512

struct Player{
	char name[MAX_PLAYER_NAME];
	int energy;
	int cell;
	int treasure;
};

struct Cell{
	int north;
	int south;
	int west;
	int east;
	int treasure;
	char description[MAX_CELL_DESCRIPTION];
};

struct Monster{
	char name[MAX_NAME];
	int energy;
	int cell;
	bool dead;
};

struct Player player;
struct Monster monster;
struct Cell cell[MAX_CELL];

bool superU;
bool death = false;
bool gameover = false;
using namespace std;
char prompt[PROMPT_STR];
char pickUp_cmd[] = "loot";
char exit_cmd[] = "exit";
char north[] = "n";
char south[] = "s";
char west[] = "w";
char east[] = "e";
char option_color[] = "";
char color[] = "COLOR ";
char option[PROMPT_STR];

void Intro();
void History();
void Options();
void Colors();
void InitializePlayer(struct Player *pPlayer, bool superU);
void PrintPlayer(struct Player *pPlayer, Monster *pMonster, bool super);
void InitializeMonster(struct Monster *pMonster);
void InitializeMap(struct Cell cell[]);
void PrintMap(struct Cell cell[]);
void ReadMap(struct Cell cell[]);
void SaveBin(struct Cell cell[]);
void ReadBin(struct Cell cell[]);
void SaveGame(Player *pPlayer, Monster *pMonster, Cell cell[]);
void LoadGame(Player *pPlayer, Monster *pMonster, Cell cell[]);
void MonsterFight(struct Player *pPlayer, Monster *pMonster);
void ActionCommand(Player *pPlayer, Monster *pMonster, Cell cell[], bool super);
void ActionPlayer(Cell cell[], Player *pPlayer);
void ActionMonster(Cell cell[], Monster *pMonster, bool super);

HANDLE hMutexPlayer;
HANDLE hMutexMonster;

DWORD WINAPI ThreadPlayer(LPVOID lpParam)
{
	InitializePlayer(&player, superU);
	PrintPlayer(&player, &monster, superU);
	while (player.energy > 0)
	{
		WaitForSingleObject(hMutexPlayer, INFINITE);

		ActionCommand(&player, &monster, cell, superU);
		ActionPlayer(cell, &player);
		system("cls");
		//Sleep(3000);
		ReleaseMutex(hMutexPlayer);
	}
	return 0;
}

DWORD WINAPI ThreadMonster(LPVOID lpParam)
{
	while (monster.energy > 0)
	{
		WaitForSingleObject(hMutexMonster, INFINITE);

		ActionMonster(cell, &monster, superU);
		//printf("Sala: %d\n", monster.cell);
		Sleep(3000);

		ReleaseMutex(hMutexMonster);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Portuguese");
	system("MODE con cols=120 lines=50");

	if (argc > 1 && strcmp(argv[1], "su") == 0) {
		superU = true;
	}
	else{
		superU = false;
	}
	Intro();
	History();
	printf("   Main Menu\n\n");
	Options();

	while (strcmp("3", prompt) == 0){
		system("cls");
		Colors();
		system("cls");
		printf("   Main Menu\n\n");
		Options();
	}
	if (strcmp("1", prompt) == 0){
		system("cls");
		ReadBin(cell);
		//InitializeMap(cell);
		InitializeMonster(&monster);
	}
	if (strcmp("2", prompt) == 0){
		system("cls");
		LoadGame(&player, &monster, cell);
		ReadBin(cell);
		//InitializeMap(cell);
		InitializeMonster(&monster);
	}

	hMutexPlayer = CreateMutex(
		NULL,                       // default security attributes
		FALSE,                      // initially not owned
		NULL);                      // unnamed mutex

	hMutexMonster = CreateMutex(
		NULL,                       // default security attributes
		FALSE,                      // initially not owned
		NULL);                      // unnamed mutex

	HANDLE hThreadPlayer = CreateThread(
		NULL,              // default security attributes
		0,                 // use default stack size  
		ThreadPlayer,        // thread function 
		NULL,             // argument to thread function 
		0,                 // use default creation flags 
		NULL);   // returns the thread identifier 

	HANDLE hThreadMonster = CreateThread(
		NULL,              // default security attributes
		0,                 // use default stack size  
		ThreadMonster,        // thread function 
		NULL,             // argument to thread function 
		0,                 // use default creation flags 
		NULL);   // returns the thread identifier 

	while (gameover == false)
	{
		//system("cls");

		if (player.cell == monster.cell){
			WaitForSingleObject(hMutexPlayer, INFINITE);
			WaitForSingleObject(hMutexMonster, INFINITE);

			printf("Finalmente encontras-te frente a frente com a Hydra. Que comece a batalha pela paz da Aldeia!\n\n");
			MonsterFight(&player, &monster);
			ReleaseMutex(hMutexPlayer);
			//system("pause");
			system("cls");

			if (death == false){
				printf("Finalmente a aldeia pode viver em paz e armonia, Obrigado %s pela tua coragem!\n\n", player.name);
				//system("pause");
				death = true;
			}
		}

		if (death == true)
			gameover = true;
	}
	printf("                               /   \\       \n"
		" _                     )      ((   ))     (\n"
		"(@)                   /|\\      ))_((     /|\\ \n"
		"|-|\\                 / | \\    (/\\|/\\)   / | \\                   (@)\n"
		"| | ----------------/--|-voV---\\'|'/--Vov-|--\\------------------|-|\n"
		"|-|                      '^`   (o o)  '^`                       | |\n"
		"| |                            '\\Y/'                            |-|\n"
		"|-|                       GAME       OVER                       | |\n"
		"| |                                                             |-|\n"
		"|-|                     OBRIGADO POR JOGARES                    | |\n"
		"| |                                                             |-|\n"
		"|_|_____________________________________________________________| |\n"
		"(@)            l   /\\ /        ( (       \\ /\\   l             `\\|-|\n"
		"               l /   V          \\ \\       V   \\ l               (@)\n"
		"               l/               _) )_          \\I\n"
		"                                '\\ /'\n");

	WaitForSingleObject(hThreadPlayer, INFINITE);

	WaitForSingleObject(hThreadMonster, INFINITE);

	CloseHandle(hThreadPlayer);
	CloseHandle(hThreadMonster);

	CloseHandle(hMutexPlayer);
	CloseHandle(hMutexMonster);

	system("pause");
	return 0;
}

void Intro(){
	printf("                                     .-.\n"
		"     A Aventura do Aldeão Destemido {{@}}\n"
		"                    <>               8@8\n"
		"                  .::::.             888\n"
		"              @\\\\/W\\/\\/W\\//@         8@8\n"
		"               \\\\/^\\/\\/^\\//     _    )8(    _\n"
		"                \\_O_<>_O_/     (@)__/8@8\\__(@)\n"
		"           ____________________ `~´-=):(=-`~´\n"
		"          |<><><>  |  |  <><><>|     |.|\n"
		"          |<>      |  |      <>|     | |\n"
		"          |<>      |  |      <>|     |'|\n"
		"          |<>   .--------.   <>|     |.|\n"
		"          |     |   ()   |     |     |S|\n"
		"          |     |   /\\   |     |     |.|\n"
		"          |_____| (O\\/O) |_____|     |'|\n"
		"          |------.  \\/  .------|     | |\n"
		"          |       '.__.'       |     |'|\n"
		"          |        |  |        |     |.|\n"
		"          :        |  |        :     |O|\n"
		"           \\<>     |  |     <>/      |'|\n"
		"            \\<>    |  |    <>/       |.|\n"
		"             \\<>   |  |   <>/        | |\n"
		"              `\\<> |  | <>/'         |'|\n"
		"        13568   `-.|__|.-`   14056   \\ /\n");
	system("pause");
	system("cls");
}

void History()
{
	printf("         ___________________________________________________________\n"
		"       / \\                                                           \\\n"
		"      |   |  Esta história passa-se numa pequena aldeia chamada       |\n"
		"       \\_ |  LittleGarden, situada perto de uma floresta              |\n"
		"          |  assustadora, a qual lhe chamam a Floresta da Morte.      |\n"
		"          |  Anciões da aldeia contam histórias de um monstro         |\n"
		"          |  assustador  que vive nos confins da floresta onde        |\n"
		"          |  também se encontra um grande tesouro, e de como esta     |\n"
		"          |  temivel criatura tem posto fim à vida de qualquer        |\n"
		"          |  animal que tenha o azar de cruzar o seu caminho.         |\n"
		"          |  Estes animais são também parte do alimento dos           |\n"
		"          |  aldeões, ou pelo menos, foram no passado..               |\n"
		"          |  Dizem os habitantes da aldeia, que os homens que         |\n"
		"          |  entraram na floresta para caçar animais, nunca mais      |\n"
		"          |  voltaram, e ouviam-se uns certos rugidos vindos da       |\n"
		"          |  floresta acompanhados de um cheiro intenso de sangue.    |\n"
		"          |                                                           |\n"
		"          |  Anos passaram, e os anciões que possuíam conhecimentos   |\n"
		"          |  de alquimia, implementaram um ritual num recém-nascido,  |\n"
		"          |  cujos pais tinham sido mortos pelo monstro.              |\n"
		"          |  O ritual foi um sucesso e o rapaz recebeu grandes        |\n"
		"          |  poderes, após um treino intensivo durante 15 anos,       |\n"
		"          |  tornou-se um guerreiro destemido.                        |\n"
		"          |  Ganhou o apelido de 'O Aldeão Destemido', e hoje, com    |\n"
		"          |  20 anos, tem a capacidade de saltar mais de 10 metros,   |\n"
		"          |  correr a uma velocidade vertiginosa, e manusear a sua    |\n"
		"          |  espada e escudo como ninguem, um guerreiro completo.     |\n"
		"          |  Chega então o dia da vingança, em que o Aldeão Destemido |\n"
		"          |  entrará na Floresta da Morte para enfrentar o temível    |\n"
		"          |  monstro, Hydra, um monstro com 8 cabeças de Serpente,    |\n"
		"          |  com cerca de 10 metros.                                  |\n"
		"          |                                                           |\n"
		"          |   Como será o fim deste duelo mortal?                     |\n"
		"          |   Conseguirá o aldeão por fim ao tormento de Hydra e      |\n"
		"          |   achar o tesouro escondido?                              |\n"
		"          |   Guia o Aldeão nesta aventura e traz a paz à Aldeia!     |\n"
		"          |   Boa Sorte!                                              |\n"
		"          |   ________________________________________________________|___\n"
		"          |  /                                                            /\n"
		"          \\_/____________________________________________________________/\n");
	system("pause");
	system("cls");
}

void Colors(){
	printf("Aqui podes costumizar a cor de fundo e da fonte!\n");
	printf("Código de cores:\n"
		"0 = Black\n"
		"1 = Blue\n"
		"2 = Green\n"
		"3 = Aqua\n"
		"4 = Red\n"
		"5 = Purple\n"
		"6 = Yellow\n"
		"7 = White\n"
		"8 = Gray\n"
		"9 = Light Blue\n"
		"A = Light Green\n"
		"B = Light Aqua\n"
		"C = Light Red\n"
		"D = Light Purple\n"
		"E = Light Yellow\n"
		"F = Bright White\n");
	printf("Escolhe um esquema de cores! Ex: '0B' - 0 (cor de fundo) e B(cor da fonte)\n");
	scanf("%s", option);
	strcpy(option_color, color);
	strcat(option_color, option);

	system(option_color);
}

void InitializePlayer(struct Player *pPlayer, bool superU){
	printf("Aldeão qual o teu nome?\n");
	scanf("%s", pPlayer->name);
	pPlayer->cell = PLAYER_INITIAL_CELL;
	pPlayer->treasure = NO_TREASURE;

	if (superU == false)
	{
		pPlayer->energy = MAX_PLAYER_ENERGY;
	}
	else
	{
		pPlayer->energy = MAX_SU_PLAYER_ENERGY;
	}
}

void InitializeMonster(struct Monster *pMonster){
	strcpy(pMonster->name, "Hydra");
	pMonster->energy = MAX_ENERGY;

	pMonster->cell = MONSTER_CELL;
}

void PrintPlayer(struct Player *pPlayer, Monster *pMonster, bool super){
	printf("\n*** Player Info ***\n");
	printf("Nome: %s o Aldeão Destemido\n", pPlayer->name);
	printf("Energia: %d\n", pPlayer->energy);
	printf("Celula: %d\n", pPlayer->cell);
	if (pPlayer->treasure == 0)
		printf("Tesouro: Nao\n\n");
	else
		printf("Tesouro: Sim\n\n");
}

void MonsterFight(struct Player *pPlayer, Monster *pMonster){

	printf("%s: %d Vida\n", pPlayer->name, pPlayer->energy);
	printf("%s: %d Vida\n", pMonster->name, pMonster->energy);

	while (death == false)
	{
		int rand_dmg = rand() % 15 + 1;
		int player_dmg = rand_dmg;
		int tresure_dmg = 30;
		int monster_dmg = rand_dmg;
		int rand_num = rand() % 10 + 1;

		if (pPlayer->treasure == 0)
		{
			if (rand_num % 2 == 0){
				pMonster->energy = pMonster->energy - player_dmg;
				printf("Atingiste a hydra com sucesso por %d dano! A %s tem %d de vida!\n", player_dmg, pMonster->name, pMonster->energy);
			}
			else{
				pPlayer->energy = pPlayer->energy - monster_dmg;
				printf("A malvadez é superior! Recebeste %d de dano. Tens %d de vida!\n", monster_dmg, pPlayer->energy);
			}

			if (pPlayer->energy <= 0){
				printf("A %s conseguiu aniquilar-te ... A aldeia continuará a viver tempos de agonia e medo!\n\n", pMonster->name);
				death = true;
				gameover = true;
			}
			if (pMonster->energy <= 0){
				printf("Muito bem %s! Finalmente conseguiste pôr um fim à malvada %s! És o Maior da tua Aldeia!\n\n", pPlayer->name, pMonster->name);
				death = true;
			}
		}
		else
		{
			if (rand_num % 2 == 0){
				pMonster->energy = pMonster->energy - tresure_dmg;
				printf("Atingiste a hydra com a lendaria Excalibur %d dano! A %s tem %d de vida!\n", tresure_dmg, pMonster->name, pMonster->energy);
			}
			else{
				pPlayer->energy = pPlayer->energy - monster_dmg;
				printf("A malvadez é superior! Recebeste %d de dano. Tens %d de vida!\n", monster_dmg, pPlayer->energy);
			}

			if (pPlayer->energy <= 0){
				printf("A %s conseguiu aniquilar-te ... A aldeia continuará a viver tempos de agonia e medo!\n\n", pMonster->name);
				death = true;
				gameover = true;
			}
			if (pMonster->energy <= 0){
				printf("Muito bem %s! Finalmente conseguiste pôr um fim à malvada %s! És o Maior da tua Aldeia!\n\n", pPlayer->name, pMonster->name);
				death = true;
			}
		}
		system("pause");
	}
}

void InitializeMap(struct Cell cell[]){

	//Number of cell
	//pMap->ncell = 19;

	//Cell 0
	cell[0].north = 1;
	cell[0].south = -1;
	cell[0].west = -1;
	cell[0].east = -1;
	cell[0].treasure = 0;
	strcpy(cell[0].description, CELL_0_DESCRIPTION);

	//Cell 1
	cell[1].north = -1;
	cell[1].south = 0;
	cell[1].west = 2;
	cell[1].east = 10;
	cell[1].treasure = 0;
	strcpy(cell[1].description, "Avistas uma placa com a indicação:\n(W - Santuario) (E - PERIGO!)\n");

	//Cell 2
	cell[2].north = -1;
	cell[2].south = -1;
	cell[2].west = 3;
	cell[2].east = 1;
	cell[2].treasure = 0;
	strcpy(cell[2].description, "Encontras uma estrada entre os arbustos\n(W - Continuar) (E - Voltar)\n");

	//Cell 3
	cell[3].north = 4;
	cell[3].south = -1;
	cell[3].west = -1;
	cell[3].east = 2;
	cell[3].treasure = 0;
	strcpy(cell[3].description, "A estrada continua\n(N - Continuar) (E - Voltar)\n");

	//Cell 4
	cell[4].north = 5;
	cell[4].south = 3;
	cell[4].west = -1;
	cell[4].east = -1;
	cell[4].treasure = 0;
	strcpy(cell[4].description, "... E continua ...\n(N - Continuar) (S - Voltar)\n");

	//Cell 5
	cell[5].north = 6;
	cell[5].south = 4;
	cell[5].west = -1;
	cell[5].east = 7;
	cell[5].treasure = 0;
	strcpy(cell[5].description, "Encontraste uma placa que indica:\n(N - Santuario) (E - KEEP OUT!) (S - Voltar)\n");

	//Cell 6
	cell[6].north = -1;
	cell[6].south = 5;
	cell[6].west = -1;
	cell[6].east = -1;
	cell[6].treasure = 0;
	strcpy(cell[6].description, "Encontraste um beco sem saida, parece que alguem trocou as placas ...\n(S - Voltar)\n");

	//Cell 7
	cell[7].north = -1;
	cell[7].south = -1;
	cell[7].west = 5;
	cell[7].east = 8;
	cell[7].treasure = 0;
	strcpy(cell[7].description, "Parece que estás no caminho certo de novo\n(W - Voltar ) (E - Continuar)\n");

	//Cell 8
	cell[8].north = 9;
	cell[8].south = -1;
	cell[8].west = 7;
	cell[8].east = -1;
	cell[8].treasure = 0;
	strcpy(cell[8].description, "Encontraste o Santuario\n(N - Entrar ) (W - Voltar)\n");

	//Cell 9
	cell[9].north = -1;
	cell[9].south = 8;
	cell[9].west = -1;
	cell[9].east = -1;
	cell[9].treasure = 1;
	strcpy(cell[9].description, "Dentro do Santuario avistas uma espada cravada numa rocha\n('loot' - Apanhar o tesouro) (S - Sair do Santuario)\n");

	//Cell 10
	cell[10].north = -1;
	cell[10].south = -1;
	cell[10].west = 1;
	cell[10].east = 11;
	cell[10].treasure = 0;
	strcpy(cell[10].description, "Esta zona da floresta está coberta de nevoeiro\n(W - Voltar) (E - Continuar)\n");

	//Cell 11
	cell[11].north = 12;
	cell[11].south = -1;
	cell[11].west = 10;
	cell[11].east = -1;
	cell[11].treasure = 0;
	strcpy(cell[11].description, "Sentes uma brisa ...\n(W - Voltar) (N - Continuar)\n");

	//Cell 12
	cell[12].north = -1;
	cell[12].south = 11;
	cell[12].west = -1;
	cell[12].east = 13;
	cell[12].treasure = 0;
	strcpy(cell[12].description, "Avistas um urso ensanguentado\n(S - Voltar) (E - Continuar)\n");

	//Cell 13
	cell[13].north = 14;
	cell[13].south = -1;
	cell[13].west = 12;
	cell[13].east = -1;
	cell[13].treasure = 0;
	strcpy(cell[13].description, "Os restos mortais dos animais continuam ...\n(W - Voltar) (N - Continuar)\n");

	//Cell 14
	cell[14].north = 15;
	cell[14].south = 13;
	cell[14].west = -1;
	cell[14].east = -1;
	cell[14].treasure = 0;
	strcpy(cell[14].description, "O caminho está coberto de pegadas gigantes\n(N - Continuar) (S - Voltar)\n");

	//Cell 15
	cell[15].north = -1;
	cell[15].south = 14;
	cell[15].west = 16;
	cell[15].east = -1;
	cell[15].treasure = 0;
	strcpy(cell[15].description, "O caminho está destruido, cheio de árvores caidas\n(W - Continuar) (S - Voltar)\n");

	//Cell 16
	cell[16].north = -1;
	cell[16].south = -1;
	cell[16].west = -1;
	cell[16].east = 15;
	cell[16].treasure = 0;
	strcpy(cell[16].description, "O nevoeiro fica cada vez mais cerrado ...\n(N - Continuar) (E - Voltar)\n");

	PrintMap(cell);
	//ReadMap(cell);
	SaveBin(cell);
	//ReadBin(cell);
}

void PrintMap(struct Cell cell[]){
	FILE *mapTxt;

	mapTxt = fopen("map.txt", "w");

	for (int i = 0; i < MAX_CELL; i++)
	{
		fprintf(mapTxt, "%d\n", cell[i].north);
		fprintf(mapTxt, "%d\n", cell[i].south);
		fprintf(mapTxt, "%d\n", cell[i].east);
		fprintf(mapTxt, "%d\n", cell[i].west);
		fprintf(mapTxt, "%d\n", cell[i].treasure);
		fprintf(mapTxt, "%s\n", cell[i].description);
	}
	fclose(mapTxt);
}

void ReadMap(struct Cell cell[]){

	FILE *mapTxt;
	char linha[MAX_LINE];

	mapTxt = fopen("map.txt", "r");

	if (mapTxt == NULL)
	{
		printf("O ficheiro não se encontra na directoria\n");
	}
	else
	{
		for (int i = 0; i < MAX_CELL; i++)
		{
			fgets(linha, MAX_LINE, mapTxt);
			cell[i].north = atoi(linha);
			fgets(linha, MAX_LINE, mapTxt);
			cell[i].south = atoi(linha);
			fgets(linha, MAX_LINE, mapTxt);
			cell[i].east = atoi(linha);
			fgets(linha, MAX_LINE, mapTxt);
			cell[i].west = atoi(linha);
			fgets(linha, MAX_LINE, mapTxt);
			cell[i].treasure = atoi(linha);
			fgets(linha, MAX_LINE, mapTxt);
			strcpy(cell[i].description, linha);
		}
		fclose(mapTxt);
	}
}

void SaveBin(struct Cell cell[])
{
	FILE *bin;

	bin = fopen("BinMap.bin", "w");

	fwrite(cell, sizeof(struct Cell), MAX_CELL, bin);
	fclose(bin);
}

void ReadBin(struct Cell cell[])
{
	FILE *bin;

	bin = fopen("BinMap.bin", "r");

	if (bin == NULL)
	{
		printf("O ficheiro não se encontra na directoria\n");
	}
	else
	{
		fread(cell, sizeof(struct Cell), MAX_CELL, bin);
		fclose(bin);

		srand(time(NULL));

		cell[TRESURE_CELL].treasure = 1;
	}
}

void ActionCommand(Player *pPlayer, Monster *pMonster, Cell cell[], bool super){

	if (pPlayer->energy == MAX_PLAYER_ENERGY || pPlayer->energy == MAX_SU_PLAYER_ENERGY){
		printf("%s\n", cell[pPlayer->cell].description);
	}

	if (cell[pPlayer->cell].treasure == 1 && pPlayer->treasure == 0){
		printf("           `        (O)       ~\n"
			"                     H      '\n"
			"     ~               H\n"
			"       `        ____hHh____\n"
			"         ~      `---------'    ~\n"
			"           `       | | |     '\n"
			"                   | | |\n"
			"                   | | |\n"
			"                   | | |\n"
			"                   | | |\n"
			"                   | | |\n"
			"                   | | |\n"
			"                   | | |\n"
			"             _____;~~~~~:____\n"
			"          __'                \\\n"
			"         /         \\          |\n"
			"        |    _\\\\_   |         |\\\n"
			"        |     \\\\    |         | |      ___\n"
			" __    /     __     |         | |    _/   \\\n"
			"/__\\  |_____/__\\____|_________|__\\  /__\\___\\\n");
		if (strcmp(pickUp_cmd, prompt) == 0){
			pPlayer->treasure = 1;
			printf("Apanha-te a Lendaria Excalibur\n");
		}
	}
	if (super == true){
		if (pPlayer->treasure == 0 && pMonster->energy == MAX_ENERGY)
			printf("O boss está na cell %d\n\n", monster.cell);

		if (pPlayer->treasure == 1 && pMonster->energy > 0)
			printf("O boss está na cell %d\n\n", monster.cell);

		if (pPlayer->energy > 0 && pPlayer->energy < 1000)
			printf("Saíste vitorioso! Voltemos ao inicio. Estás na cell %d.\n", pPlayer->cell);

		if (pPlayer->energy <= 0)
			printf("Acabou-se... GAME OVER! Insere 'end'\n");

		if (pPlayer->treasure == 0)
			printf("Estás na cell %d. Para onde queres ir?\n", pPlayer->cell);

		if (pPlayer->treasure == 1 && pPlayer->energy == MAX_SU_PLAYER_ENERGY)
			printf("Estás na cell %d. Para onde queres ir?\n", pPlayer->cell);
	}
	else{

		if (pPlayer->energy > 0 && pPlayer->energy < 100)
			printf("Saíste vitorioso! Voltemos ao inicio. Estás na cell %d.\n", pPlayer->cell);

		if (pPlayer->energy <= 0)
			printf("Acabou-se... GAME OVER! Insere 'exit'\n");

		if (pPlayer->treasure == 0)
			printf("Estás na cell %d. Para onde queres ir?\n", pPlayer->cell);

		if (pPlayer->treasure == 1 && pPlayer->energy == MAX_PLAYER_ENERGY)
			printf("Estás na cell %d. Para onde queres ir?\n", pPlayer->cell);
	}

	scanf("%s", prompt);

	if (strcmp("save", prompt) == 0){
		SaveGame(pPlayer, pMonster, cell);
	}

	if (strcmp(exit_cmd, prompt) == 0){
		gameover = true;
	}
}

void ActionPlayer(Cell cell[], Player *pPlayer){

	if (strcmp(north, prompt) == 0){
		if (cell[pPlayer->cell].north != -1){
			pPlayer->cell = cell[pPlayer->cell].north;
		}
		else{
			pPlayer->cell = pPlayer->cell;
			printf("Caminho bloqueado! Escolhe outra direcção...\n\n");
		}
	}
	if (strcmp(south, prompt) == 0){
		if (cell[pPlayer->cell].south != -1){
			pPlayer->cell = cell[pPlayer->cell].south;
		}
		else{
			pPlayer->cell = pPlayer->cell;
			printf("Caminho bloqueado! Escolhe outra direcção...\n\n");
		}
	}
	if (strcmp(east, prompt) == 0){
		if (cell[pPlayer->cell].east != -1){
			pPlayer->cell = cell[pPlayer->cell].east;
		}
		else{
			pPlayer->cell = pPlayer->cell;
			printf("Caminho bloqueado! Escolhe outra direcção...\n\n");
		}
	}
	if (strcmp(west, prompt) == 0){
		if (cell[pPlayer->cell].west != -1){
			pPlayer->cell = cell[pPlayer->cell].west;
		}
		else{
			pPlayer->cell = pPlayer->cell;
			printf("Caminho bloqueado! Escolhe outra direcção...\n\n");
		}
	}
}

void ActionMonster(Cell cell[], Monster *pMonster, bool super){
	int random = rand() % 4 + 1;

	if (random == 1){
		if (cell[pMonster->cell].north != -1){
			pMonster->cell = cell[pMonster->cell].north;
		}
		else{
			pMonster->cell = pMonster->cell;
		}
	}
	if (random == 2){
		if (cell[pMonster->cell].south != -1){
			pMonster->cell = cell[pMonster->cell].south;
		}
		else{
			pMonster->cell = pMonster->cell;
		}
	}
	if (random == 3){
		if (cell[pMonster->cell].west != -1){
			pMonster->cell = cell[pMonster->cell].west;
		}
		else{
			pMonster->cell = pMonster->cell;
		}
	}
	if (random == 4){
		if (cell[pMonster->cell].east != -1){
			pMonster->cell = cell[pMonster->cell].east;
		}
		else{
			pMonster->cell = pMonster->cell;
		}
	}
}

void SaveGame(Player *pPlayer, Monster *pMonster, Cell cell[]){
	FILE *savegame;
	savegame = fopen("save.txt", "w");

	fprintf(savegame, "%s\n", pPlayer->name);
	fprintf(savegame, "%d\n", pPlayer->cell);
	fprintf(savegame, "%d\n", pPlayer->energy);
	fprintf(savegame, "%d\n", pPlayer->treasure);
	fprintf(savegame, "\n");
	fprintf(savegame, "%s\n", pMonster->name);
	fprintf(savegame, "%d\n", pMonster->energy);
	fprintf(savegame, "\n");
	fprintf(savegame, "Excalibur\n");

	if (pPlayer->treasure == 0)
	{
		for (int i = 0; i < MAX_CELL; i++)
		{
			if (cell[i].treasure == 1)
			{
				fprintf(savegame, "%d", i);
			}
		}
	}
	fclose(savegame);
	printf("Jogo salvo com sucesso!\n");
}

void LoadGame(Player *pPlayer, Monster *pMonster, Cell cell[]){
	FILE *loadGame;
	char data[MAX_LINE];

	loadGame = fopen("save.txt", "r");

	fgets(data, MAX_LINE, loadGame);
	data[strcspn(data, "\n")] = '\0';
	strcpy(pPlayer->name, data);

	fgets(data, MAX_LINE, loadGame);
	pPlayer->cell = atoi(data);

	fgets(data, MAX_LINE, loadGame);
	pPlayer->energy = atoi(data);

	fgets(data, MAX_LINE, loadGame);
	pPlayer->treasure = atoi(data);

	fgets(data, MAX_LINE, loadGame);
	pMonster->cell = atoi(data);

	fgets(data, MAX_LINE, loadGame);
	for (int i = 0; i < MAX_CELL; i++)
	{
		cell[i].treasure = 0;
	}

	if (pPlayer->treasure == 0)
	{
		cell[atoi(data)].treasure = 1;
	}
	printf("Voltaste %s! Estavas pela última vez na sala %d. Vamos para a vitória!\n", pPlayer->name, pPlayer->cell);
	system("pause");
}

void Options()
{
	printf("Novo jogo - 1\n");
	printf("Carregar jogo - 2\n");
	printf("Opções - 3\n");
	scanf("%s", prompt);
}