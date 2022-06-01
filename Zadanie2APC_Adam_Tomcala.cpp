#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using std::cout;

void entery(vector<int64_t> &enter, char data[], streamsize velkost, int64_t pozicia, string slovo) {		// funckia sluzi na 
	for (int i = 0; i < velkost; i++) {																		// zistenie pozicii '\n'
		if (data[i] == '\n') {																				// v subore
			if (pozicia == 0) {
				enter.push_back(i);
			}
			else
			{
				enter.push_back(pozicia + i + (slovo.length() - 1));
			}
		}
	}
}

void zapisPozicie(string retazec, string slovo, vector<int64_t> &pozicie, int64_t celkovaPozicia) {			// funkcia sluzi na 
	size_t poziciaPrveho = 0;																				// zapis zaciatku
	while (true) {																							// hladaneho retazca 
		poziciaPrveho = retazec.find(slovo, poziciaPrveho);													// v subore
		if (poziciaPrveho == string::npos) {
			poziciaPrveho = 0;
			break;
		}
		pozicie.push_back(poziciaPrveho + celkovaPozicia);
		poziciaPrveho++;
	}
}

int main(int argc, char* argv[])
{	
	if (argc != 4) {												//Pocet argumentov je rozny od 3.
		return 1;
	}
	string cesta = argv[1];
	fstream inputFile;
	inputFile.open(cesta, ios::in);

	if (!inputFile.is_open()) {										//Subor sa nepodaril otvorit.
		cout << "Chyba pri otvarani suboru." << endl;
		return 1;
	}

	string slovo = argv[2];
	if (slovo.length() > 255) {										//Presiahnuta dlzka hladaneho slova.
		cout << "Presiahnuta dlzka slova." << endl;
		return 1;
	}
	string vzd = argv[3];
	for (char znak : vzd){											//Kontrola argumentu vstup.
		if (!isdigit(znak)) {
			cout << "Chyba pri vstupe (vzdialenost)." << endl;
			return 1;
		}
	}
	int32_t vzdialenost = stoi(vzd);
	if (vzdialenost == 0) {											//Ak je vzdialenost 0, chyba.
		cout << "Nespravna vzdialenost." << endl;
		return 1;
	}

	int64_t pocetZnakov = 1024;							// prehladavam po 1024 znakoch
	char data[1024];
	string retazec = "";
	string podretazec = "";
	vector<int64_t> celkovePozicie;						// vektor, kde ukladam pozicie najdenych slov
	vector<int64_t> relativnePozicie;					// vektor, kde ukladam pozicie slov vzhladom na riadky
	vector<int64_t> riadky;								// vektor, kde ukladam riadky najdenych slov
	vector<int64_t> pozicieEnterov;						// vektor, kde ukladam pozicie '\n'
	int64_t celkovaPozicia = 0;							// premenna, ktora sluzi na pohyb v subore
	int64_t pocetIteracii = 1;

	while (inputFile.read(data, pocetZnakov)) {			// citam po 1024 znakoch so suboru
		retazec = podretazec;							
		retazec += data;
		retazec.resize(inputFile.gcount() + podretazec.length());					//nasatavenie dlzky stringu na pozadovanu dlzku
		podretazec.clear();												
		entery(pozicieEnterov, data, inputFile.gcount(), celkovaPozicia, slovo);	// funkcia na najdenie pozicii '\n'
		zapisPozicie(retazec, slovo, celkovePozicie, celkovaPozicia);				// funckia na najdenie pozicii hladanych slov
		celkovaPozicia = pocetZnakov * pocetIteracii;								// aktualizovanie celkovej pozicie v subore
		podretazec.append(retazec.end() - slovo.length() + 1, retazec.end());		// podretazec sluzi na najdenie slov v "odseknutej casti"
		celkovaPozicia -= slovo.length() - 1;
		pocetIteracii++;
	}
																					// ak sa uz nenacita pozadovane mnozstvo znakov (1024)
	retazec = podretazec;															// prehladavam este tie zvysne rovankym sposobom
	retazec += data;
	retazec.resize(inputFile.gcount() + podretazec.length());
	entery(pozicieEnterov, data, inputFile.gcount(), celkovaPozicia, slovo);
	zapisPozicie(retazec, slovo, celkovePozicie, celkovaPozicia);


	if (celkovePozicie.size() < 2) {												// ak je pocet najdenzch slov menej ako 2, netreba ani kontrolovat
		return 0;
	}
	vector<bool> vypis;																// pomocny vektor, ktory sluzi na to, abz som vedel ci dany vyskyt splna
																					// vyskyt splna poziadavky	
	
	for (size_t i = 0; i < celkovePozicie.size(); i++) {							// cyklus prechadza vsetky vyskyty slov a preratava celkovu zaciatocnu				
		if (pozicieEnterov.size() != 0) {											// poziciu slova na poziciu, ktora bude obsahovat cislo riadku a stlpec v nom
			size_t j = 0;
			for (j = 0; j < pozicieEnterov.size(); j++) {
				if (pozicieEnterov[j] > celkovePozicie[i]) {
					break;
				}
			}
			riadky.push_back(j);													// riadok je v podstate, pocet '\n', ktore som napocital
			if (j == 0) {															// kym som dosiel k aktualnemu slovu
				relativnePozicie.push_back(celkovePozicie[i]);
			}
			else {
				relativnePozicie.push_back(celkovePozicie[i] - pozicieEnterov[j - 1] - 1);		// vypocitanie pozicii v riadku na zaklade polohy slova a polohy '\n'
			}
		}
		else {
			relativnePozicie.push_back(celkovePozicie[i]);
			riadky.push_back(0);
		}
		if (i == 0) {																// overovanie vzdialenosti (ci sa dane slovo vypise alebo nie)
			if (celkovePozicie[i + 1] - celkovePozicie[i] > vzdialenost) {
				vypis.push_back(false);
			}
			else {
				vypis.push_back(true);
			}
			continue;
		}
		if (i == celkovePozicie.size() - 1) {
			if (celkovePozicie[i] - celkovePozicie[i - 1] > vzdialenost) {
				vypis.push_back(false);
			}
			else {
				vypis.push_back(true);
			}
			continue;
		}
		if (celkovePozicie[i] - celkovePozicie[i - 1] > vzdialenost && celkovePozicie[i + 1] - celkovePozicie[i] > vzdialenost) {
			vypis.push_back(false);
		}
		else {
			vypis.push_back(true);
		}
	}

	for (size_t i = 0; i < relativnePozicie.size(); i++) {					// samotny vypis slov, ktore splnaju podmienky
		if (vypis[i]) {
			cout << riadky[i] << " " << relativnePozicie[i] << endl;
		}
	}
	return 0;
}