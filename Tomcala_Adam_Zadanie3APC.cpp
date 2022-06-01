
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

void zistiRange(vector<int>& rozsah, string prikaz, int velkost) {				// Funkcia, ktora overi spravnost rozsahu
	size_t i = 1;
	if (prikaz.size() == 1) {													// Dlzka prikazu je 1 -> prikaz bez range-u.
		return;
	}

	while (isspace(prikaz[i])) {												// Cyklus sluzi na 'skipnutie' whitespace-ov.
		++i;
		if (i == prikaz.size()) {												// Su tam len whitespaces -> prikaz bez range-u.
			return;
		}
	}

	string range = "";
	while (!isspace(prikaz[i])) {												// Cyklus sluzi na zapisanie range-u do stringu.
		range += prikaz[i];
		++i;
		if (i == prikaz.size()) {												// Ak sa dostanem na dlzku prikazu -> idem testovat range.
			break;
		}
	}

	for (; i < prikaz.size(); i++) {											// Cyklus kontroluje, ci sa nachadzaju nejake znaky za range-om.
		if (!isspace(prikaz[i])) {												// Ak ano "Unsupported command"
			cout << "Unsupported command" << endl;
			return;
		}
	}
	// Kontrola samotneho range-u.
	if (count(range.begin(), range.end(), ',') == 0) {							// Zistenie, kolko ',' sa nachadza v retazci.
		for (size_t index = 0; index < range.size(); index++) {					// Ak 0 -> kontrolujem jedno cislo.
			if (!isdigit(range[index])) {										// Ak to nie je cislica -> "Invalid range".
				cout << "Invalid range" << endl;
				return;
			}
		}
		rozsah.push_back(stoi(range));											// Ak to prejde testom -> str -> int -> ukoncenie funkcie.
		return;
	}
	else if (count(range.begin(), range.end(), ',') == 1) {						// Ak sa tam nachadza jedna ','.
		string prveCislo = "";
		string druheCislo = "";
		bool flag = true;
		for (size_t index = 0; index < range.size(); index++) {					// Cyklus prechadza cez retazec.
			if (range[index] == ',') {											// Kontrola moznych situacii:

				if (range.size() == 1) {										// Iba ','. -> range od 1 - velkost suboru.
					rozsah.push_back(1);
					rozsah.push_back(velkost);
					return;
				}
				if (prveCislo.size() == 0) {									// Prve cislo je prazdne -> range od zaciatku - n.
					rozsah.push_back(1);
					flag = false;
					continue;
				}

				if (druheCislo.size() == 0) {									// Prve cislo nie je prazdne -> zapisem ho a idem zapisovat
					rozsah.push_back(stoi(prveCislo));							// do druheho cisla.
					flag = false;
				}

				if (index == range.size() - 1) {								// Druhe cislo nie je -> range od n - velkost suboru.
					rozsah.push_back(velkost);
					return;
				}
				continue;
			}
			if (!isdigit(range[index])) {										// Kontrola ci je znak cislica.
				cout << "Invalid range" << endl;
				rozsah.clear();
				return;
			}
			if (flag) {															// Zapis do prveho cisla.
				prveCislo += range[index];
			}
			else {																// Zapis do druheho cisla.
				druheCislo += range[index];
			}
		}
		rozsah.push_back(stoi(druheCislo));
	}
	else {																		// Viac ako 1 ',' -> chyba.
		cout << "Unsupported command" << endl;
		return;
	}
}

void zistiRiadok(int& riadok, string prikaz) {									// Funkcia overuje spravnost zadaneho riadku.
	if (prikaz.size() == 1) {													// Ak je dlzka prikazu 1 -> je to len prikaz bez riadku.
		riadok = -1;
		return;
	}
	size_t i;
	for (i = 1; i < prikaz.length(); i++) {										// Cyklus sluzi na "preskocenie" whitespaces.
		if (!isspace(prikaz[i])) {												// Ak uz znak nebude space break.
			break;
		}
	}
	int zaciatok = int(i);
	bool flag = false;
	for (; i < prikaz.length(); i++) {											// Cyklus zistuje spravnost riadku.
		flag = true;
		if (isspace(prikaz[i])) {												// Ak tam bude medzera vyskakujem z cyklu a idem kontrolovat dalej.
			break;
		}
		if (!isdigit(prikaz[i])) {												// Ak prikaz bude obsahovat nieco ine ako cislicu -> chyba.
			riadok = -2;														// Vraciam -2 (oznacenie chyby).
			return;
		}
	}
	if (flag) {
		riadok = stoi(prikaz.substr(zaciatok, i - zaciatok));					// Prevod cisla stringu do int.
	}
	else {
		riadok = -1;
		return;
	}

	if (riadok <= 0) {															// Kontrola ci cislo nie je <= 0.				
		riadok = -2;															// Vraciam -2 (oznacenie chyby).
		return;
	}

	for (; i < prikaz.length(); i++) {											// Kontrola toho, co sa nachadza za riadkom.
		if (!isspace(prikaz[i])) {												// Ak tam je nieco ine ako medzera -> chyba.
			riadok = -2;
			return;
		}
	}
}

void vypisSubor(int od, int doo, vector<string> subor) {						// Funkcia, ktora vypise subor po riadkoch.
	for (int i = od; i <= doo; i++) {
		cout << subor[i];
	}
}


void vytvor_prikaz(string& prikaz) {											// Funkcia vytvori prikaz tak, ze ako prikaz obsahuje na zacitaku
	int i = 0;																	// medzery, tak ich odstrani.
	while (isspace(prikaz[i])) {
		prikaz.erase(prikaz.begin());
		if (i == int(prikaz.size())) {
			break;
		}
	}
}

void onelinerAppend(string prikaz, int& riadok, string& append) {				// Funkcia funguje ako oneliner pre append
	size_t i = 1;
	bool flag = true;
	string line = "";
	for (; i < prikaz.size(); i++) {
		if (isspace(prikaz[i])) {												// Ignorovanie whitespace-ov.
			continue;
		}
		if (isdigit(prikaz[i]) && flag) {										// Zaciatok overovania line.
			size_t j = i;
			for (; j < prikaz.size(); j++) {
				if (isdigit(prikaz[j])) {
					line += prikaz[j];
				}
				else if (isspace(prikaz[j])) {
					flag = false;
					i = j - 1;
					break;
				}
				else {
					line.clear();
					--i;
					flag = false;
					break;
				}
			}
		}
		else {
			flag = false;
			append += prikaz[i];
		}
	}
	if (line.size() == 0) {
		riadok = -1;
	}
	else {
		riadok = stoi(line);
	}
}

int main(int argc, char* argv[])
{
	if (argc != 2) {															// Nespravny pocet argumentov
		cout << "Nespravny pocet argumentov." << endl;
		return 1;
	}
	fstream vstupnySubor;
	vstupnySubor.open(argv[1]);
	vector<string> subor;
	if (vstupnySubor.is_open()) {															// Nacitanie suboru do vector<string> po riadkoch
		string riadok;
		string s((std::istreambuf_iterator<char>(vstupnySubor)), std::istreambuf_iterator<char>());
		int temp = 0;
		for (size_t i = 0; i < s.size(); i++) {
			if (s[i] == '\n') {
				subor.push_back(s.substr(temp, i - temp) + '\n');
				temp = int(i + 1);
			}
		}
		if (temp < int(s.size())) {
			subor.push_back(s.substr(temp));
		}
	}
	vstupnySubor.close();

	string prikaz;
	char znak;
	vector<int> rozsah;
	int riadok = 0;
	while (true) {																// Cyklus, ktory iteruje dokym sa program neukonci.
		cout << "* ";
		getline(cin, prikaz);
		vytvor_prikaz(prikaz);


		znak = char(tolower(prikaz[0]));
		if (znak == 'w') {														// Ak sa stlaci 'w' zapise sa vector<string> do suboru
			vstupnySubor.open(argv[1], ios::out | ios::trunc | ios::in);		// Otvorenie suboru a vymazanie aktualneho obsahu.
			vstupnySubor.clear();
			vstupnySubor.seekg(0, ios::beg);
			for (string r : subor) {											// Po riadkoch sa nacita vector<string> do suboru.
				vstupnySubor << r;
			}
			vstupnySubor.close();
		}
		else if (znak == 'a') {													// Ak sa stlaci 'a', appendne sa text na urcite miesto
			zistiRiadok(riadok, prikaz);
			if (riadok == -2) {													// Oneliner
				//cout << "Unsupported command" << endl;
				string vstupneSlovo = "";
				onelinerAppend(prikaz, riadok, vstupneSlovo);

				if (riadok == -1) {												// Ak bude chybny line -> beriem to ako pridanie stringu na koniec file-u.
					subor.push_back(vstupneSlovo + '\n');
				}
				else {
					if (riadok > (int)subor.size()) {							// Vytvorene neexistujucich riadkov a dopisanie stringu.
						for (int i = 0; i < riadok - (int)subor.size(); i++) {
							subor.push_back("\n");
						}
						subor.push_back(vstupneSlovo + '\n');
					}
					else {
						subor.insert(subor.begin() + riadok, vstupneSlovo + '\n');		// Vlozenie stringu na urcitu poziciu.
					}

				}
				riadok = 0;
				vstupneSlovo.clear();
				continue;
			}
			if (riadok == -1) {													// Ak je riadok = -1 -> nastavi sa hodnota riadku na velkost suboru
				riadok = (int)subor.size();									// Append na koniec.
			}
			string text;
			vector<string> celyText;
			while (getline(cin, text)) {										// Nacitanie textu, ktory chceme appendnut.
				if (text == ".") {
					break;
				}
				celyText.push_back(text);
			}

			if (riadok > (int)subor.size()) {									// Ak je riadok vacsi ako velkost suboru, doplnim neexistujuce riadky '\n'
				size_t velkostSuboru = subor.size();
				for (int i = 0; i < riadok - (int)velkostSuboru; i++) {
					subor.push_back("\n");
				}
				for (size_t i = 0; i < celyText.size(); i++) {					// A potom doplnim text, ktory sme zadali.
					subor.push_back(celyText[i] + '\n');
				}
			}
			else {																// Vkladanie na existujuci riadok.
				if (subor.size() != 0) {
					for (size_t i = 0; i < subor.size(); i++) {
						if ((int)(i + 1) == riadok) {
							for (size_t j = 0; j < celyText.size(); j++) {
								subor.insert(subor.begin() + i + 1, celyText[j] + '\n');
								i++;
							}
							break;
						}
					}
				}
				else {															// Vkladanie do suboru, ak vstupny subor neexistoval.
					for (size_t j = 0; j < celyText.size(); j++) {
						subor.push_back(celyText[j] + '\n');
					}
				}
			}
			celyText.clear();
			riadok = 0;
		}
		else if (znak == 'p') {													// Vypisanie urcitej casti suboru.
			zistiRange(rozsah, prikaz, int(subor.size()));						// Zistenie range-u, ktory treba vypisat.

			// Rozsah : [m, n]
			if (rozsah.size() == 0) {
				vypisSubor(0, int(subor.size() - 1), subor);					// Vypisanie celeho suboru.
			}
			else if (rozsah.size() == 1) {										// Vypisanie urciteho riadku.							
				if ((int)subor.size() >= rozsah[0]) {
					vypisSubor(rozsah[0] - 1, rozsah[0] - 1, subor);
				}
				else {
					cout << "Invalid range" << endl;
				}
			}
			else if (rozsah.size() == 2) {										// Vypisanie urciteho rozsahu.
				if (rozsah[0] > rozsah[1]) {									// Ak je zaciatok range-u > koniec range-u -> "Invalid range".
					cout << "Invalid range" << endl;
				}
				else {
					if ((int)subor.size() >= rozsah[0] && (int)subor.size() >= rozsah[1]) {
						vypisSubor(rozsah[0] - 1, rozsah[1] - 1, subor);
					}
					else {
						cout << "Invalid range" << endl;
					}
				}
			}

			rozsah.clear();
		}
		else if (znak == 'd') {													// Vymazanie urcitej casti suboru.
			zistiRange(rozsah, prikaz, int(subor.size()));						// Zistenie range-u.

			if (rozsah.size() == 0) {											// Ak je velkost range-u 0 -> prikaz bez range-u  
				subor.clear();													// odstranenie celeho suboru.
			}
			else if (rozsah.size() == 1) {										// Vymazanie urciteho riadku.
				if ((int)subor.size() >= rozsah[0]) {
					subor.erase(subor.begin() + rozsah[0] - 1);
				}
			}
			else if (rozsah.size() == 2) {										// Vymazanie urciteho range-u.
				if (rozsah[0] > rozsah[1]) {									// Ak je zaciatok range-u > koniec range-u -> "Invalid range".
					cout << "Invalid range" << endl;
				}
				else if((int)subor.size() >= rozsah[0] && (int)subor.size() >= rozsah[1])
				{																// Odstranenie urciteho range-u.
					for (int i = 0; i < (rozsah[1] - rozsah[0] + 1); i++) {
						subor.erase(subor.begin() + rozsah[0] - 1);
					}
				}
			}

			rozsah.clear();
		}
		else if (znak == 'q') {
			if (prikaz.length() == 1) {											// Iba samotny prikaz 'q'
				vstupnySubor.open(argv[1], ios::out | ios::in);					// Otvorenie suboru a vymazanie aktualneho obsahu.

				vstupnySubor.seekg(0, ios::beg);
				int i = 0;
				string r = "";
				bool flag = true;
				string temp = "";
				while (getline(vstupnySubor, r)) {								// Kontrola vector<string> so vstupnym suborom
					if (count(subor[i].begin(), subor[i].end(), '\n') != 0) {	// Vytvorenie pomocneho stringu bez znaku '\n'. 
						for (size_t x = 0; x < subor[i].size() - 1; x++) {
							temp += subor[i][x];
						}
					}
					else {
						temp = subor[i];
					}
					if (r.compare(temp) != 0) {									// Ak najdem nezhodu -> vratim sa spat.
						cout << "You have unsaved changes" << endl;
						flag = false;
						break;
					}
					i++;
					temp.clear();
				}

				if ((int)subor.size() > i && flag) {
					cout << "You have unsaved changes" << endl;
					flag = false;
				}
				vstupnySubor.close();
				if (flag) {														// Ak nie su ziadne zmeny ukoncim program.
					break;
				}
			}
			else if (prikaz.length() == 2 && prikaz[1] == '!') {				// Ak je prikaz 'q!', ukoncim program bez kontroly.
				break;
			}
			else {																// Inak "Unsupported command".
				cout << "Unsupported command" << endl;
			}
		}
		else if (znak == 'c') {
			zistiRange(rozsah, prikaz, int(subor.size()));						// Zistenie range-u.
			string text = "";
			vector<string> novyText;
			if (rozsah.size() == 0) {											// Ak je velkost range-u 0 -> nahradim cely subor.
				subor.clear();
				while (getline(cin, text)) {									// Nacitanie vstupu.
					if (text == ".") {
						break;
					}
					subor.push_back(text + '\n');
				}
			}
			else if (rozsah.size() == 1) {										// Nahradenie jedneho riadku.
				if((int)subor.size() >= rozsah[0]){
					subor.erase(subor.begin() + rozsah[0] - 1);
				}
				while (getline(cin, text)) {
					if (text == ".") {
						break;
					}
					novyText.push_back(text);
				}
				if (rozsah[0] > (int)subor.size()) {							// Vytvorene neexistujucich riadkov a dopisanie stringu.
					int v = (int)subor.size();
					for (int i = 0; i < rozsah[0] - v - 1; i++) {
						subor.push_back("\n");
					}
					for (size_t i = 0; i < novyText.size(); i++) {
						subor.push_back(novyText[i] + '\n');
					}
				}
				else {
					for (size_t i = 0; i < subor.size(); i++) {
						if ((int)(i + 1) == rozsah[0]) {
							for (size_t j = 0; j < novyText.size(); j++) {
								subor.insert(subor.begin() + rozsah[0] - 1 + j, novyText[j] + '\n');
							}
							break;
						}
					}
				}
			}
			else if (rozsah.size() == 2) {										
				if (rozsah[0] > rozsah[1]) {									// Ak je zaciatok range-u > koniec range-u -> "Invalid range".
					cout << "Invalid range" << endl;
					continue;
				}
				for (int i = 0; i < (rozsah[1] - rozsah[0] + 1); i++) {
					subor.erase(subor.begin() + rozsah[0] - 1);
				}
				while (getline(cin, text)) {
					if (text == ".") {
						break;
					}
					novyText.push_back(text);
				}
				for (size_t i = 0; i < novyText.size(); i++) {
					subor.insert(subor.begin() + rozsah[0] + i - 1, novyText[i] + '\n');
				}

			}
			rozsah.clear();
		}
		else {
			cout << "Unsupported command" << endl;
		}
	}

	return 0;
}