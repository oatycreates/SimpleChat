/**
	Totally ninja'd from : http://pastebin.com/cXpPpC4m Don't sue me please.
	(c) Guest Aug 12th 2014
*/

#include "TextBasedGame.h"

#include "windows.h"
#include <iostream>
#include <conio.h>


TextBasedGame::TextBasedGame()
{

}


TextBasedGame::~TextBasedGame()
{
}

using namespace std;

void logoTop();
void foundEnemy();
void drawMap(class Character Hero, class Character Baddie);
void Console();

struct Color{ //change color of text
	void red(){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0C);
	}

	void green(){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0A);
	}

	void yellow(){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0E);

	}

	void white(){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
	}


};

class Character{

	Color color;

public:

	void setAlive(bool k){
		Alive = k;
	}

	bool isAlive(){
		return Alive;
	}

	int gethealth(){
		if (health <= 0){
			Alive = false;
		}

		if (health >= 70){ //change font color depending on the state of health
			color.green();
			return health;
		}
		else if (health < 70 && health >= 40){
			color.yellow();
			return health;
		}
		else{
			color.red();
			return health;
		}

	}

	void setHealth(int hp){ //change health to a new value
		health = hp;
	}

	int getXCoord(){
		return itsXCoord;
	}

	void setXCoord(int x){
		itsXCoord = x;
	}

	int getYCoord(){
		return itsYCoord;
	}

	void setYCoord(int y){
		itsYCoord = y;
	}

private:

	bool Alive;
	int health;
	int itsXCoord;
	int itsYCoord;

};

int TextBasedGame::InitTextBasedGame()
{
	char keystroke;

	bool running = true;
	int move = 0;
	Character Hero;
	Character Baddie;
	Character Wall;
	Color color;

	Wall.setXCoord(2);
	Wall.setYCoord(1);

	Baddie.setXCoord(7);
	Baddie.setYCoord(5);
	Baddie.setAlive(true);

	Hero.setXCoord(0);
	Hero.setYCoord(2);
	Hero.setHealth(100);
	Hero.setAlive(true);

	logoTop();
	Sleep(1500);
	cout << "(c) Bethesda Softworks 2014";
	Sleep(3000);
	cout << endl << endl;     //  INTRO AND SHIT
	system("cls");
	drawMap(Hero, Baddie);

	while (Hero.isAlive()){
		system("cls");
		drawMap(Hero, Baddie);
		cout << "You are at : (" << Hero.getYCoord() << ", " << Hero.getXCoord() << ")" << endl; //coordinates

		cout << "| Health: ";
		cout << Hero.gethealth(); //health bar
		color.white();
		cout << " |";

		cout << " Stamina: 44 | Armor: 15 | Holding: Pip-Boy |" << endl;
		Console();

		keystroke = _getch();
		switch (keystroke){
		case 77:
			Hero.setYCoord(Hero.getYCoord() + 1);
			break;
		case 75:
			Hero.setYCoord(Hero.getYCoord() - 1);
			break;
		case 80:
			Hero.setXCoord(Hero.getXCoord() + 1);
			break;
		case 72:
			Hero.setXCoord(Hero.getXCoord() - 1);
			break;
		case 'a':
			system("cls");
			break;
		}

		if (Baddie.isAlive()){//if the enemy is alive...
			if (Hero.getXCoord() == Baddie.getXCoord() && Hero.getYCoord() == Baddie.getYCoord()){ //when the player and enemy cross paths

				foundEnemy();

				Baddie.setAlive(false);
			}
		}
	}

	system("PAUSE");
	return 0;
}

void logoTop(){

	cout << ".----. .--. .-.   .-.    .--. .-..-..-----.  " << "\n";
	cout << "| .--'| .; || |   | |   | ,. || || |`-. .-'_ " << "\n";
	cout << "| `;  |    || |   | |   | || || || |  | | |_|" << "\n";
	cout << "| |   | || || |__ | |__ | || || || |  | |  _ " << "\n";
	cout << "|_|   |_||_||____||____|`.__.'`.__.'  |_| |_|" << "\n";
	cout << "                                                        " << "\n";
	cout << ".--                 .-             . .       . .    _-~88e  [~~~~d88P " << "\n";
	cout << "|- .-.-.-. .-..-,  -|-.-..-..-.-.  | |.-. . .| |-      888b     d88P  " << "\n";
	cout << "'---'`-`-`-|-'`'-   ' '  `-'' ' '   ` `-`-'-''-'-    __888'    d88P   " << "\n";
	cout << "           '                                           888e   d88P    " << "\n";
	cout << "                                                       888P  d88P     " << "\n";
	cout << "                                                    ~-_88'  d88P       " << "\n";
	cout << "" << "\n";



}

void foundEnemy(){
	system("COLOR C7");
	system("COLOR D7");
	system("COLOR 17");
	system("COLOR C7");
	system("COLOR D7");
	system("COLOR 17");
	system("COLOR C7");
	system("COLOR 07");
}

void drawMap(Character Hero, Character Baddie){
	int map[16][16] = {
		{ 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	};

	map[Hero.getXCoord()][Hero.getYCoord()] = 2;

	if (Baddie.isAlive()){
		map[Baddie.getXCoord()][Baddie.getYCoord()] = 3;
	}
	for (int x = 0; x < 16; x++){
		for (int y = 0; y < 16; y++){

			if (map[x][y] == 4){
				cout << "\\";
			}
			else if (map[x][y] == 3){
				cout << " &";
			}
			else if (map[x][y] == 2){
				cout << " @";
			}
			else if (map[x][y] == 1){
				cout << " #";
			}
			else {
				cout << "  ";
			}


		}
		cout << endl;
	}

}

void Console(){
	Color colors;
	colors.red();
	cout << " ==========================================================" << endl;
	cout << "|   It's really dark in here. The vault walls feel cold,   |" << endl;
	cout << "| and you can hear the faint sound of a Mr Handy nearby... |" << endl;
	cout << "|   Wait... What's that in the middle of the room there?   |" << endl;
	cout << " ==========================================================" << endl;
	colors.white();
}