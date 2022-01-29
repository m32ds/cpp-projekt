#include <ctime>
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>

struct field { //pojedyncza kratka, ustawiamy na 0 i ustawiamy, żeby nie było widzialne, potem się zajmiemy zmianami tych wartości
	int value = 0;
	bool visible = false;
};
field board[10][10]; //generujemy pustą tablicę dwuwymiarową pełną pustych pól
int posX = 0, posY = 0, posXBefore = 1, posYBefore = 1;
int endGame = 0;

void placeMines() //funkcja, która losuje i stawia miny
{
	time_t t; //czas w sekundach do seeda do liczby pseudolosowej
	int x, y;
	int minesCount = 10;

	srand((unsigned)time(&t)); //punkt startowy generatora pseudolosowego

	while (minesCount > 0) //dodajemy miny do planszy, tak długo, aż nie będzie żadnej do rozdania
	{
		x = rand() % 10; //losowanie współrzędnej x
		y = rand() % 10; //losowanie współrzędnej y

		if (board[x][y].value != 9) {
			board[x][y].value = 9; //ustawiamy mine

			for (int k = -1; k < 2; k++)
				for (int l = -1; l < 2; l++)
				{
					if ((x + l) < 0 || (y + k) < 0) {
						continue; //wychodzimy, bo tutaj jest krawędź z dołu lub lewej
					}
					if ((x + l) > 9 || (y + k) > 9) {
						continue; //wychodzimy, bo tutaj jest krawędź z góry lub prawej
					}

					if (board[x + l][y + k].value == 9) {
						continue; //wychodzimy, bo tu mina jest i nie dodajemy +1
					}
					board[x + l][y + k].value += 1; //dodajemy +1 do pola
				}
			minesCount--;
		}
	}
}

void revealField(int x, int y) //funckja, która odkrywa pola
{
	if (x < 0 || x>9) {
		return; // wyszliśmy poza planszę z dołu lub z lewej strony
	}
	if (y < 0 || y>9) {
		return; // wyszliśmy poza planszę z góry lub z prawej strony
	}
	if (board[x][y].visible == true) {
		return;  // już pole wydoczne, nie ma co zmieniać
	}

	if (board[x][y].value != 9 && board[x][y].visible == false) { //jeżeli pole niewidoczne i nie jest miną to odkryj
		board[x][y].visible = true;
	}

	if (board[x][y].value != 0) {
		return; // wartość > 0 wyjście	
	}

	//wywołanie funkcji dla każdego sąsiada, żeby każde puste pole zostało odkryte
	revealField(x - 1, y - 1);
	revealField(x - 1, y);
	revealField(x - 1, y + 1);
	revealField(x + 1, y - 1);
	revealField(x + 1, y);
	revealField(x + 1, y + 1);
	revealField(x, y - 1);
	revealField(x, y);
	revealField(x, y + 1);
}
void showBoard()
{
	system("cls"); //wyczysc ekran

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (j == posX && i == posY) { //aktualkna pozycja kursora
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x02); //kolorowanie pola
				std::cout << "#";
			}
			else
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07); //powrót do normalnego koloru
				if (board[j][i].visible == true) { // jeśli pole jest widoczne, to wyświetl odpowiedni znak
					if (board[j][i].value == 0) {
						std::cout << "."; //wyswietl kropkę w pustym miejscu
					}
					else {
						std::cout << board[j][i].value; //yświetl wartość od 1 do 8
					}

				}
				if (board[j][i].visible == false) //jeśli pole nie jest widoczne, to wtedy wyświetl #
					std::cout << "#"; //wyswietl #
			}
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07); // powrót do normalnego koloru
		std::cout << std::endl; //po każdym wydrukowanym rzędzie zakończ linię
	}
}


void steering()
{
	if ((GetKeyState(0x0D) & 0x8000)) { //0x0D- wciśnięty enter
		if (board[posX][posY].value == 9) { //odkryto minę, najgorzej :C
			endGame = 2;
		}

		revealField(posX, posY); //odkryj pole
		showBoard(); //wydrukuj planszę na nowo
	}

	if ((GetKeyState(0x27) & 0x8000) && posX < 9) { //0x27- strzałka w prawo
		posX++;
	}
	if ((GetKeyState(0x25) & 0x8000) && posX > 0) { //0x25- strzałka w lewo
		posX--;
	}
	if ((GetKeyState(0x28) & 0x8000) && posY < 9) { //0x28- strzałka w dół
		posY++;
	}
	if ((GetKeyState(0x26) & 0x8000) && posY > 0) { //0x26- strzałka w górę
		posY--;
	}

	if (posYBefore == posY && posXBefore == posX) {
		return; //jeżeli nie ma ruchu wyjdz
	}

	posYBefore = posY; //Zapisujemy stan poprzedniego ruchu
	posXBefore = posX;

	showBoard(); // wyswietl plansze
}





bool checkWinnerWinnerChickenDinner()
{
	int minesCount = 0;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (board[j][i].visible == false) {
				minesCount++;
			}
		}
	}
	if (minesCount == 10) {
		return true;
	}
	return false;
}

int main()
{
	setlocale(LC_ALL, "pl_PL");//poprawne wyświetlanie polskich znaków w konsoli
	placeMines(); //losuj i ustaw miny
	Sleep(200);//bufor w razie w
	while (endGame == 0)//endGame z zerem czyli ani nie przegraliśmy ani nie wygraliśmy- gra trwa dalej
	{
		Sleep(50); //zapobiega nadmiernym aktualizacjom planszy, bo jak ustawimy więcej to wtedy pozycja kursora będzie się za szybko zmieniała i będzie o 2 lub więcej przeskakiwała naraz
		steering();
		if (checkWinnerWinnerChickenDinner() == true) {
			endGame = 1;
		}
	}

	if (endGame == 1) {
		std::cout << "\nCałkiem sprawnie, jest coś w Tobie z Sapera, wszystkie miny nienaruszone, gratulacje";
	}

	if (endGame == 2) {
		std::cout << "\n Sory, ale nie jesteś zacnym Saperem, trafiłeś na minę. Może kiedy indziej";
	}

	system("pause >nul"); //zeby okno od razu się nie zamknęło po zakonczonej grze, każdy kolejny przycisk wyłączy okno
	return 0;
}