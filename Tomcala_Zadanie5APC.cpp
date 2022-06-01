#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

vector<string> kompres(ifstream& vstSubor) {
	string riadok = "";
	int cisloRiadku = 1;
	bool novyZnak;
	char znak = ' ';
	int pocet;
	vector<string> vysledok;
	while (getline(vstSubor, riadok)) {
		novyZnak = true;
		vysledok.push_back("");
		pocet = 0;
		for (size_t i = 0; i < riadok.size(); i++) {
			if (!isalpha(riadok[i])) {
				cout << "Error on line " << cisloRiadku << endl;
				vstSubor.close();
				exit(1);
			}

			if (novyZnak) {
				pocet = 0;
				znak = riadok[i];
				novyZnak = false;
				++pocet;
				if (i == riadok.size() - 1) {
					vysledok[cisloRiadku - 1].push_back(znak);
					vysledok[cisloRiadku - 1].append(to_string(pocet));
				}
				continue;
			}
			else {
				if (znak == riadok[i]) {
					++pocet;
					if (i == riadok.size() - 1) {
						vysledok[cisloRiadku - 1].push_back(znak);
						vysledok[cisloRiadku - 1].append(to_string(pocet));
					}
				}
				else {
					--i;
					novyZnak = true;
					vysledok[cisloRiadku - 1].push_back(znak);
					vysledok[cisloRiadku - 1].append(to_string(pocet));
				}
			}
		}
		cisloRiadku++;
	}
	return vysledok;
}


vector<string> dekompresia(ifstream& vstSubor) {
	string riadok = "";
	int cisloRiadku = 1;
	char znak = ' ';
	vector<string> vysledok;
	while (getline(vstSubor, riadok)) {
		vysledok.push_back("");
		bool hladajZnak = true;
		cout << riadok << endl;
		for (size_t i = 0; i < riadok.size(); i++) {
			if (isalpha(riadok[i]) && hladajZnak) {
				znak = riadok[i];
				hladajZnak = false;
			}
			else if (isdigit(riadok[i]) && !hladajZnak) {
				string cislo = "";
				while (isdigit(riadok[i])) {
					cislo.push_back(riadok[i]);
					++i;
				}
				hladajZnak = true;
				int pocetZnakov = stoi(cislo);
				--i;

				for (int j = 0; j < pocetZnakov; j++) {
					vysledok[cisloRiadku - 1].push_back(znak);
				}
			}
			else {
				cout << "Error on line " << cisloRiadku << endl;
				vstSubor.close();
				exit(1);
			}
		}
		cisloRiadku++;
	}
	return vysledok;
}


int main(int argc, char* argv[])
{
	if (argc != 4) {
		cout << "Nespravny pocet argumentov." << endl;
		return 1;
	}

	string stav = argv[1];

	if (stav.compare("compress") != 0 && stav.compare("decompress") != 0) {
		cout << "Nespravny prvy argument." << endl;
		return 1;
	}

	string menoVstSuboru = argv[2];
	string menoVystSuboru = argv[3];

	ifstream vstupnySubor;
	vstupnySubor.open(menoVstSuboru);

	if (vstupnySubor) {
		vector<string> vysledok;
		if (stav.compare("compress") == 0) {
			vysledok = kompres(vstupnySubor);
		}
		else {
			vysledok = dekompresia(vstupnySubor);
		}
		vstupnySubor.close();

		ofstream vystupnySubor;
		vystupnySubor.open(menoVystSuboru);
		if (vystupnySubor) {
			for (size_t i = 0; i < vysledok.size(); i++) {
				if (i != vysledok.size() - 1) {
					vystupnySubor << vysledok[i] << endl;
				}
				else {
					vystupnySubor << vysledok[i];
				}
			}
			vystupnySubor.close();
		}
		else {
			cout << "Problem s vystupnym suborom." << endl;
			vystupnySubor.close();
			return 1;
		}
	}

	else {
		cout << "Chyba pri otvarani vstupneho suboru." << endl;
		return 1;
	}

	return 0;
}
