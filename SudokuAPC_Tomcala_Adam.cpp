
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

bool overCislo(std::vector<std::vector<char>> sudoku, int riadok, int stlpec, char num) {	//funkcia, ktora overuje ci dane cislo mozeme zapisat do sudoku
	for (int i = 0; i < 9; i++) {													//overenie riadku
		if (num == sudoku[riadok][i]) {
			return false;
		}
	}
	for (int i = 0; i < 9; i++) {													//overenie stlpca
		if (num == sudoku[i][stlpec]) {
			return false;
		}
	}
	int i = riadok / 3;
	int j = stlpec / 3;
	int indexX, indexY;
	for (int x = 0; x < 3; x++) {													//overenie Boxu
		for (int y = 0; y <  3; y++) {
			indexX = i * 3 + x;
			indexY = j * 3 + y;
			if (num == sudoku[indexX][indexY]) {
				return false;
			}
		}
	}																				//ak dane cislo nevyhovuje pravidlam, vracia false
	return true;																	//ak vyhovuje, vracia true
}

bool riesSudoku(std::vector<std::vector<char>> &sudoku) {							//rekurzivna funkcia na riesenie sudoku (backtracking)
	bool jePrazdne = false;
	int x = 0, y = 0;

	for (x = 0 ; x < 9; x++) {														//cyklusy pre najdenie najblizsieho nevyplneneho policka
		for (y = 0; y < 9; y++) {
			if (sudoku[x][y] == '.' || sudoku[x][y] == '0') {
				jePrazdne = true;
				break;
			}
		}
		if (jePrazdne) {
			break;
		}
	}

	if (!jePrazdne) {																//ak uz neostava policko, ktore je prazdne
		return true;																//sudoku sa podarilo vyriesit
	}
	if (jePrazdne) {																//ak je nejake policko prazdne, pokracovanie v rieseni
		for (int i = 1; i < 10; i++) {
			char num = char(i) + '0';
			if (overCislo(sudoku, x, y, num)) {										//zistovanie ci dane cislo mozeme vlozit do sudoku
				sudoku[x][y] = char(i) + '0';										//bez toho, aby boli porusene pravidla
				if (riesSudoku(sudoku)) {											//vnaranie sa 
					return true;
				}
				sudoku[x][y] = '0';													//ak dane cislo nie je spravne, vrati sa policko spat na '0'
			}
		}
	}
	return false;
}

std::vector<std::vector<char>> zapisSudoku(std::string line) {						//Funkcia, ktora prepise vstup do 2D vektora
	std::vector<std::vector<char>> sudoku (9, std::vector<char>(9, ' '));;
	int index;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			index = i * 9 + j;
			sudoku[i][j] = line[index];
		}
	}
	return sudoku;
}

bool kontrolaSudoku(std::vector<std::vector<char>> sudoku) {						//funkcia, ktora kontroluje zadane sudoku
	for (int i = 0; i < 9; i++) {													//kontroluje sa kazde policku sudoku
		for (int j = 0; j < 9; j++) {
			for (int k = 0; k < 9; k++) {											//kontrola ci sa cislo nachadza 1krat
				if (sudoku[i][j] == sudoku[i][k] && k != j) {						//v riadku
					return false;													//ak ano sudoku je chybne
				}
			}
			for (int k = 0; k < 9; k++) {											//kontrola ci sa cislo nachadza 1krat
				if (sudoku[i][j] == sudoku[k][j] && k != i) {						//v stlpci
					return false;													//ak ano chyba
				}
			}
			int sirka = i / 3;
			int vyska = j / 3;
			int indexX, indexY;
			for (int x = 0; x < 3; x++) {											//kontrola ci sa cislo nachadza 1krat 
				for (int y = 0; y < 3; y++) {										//v boxe 3x3
					indexX = sirka * 3 + x;
					indexY = vyska * 3 + y;
					if (sudoku[i][j] == sudoku[indexX][indexY] && indexX != i && indexY != j) {
						return false;												//ak ano chyba
					}
				}
			}
		}
	}
	return true;																	//sudoku je vyriesene spravne
}

bool kontrolaVstupu(std::string line) {												//Funkcia, ktora overuje spravnost vstupu
	if (line.length() != 81) {														//Ak nie je dlzka 81, error
		std::cout << "Nespravny dlzka sudoku." << std::endl;		
		return false;															
	}
	for (int i = 0; i < 81; i++) {
		if (!isdigit(line[i]) && line[i] != '.') {									//Ak obsahuje subor nieco ine ako cislice alebo '.', error
			std::cout << "Nespravny format sudoku." << std::endl;
			return false;
		}
	}
	return true;																	//Inak, spravny vstup
}

std::string sudokuNaString(std::vector<std::vector<char>> sudoku) {					//funkcia, ktora "prepisuje" vector na string
	std::string vysledok = "";
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			vysledok += sudoku[i][j];
		}
	}
	return vysledok;
}

void zapisVysledok(std::fstream &oFile, std::string sudoku, bool isOutputFile) {	//funkcia, ktora sluzi na vypis vysledku
	if (isOutputFile) {
		oFile << sudoku << std::endl;
	}
	else {
		std::cout << sudoku << std::endl;
	}
}

void nacitajSudoku(std::string iFileName, std::string oFileName) {
	bool isOutputFile;
	std::fstream outputFile;
	std::string line;
	std::vector<std::vector<char>> sudoku;
	if (oFileName != "") {															//zistujem ci mam vystupny subor alebo
		outputFile.open(oFileName, std::ios::out);									
		if (!outputFile.is_open()) {
			std::cout << "Chyba pri otvaranie vystupneho suboru.";
			exit(1);
		}
		isOutputFile = true;														//ak ano zapisujem do neho
	}
	else {																			//inak vypisujem na stdout
		isOutputFile = false;
	}

	if (iFileName != "") {															//Bol zadany vstupny subor
		std::fstream inputFile;
		inputFile.open(iFileName, std::ios::in);
		if (!inputFile.is_open()) {
			std::cout << "Chyba pri otvaranie vstupneho suboru.";					//osetrenie ci je subor otvreny a je mozne s nim pracovat
			exit(1);
		}											
		while (std::getline(inputFile, line)) {										//nacitavanie vstupov zo suboru
			if (!kontrolaVstupu(line)) {											//Kontrola vstupu
				std::cout << "Nespravny format vstupu." << std::endl;
				exit(1);
			}

			if (std::count(line.begin(), line.end(), '0') == 0						//ak vstup neobsahuje '0' alebo '.'
				&& std::count(line.begin(), line.end(), '.') == 0) {				//kontrolujem sprvost sudoku
				sudoku = zapisSudoku(line);											//prepis vstupu
				if (kontrolaSudoku(sudoku)) {										//ak sudoku je sprvane vyriesene, vypisem ho
					std::string vysledok = sudokuNaString(sudoku);					//prepis vectora na string
					zapisVysledok(outputFile, vysledok, isOutputFile);				//vypis sudoku (stdout alebo file)
				}
				else {
					zapisVysledok(outputFile, "", isOutputFile);					//vypisanie prazdneho riadku
				}
			}
			else {																	//riesenie sudoku
				sudoku = zapisSudoku(line);
				if (riesSudoku(sudoku)) {											//ak sa podari vyriesit sudoku
					std::string vysledok = sudokuNaString(sudoku);					//prepis vectora na string
					zapisVysledok(outputFile, vysledok, isOutputFile);				//vypis sudoku (stdout alebo file)
				}
				else {																//ak sudoku riesenie nema, vypise sa prazdny riadok
					zapisVysledok(outputFile, "", isOutputFile);
				}
			}
		}
	}
	else {																			//nebol zadany vstupny subor
		while (std::cin >> line) {													//nacitavanie vstupu zo stdin
			if (!kontrolaVstupu(line)) {											//kontrola vstupu									
				exit(1);
			}
			if (std::count(line.begin(), line.end(), '0') == 0						//ak vstup neobsahuje '0' alebo '.'
				&& std::count(line.begin(), line.end(), '.') == 0) {				//kontrolujem sprvost sudoku
				sudoku = zapisSudoku(line);											//prepis vstupu
				if (kontrolaSudoku(sudoku)) {										//ak sudoku je sprvane vyriesene, vypisem ho
					std::string vysledok = sudokuNaString(sudoku);					//prepis vectora na string
					zapisVysledok(outputFile, vysledok, isOutputFile);				//vypis sudoku (stdout alebo file)
				}
				else {
					zapisVysledok(outputFile, "", isOutputFile);					//vypisanie prazdneho riadku
				}
			}
			else {																	//riesenie sudoku
				sudoku = zapisSudoku(line);
				if (riesSudoku(sudoku)) {											//ak sa podari vyriesit sudoku
					std::string vysledok = sudokuNaString(sudoku);					//prepis vectora na string
					zapisVysledok(outputFile, vysledok, isOutputFile);				//vypis sudoku (stdout alebo file)
				}
				else {																//ak sudoku riesenie nema, vypise sa prazdny riadok
					zapisVysledok(outputFile, "", isOutputFile);
				}
			}
		}
	}
}


int main(int argc, char* argv[])
{	
	if (argc != 1 && argc != 3 && argc != 5) {										//Osetrenie nespravneho poctu argumentov
		std::cout << "Nespravny pocet argumentov.";									//Akceptuje sa iba 1, 3 alebo 5
		std::cout << argv[0] << std::endl;
		return 1;
	}

	std::string inputFileName = "";
	std::string outputFileName = "";

	if (argc == 3) {																//ak su iba 3 argumenty, overim ci druhy z nich je "-i" alebo "-o"
		std::string typeOfFile = argv[1];
		if (typeOfFile.compare("-i") == 0) {										//ak to je "-i" citam zo suboru a vypisujem na stdout
			inputFileName = argv[2];
		}
		else if (typeOfFile.compare("-o") == 0) {									//ak to je "-o" citam zo stdin a nacitavam zo suboru
			outputFileName = argv[2];
		}
		else {																		//inak error
			return 1;
		}
	}
	else if (argc == 5) {															//ak mam 5 argumentov, overim ci je prvy "-i" a druhy "-o" alebo naopak	
		std::string typeOfFile1 = argv[1];
		std::string typeOfFile2 = argv[3];
		if (typeOfFile1.compare("-i") == 0 && typeOfFile2.compare("-o") == 0) {		//najskor "-i" potom "-o"
			inputFileName = argv[2];
			outputFileName = argv[4];
		}
		else if (typeOfFile1.compare("-o") == 0 && typeOfFile2.compare("-i") == 0) {//najskor "-o" potom "-i"
			inputFileName = argv[4];
			outputFileName = argv[2];
		}
		else {																		//error
			return 1;
		}
	}

	nacitajSudoku(inputFileName, outputFileName);
	return 0;
}