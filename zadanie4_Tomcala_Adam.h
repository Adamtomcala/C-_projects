#pragma once
// here you can include whatever you want :)

#include <string>
#include <stdint.h>
#include <vector>
#include <exception>
#include <iostream>
#define SUPPORT_DIVISION 0 // define as 1 when you have implemented the division
#define SUPPORT_IFSTREAM 0 // define as 1 when you have implemented the input >>

using namespace std;
// if you do not plan to implement bonus, just delete those lines

class BigNum final
{
public:

    // constructors
    
    BigNum() : cislo("0")                                                       // Konstruktor bez parametrov, "0"
    {   }

    BigNum(int64_t n) {                                                         // Int64_t prevadzam na string          
        bool jeZaporne = false;
        if (n < 0) {                                                            // Zistenie ci je zaporne
            jeZaporne = true;
            n *= -1;
        }
        while (n >= 10) {                                                       // Zistovanie jednotlivych cifier
            cislo.append(to_string(n % 10));                                    // a prepis na String
            n /= 10;
        }
        cislo.append(to_string(n % 10));
        if (jeZaporne) {
            cislo.append("-");
        }
        reverse(cislo.begin(), cislo.end());                                    // Otocenie cisla
    }

    explicit BigNum(const std::string& str) {
            for (size_t i = 0; i < str.size(); i++) {                           // Kontrola vstupu
                if (i == 0 && (str[i] == '-' || str[i] == '+')) {
                    continue;
                }
                if (!isdigit(str[i])) {                                         // Ak to nie je cislica -> chyba
                    throw exception();
                }
            }
            this->cislo = str;
            if (this->cislo[0] == '+') {
                this->cislo.erase(this->cislo.begin());
            }

            for (int i = 0; i < int(this->cislo.length() - 1); i++) {           // Odstranenie zbytocnych nul
                if (this->cislo[i] == '-') {
                    continue;
                }
                if (this->cislo[i] == '0') {
                    this->cislo.erase(this->cislo.begin() + i);
                    --i;
                }
                else {
                    break;
                }
            }
    }

    // copy
    BigNum(const BigNum& other) : cislo(other.cislo)
    {   }

    BigNum& operator=(const BigNum& rhs) {                                      // Priradovaci konstruktor
        if (this != &rhs) {                                                     // Ak sa cisla nerovnaju, prepisem ho
            cislo = rhs.cislo;
        }
        return *this;
    }

    // unary operators
    const BigNum& operator+() const {                                           // Konstruktor vraciam ten istz=y objekt
        return *this;
    }
    BigNum operator-() const {
        if (this->cislo[0] == '-') {                                            // Zmena cisla na opacne
            return BigNum(this->cislo.substr(1));
        }
        string noveCislo = "-";
        for (int i = 0; i < int(this->cislo.length()); i++) {
            noveCislo.push_back(this->cislo[i]);
        }
        return BigNum(noveCislo);
    }

    // binary arithmetics operators
    BigNum& operator+=(const BigNum& rhs) {
        string prveCislo = this->cislo;
        string druheCislo = rhs.cislo;

        bool swapped = false;
        odstran_minus(prveCislo, druheCislo);                                   // Funkcia odstranuje z cisla '-'
        najdi_vacsie(prveCislo, druheCislo, swapped);                           // Funkcia zisti, ktore cislo je vacsie (to je prve)

        string vysledok;

        // Ak su obe kladne alebo zaporne, scitam ich
        if ((isdigit(this->cislo[0]) && isdigit(rhs.cislo[0])) || (this->cislo[0] == '-' && rhs.cislo[0] == '-')) {
            vysledok = scitaj(prveCislo, druheCislo);
        }
        // Inak ich odcitam
        else {
            vysledok = odcitaj(prveCislo, druheCislo);    
        }
        // Vymazanie nadbytocnych nul
        odstan_nuly(vysledok);

        // Ak bolo vacsie cislo zaporne pridam minus
        if (prveCislo == this->cislo.substr(1) || prveCislo == rhs.cislo.substr(1)) {                                           // Pridanie -
            vysledok.insert(vysledok.begin(), '-');
        }
        // Pripisem vysledok na aktualny
        this->cislo = vysledok;
        return *this;
    }

    BigNum& operator-=(const BigNum& rhs) {
        string prveCislo = this->cislo;
        string druheCislo = rhs.cislo;
        bool jeZapornePrve = prveCislo.at(0) == '-' ? true : false;         // Zapamatanie si ci boli cisla zaporne alebo nie
        bool jeZaporneDruhe = druheCislo.at(0) == '-' ? true : false;

        bool swapped = false;
        odstran_minus(prveCislo, druheCislo);                                       
        najdi_vacsie(prveCislo, druheCislo, swapped);

        string vysledok;
        // Ak su cisla roznych znamienok, scitam ich 
        if ((isdigit(this->cislo[0]) && !isdigit(rhs.cislo[0])) || (!isdigit(this->cislo[0]) && isdigit(rhs.cislo[0]))) {
            vysledok = scitaj(prveCislo, druheCislo);
        }
        // Inak ich odcitam
        else {
            vysledok = odcitaj(prveCislo, druheCislo);
        }
        // Odstanienie nadbytocnych nul
        odstan_nuly(vysledok);

        // Zistovanie ci mam pridat '-' pred cislo
        if (vysledok.compare("0") != 0) {
            if (swapped) {
                swap(jeZapornePrve, jeZaporneDruhe);
                if ((!jeZapornePrve && jeZaporneDruhe) || (!jeZapornePrve && !jeZaporneDruhe)) {
                    vysledok.insert(vysledok.begin(), '-');
                }
            }
            else {
                if (jeZapornePrve) {
                    vysledok.insert(vysledok.begin(), '-');
                }
            }
        }

        this->cislo = vysledok;
        return *this;
    }
    
    BigNum& operator*=(const BigNum& rhs)
    {
        string prveCislo = this->cislo;
        string druheCislo = rhs.cislo;
        bool jeZaporneP = this->cislo[0] == '-' ? true : false;             // Zapamatanie si znamienka cisel
        bool jeZaporneD = rhs.cislo[0] == '-' ? true : false;

        odstran_minus(prveCislo, druheCislo);

        // Ak je jedno cislo -> vysledok je 0
        if (prveCislo == "0" || druheCislo == "0") {                        
            this->cislo = "0";
            return *this;
        }

        // Zistenie, ktore cislo je vacsie
        if (prveCislo.length() < druheCislo.length()) {
            swap(prveCislo, druheCislo);
        }

        // Samotne nasobenie cisel
        int priebVysledok;
        int index1 = 0;
        int index2 = 0;
        int64_t velkost = druheCislo.length() + prveCislo.length();
        vector<int> vysledky(velkost, 0);
        int zvysok;
        for (int i = int(druheCislo.length() - 1); i >= 0; i--) {
            zvysok = 0;
            index2 = 0;
            for (int j = int(prveCislo.length() - 1); j >= 0; j--) {
                velkost = index1 + index2;
                priebVysledok = (druheCislo[i] - '0') * (prveCislo[j] - '0') + vysledky[velkost] + zvysok;
                vysledky[index1 + index2] = priebVysledok % 10;
                zvysok = priebVysledok / 10;
                ++index2;
                if (j == 0) {
                    vysledky[index1 + index2] = zvysok;
                    break;
                }
            }
            if (i == 0) {
                break;
            }
            ++index1;
        }
        // Reversnutie vysledku
        reverse(vysledky.begin(), vysledky.end());
        // Odstranenie nul
        for (int i = 0; i < int(vysledky.size()); i++) {
            if (vysledky[i] == 0) {
                vysledky.erase(vysledky.begin());
                --i;
            }
            else {
                break;
            }
        }

        // Prepis na string
        string vysledok;
        for (int i = 0; i < int(vysledky.size()); i++) {
            vysledok.append(to_string(vysledky[i]));
        }
        // Pridanie '-', ak treba
        if (jeZaporneD != jeZaporneP) {
            vysledok.insert(vysledok.begin(), '-');
        }

        this->cislo = vysledok;
        return *this;
    }

#if SUPPORT_DIVISION == 1    
    BigNum& operator/=(const BigNum& rhs); // bonus
    BigNum& operator%=(const BigNum& rhs); // bonus
#endif

private:
    string cislo;

    // Funckia odstrani '-'
    void odstran_minus(string& prveCislo, string& druheCislo) {
        if (prveCislo.at(0) == '-') {
            prveCislo.erase(prveCislo.begin());
        }
        if (druheCislo.at(0) == '-') {
            druheCislo.erase(druheCislo.begin());
        }
    }
    // Funckia najde vacsie cislo
    void najdi_vacsie(string& prveCislo, string& druheCislo, bool& swapped) {
        if (prveCislo.length() > druheCislo.length()) {
            return;
        }
        if (prveCislo.length() < druheCislo.length()) {
            swap(prveCislo, druheCislo);
            swapped = true;
        }
        else {
            for (size_t i = 0; i < prveCislo.length(); i++) {
                if (prveCislo[i] != druheCislo[i]) {
                    if (prveCislo[i] < druheCislo[i]) {
                        swap(prveCislo, druheCislo);
                        swapped = true;
                        break;
                    }
                    else {
                        return;
                    }
                }
            }
        }
    }
    // Funkcia odstranuje nadbytocne 0
    void odstan_nuly(string& vysledok) {
        for (int i = 0; i < int(vysledok.length() - 1); i++) {
            if (vysledok[i] == '0' && i == 0) {
                vysledok.erase(vysledok.begin());
                --i;
            }
            else {
                break;
            }
        }
    }
    // Funckia na scitanie cisel pod seba
    string scitaj(string prveCislo, string druheCislo) {
        int cislica;                                                    
        int zvysok = 0;
        string vysledok;
        int indexDruheho = int(druheCislo.length() - 1);            //prva cifra druheho cisla
        int indexPrveho = int(prveCislo.length() - 1);              //prva cifra prveho cisla

        for (int i = indexDruheho; i >= 0; i--) {                   //Scitavam do konca spodneho cisla
            cislica = (prveCislo[indexPrveho] - '0') + (druheCislo[i] - '0') + zvysok;
            vysledok.append(to_string((cislica) % 10));
            zvysok = cislica >= 10 ? 1 : 0;
            indexPrveho--;
            if (i == 0) {
                break;
            }
        }

        for (int i = indexPrveho; i >= 0; i--) {                    // Dokoncim aj vrchne cislo
            cislica = (prveCislo[i] - '0') + zvysok;
            vysledok.append(to_string((cislica) % 10));
            zvysok = cislica >= 10 ? 1 : 0;
            if (i == 0) {
                break;
            }
        }

        if (zvysok == 1) {                                          // Ak ostal zvysok, pridam ho.
            vysledok.append("1");
        }

        reverse(vysledok.begin(), vysledok.end());                  // Otocim cislo.
        return vysledok;
    }
    // Funkcia na odcitanie cisel pod seba
    string odcitaj(string prveCislo, string druheCislo) {
        int indexDruheho = int(druheCislo.length() - 1);
        int indexPrveho = int(prveCislo.length() - 1);
        int rozdiel;
        int zvysok = 0;
        string vysledok;
        for (int i = indexDruheho; i >= 0; i--) {
            if (prveCislo[indexPrveho] < (druheCislo[i] + zvysok)) {
                rozdiel = ((prveCislo[indexPrveho] - '0' + 10) - (druheCislo[i] - '0' + zvysok));
                vysledok.append(to_string(rozdiel));
                zvysok = 1;
            }
            else {
                rozdiel = (prveCislo[indexPrveho] - '0') - (druheCislo[i] - '0' + zvysok);
                vysledok.append(to_string(rozdiel));
                zvysok = 0;
            }
            indexPrveho--;
            if (i == 0) {
                break;
            }
        }

        for (int i = indexPrveho; i >= 0; i--) {
            if (zvysok > prveCislo[i] - '0') {
                rozdiel = (prveCislo[indexPrveho] - '0' + 10) - zvysok;
                vysledok.append(to_string(rozdiel));
                zvysok = 1;
            }
            else {
                vysledok.append(to_string(prveCislo[i] - '0' - zvysok));
                zvysok = 0;
            }
            if (i == 0) {
                break;
            }
        }

        reverse(vysledok.begin(), vysledok.end());
        return vysledok;
    }

    // Zadefinovanie friendov
    friend bool operator>=(const BigNum& lhs, const BigNum& rhs);
    friend bool operator<=(const BigNum& lhs, const BigNum& rhs);
    friend bool operator>(const BigNum& lhs, const BigNum& rhs);
    friend bool operator<(const BigNum& lhs, const BigNum& rhs);
    friend bool operator!=(const BigNum& lhs, const BigNum& rhs);
    friend bool operator==(const BigNum& lhs, const BigNum& rhs);
    friend BigNum operator+(BigNum lhs, const BigNum& rhs);
    friend BigNum operator-(BigNum lhs, const BigNum& rhs);
    friend BigNum operator*(BigNum lhs, const BigNum& rhs);
    friend std::ostream& operator<<(std::ostream& lhs, const BigNum& rhs);
};



BigNum operator+(BigNum lhs, const BigNum& rhs) {
    bool jeZaporneP = lhs.cislo[0] == '-' ? true : false;       // Zapamatanie si znamienok cisel
    bool jeZaporneD = rhs.cislo[0] == '-' ? true : false;
    bool swapped = false;
    string prveCislo = lhs.cislo;
    string druheCislo = rhs.cislo;

    lhs.odstran_minus(prveCislo, druheCislo);
    lhs.najdi_vacsie(prveCislo, druheCislo, swapped);

    string vysledok;

    // Ak su znamienka rozdielne, odctavam
    if (jeZaporneD != jeZaporneP) {
        vysledok = lhs.odcitaj(prveCislo, druheCislo);
    }
    // Inak scitavam
    else
    {
        vysledok = lhs.scitaj(prveCislo, druheCislo);
    }
    
    lhs.odstan_nuly(vysledok);

    if (prveCislo == lhs.cislo.substr(1) || prveCislo == rhs.cislo.substr(1)) {                                           // Pridanie -
        vysledok.insert(vysledok.begin(), '-');
    }

    return BigNum (vysledok);
}
BigNum operator-(BigNum lhs, const BigNum& rhs) {
    // Takmer totozne postupy ako pri -=
    bool jeZaporneP = lhs.cislo[0] == '-' ? true : false;
    bool jeZaporneD = rhs.cislo[0] == '-' ? true : false;
    bool swapped = false;
    string prveCislo = lhs.cislo;
    string druheCislo = rhs.cislo;


    lhs.odstran_minus(prveCislo, druheCislo);
    lhs.najdi_vacsie(prveCislo, druheCislo, swapped);

    string vysledok;
    if (jeZaporneD != jeZaporneP) {
        vysledok = lhs.scitaj(prveCislo, druheCislo);
    }
    else
    {
        vysledok = lhs.odcitaj(prveCislo, druheCislo);
    }

    lhs.odstan_nuly(vysledok);

    if (vysledok.compare("0") != 0) {
        if (swapped) {
            swap(jeZaporneP, jeZaporneD);
            if ((!jeZaporneP && jeZaporneD) || (!jeZaporneP && !jeZaporneD)) {
                vysledok.insert(vysledok.begin(), '-');
            }
        }
        else {
            if (jeZaporneP) {
                vysledok.insert(vysledok.begin(), '-');
            }
        }
    }

    return BigNum(vysledok);
}
BigNum operator*(BigNum lhs, const BigNum& rhs) {
    // Takmer rovnaky postup ako pri *=
    string prveCislo = lhs.cislo;
    string druheCislo = rhs.cislo;
    bool jeZaporneP = lhs.cislo[0] == '-' ? true : false;
    bool jeZaporneD = rhs.cislo[0] == '-' ? true : false;
    
    lhs.odstran_minus(prveCislo, druheCislo);

    if (prveCislo == "0" || druheCislo == "0") {
        return BigNum();
    }

    if (prveCislo.length() < druheCislo.length()) {
        swap(prveCislo, druheCislo);
    }

    int priebVysledok;
    int index1 = 0;
    int index2 = 0;
    int64_t velkost = druheCislo.length() + prveCislo.length();
    vector<int> vysledky(velkost, 0);
    int zvysok;
    for (int i = int(druheCislo.length() - 1); i >= 0; i--) {
        zvysok = 0;
        index2 = 0;
        for (int j = int(prveCislo.length() - 1); j >= 0; j--) {
            velkost = index1 + index2;
            priebVysledok = (druheCislo[i] - '0') * (prveCislo[j] - '0') + vysledky[velkost] + zvysok;
            vysledky[index1 + index2] = priebVysledok % 10;
            zvysok = priebVysledok / 10;
            ++index2;
            if (j == 0) {
                vysledky[index1 + index2] = zvysok;
                break;
            }
        }
        if (i == 0) {
            break;
        }
        ++index1;
    }
    reverse(vysledky.begin(), vysledky.end());
    for (int i = 0; i < int(vysledky.size()); i++) {
        if (vysledky[i] == 0) {
            vysledky.erase(vysledky.begin());
            --i;
        }
        else {
            break;
        }
    }

    string vysledok;
    for (int i = 0; i < int(vysledky.size()); i++) {
        vysledok.append(to_string(vysledky[i]));
    }
    if (jeZaporneD != jeZaporneP) {
        vysledok.insert(vysledok.begin(), '-');
    }

    return BigNum(vysledok);
}
#if SUPPORT_DIVISION == 1
BigNum operator/(BigNum lhs, const BigNum& rhs); // bonus
BigNum operator%(BigNum lhs, const BigNum& rhs); // bonus
#endif
                                                 
// alternatively you can implement
// std::strong_ordering operator<=>(const BigNum& lhs, const BigNum& rhs);
// idea is, that all comparison should work, it is not important how you do it
bool operator==(const BigNum& lhs, const BigNum& rhs) {
    bool jezaporneP = lhs.cislo[0] == '-' ? true : false;
    bool jezaporneD = rhs.cislo[0] == '-' ? true : false;

    if (jezaporneP != jezaporneD) {
        return false;
    }

    if (rhs.cislo.length() != lhs.cislo.length()) {
        return false;
    }

    for (size_t i = 0; i < lhs.cislo.length(); i++) {
        if (lhs.cislo[i] != rhs.cislo[i]) {
            return false;
        }
    }
    return true;
}
bool operator!=(const BigNum& lhs, const BigNum& rhs) {
    return !(lhs == rhs);
}
bool operator<(const BigNum& lhs, const BigNum& rhs) {
    return !(lhs >= rhs);
}
bool operator>(const BigNum& lhs, const BigNum& rhs) {
    return !(lhs <= rhs);
}
bool operator<=(const BigNum& lhs, const BigNum& rhs) {
    bool jezaporneP = lhs.cislo[0] == '-' ? true : false;
    bool jezaporneD = rhs.cislo[0] == '-' ? true : false;

    if (jezaporneP && !jezaporneD) {
        return true;
    }
    if (!jezaporneP && jezaporneD) {
        return false;
    }
    if (jezaporneP && jezaporneD) {
        if (lhs.cislo.length() > rhs.cislo.length()) {
            return true;
        }
        else if (lhs.cislo.length() < rhs.cislo.length()) {
            return false;
        }
    }

    if (!jezaporneP && !jezaporneD) {
        if (lhs.cislo.length() > rhs.cislo.length()) {
            return false;
        }
        else if (lhs.cislo.length() < rhs.cislo.length()) {
            return true;
        }
    }

    for (size_t i = 0; i < lhs.cislo.length(); i++) {
        if (lhs.cislo[i] != rhs.cislo[i]) {
            if (lhs.cislo[i] > rhs.cislo[i]) {
                if (jezaporneP && jezaporneD) {
                    return true;
                }
                else {
                    return false;
                }
            }
            else if (lhs.cislo[i] < rhs.cislo[i]) {
                if (jezaporneP && jezaporneD) {
                    return false;
                }
                else {
                    return true;
                }
            }
        }
    }
    return true;
}
bool operator>=(const BigNum& lhs, const BigNum& rhs) {
    bool jezaporneP = lhs.cislo[0] == '-' ? true : false;
    bool jezaporneD = rhs.cislo[0] == '-' ? true : false;

    if (jezaporneP && !jezaporneD) {
        return false;
    }
    if (!jezaporneP && jezaporneD) {
        return true;
    }

    if (jezaporneP && jezaporneD) {
        if(lhs.cislo.length() > rhs.cislo.length()){
            return false;
        }
        else if(lhs.cislo.length() < rhs.cislo.length()){
            return true;
        }
    }

    if (!jezaporneP && !jezaporneD) {
        if (lhs.cislo.length() > rhs.cislo.length()) {
            return true;
        }
        else if(lhs.cislo.length() < rhs.cislo.length()) {
            return false;
        }
    }

    for (size_t i = 0; i < lhs.cislo.length(); i++) {
        if (lhs.cislo[i] != rhs.cislo[i]) {
            if (lhs.cislo[i] > rhs.cislo[i]) {
                if (jezaporneP && jezaporneD) {
                    return false;
                }
                else {
                    return true;
                }
            }
            else if (lhs.cislo[i] < rhs.cislo[i]) {
                if (jezaporneP && jezaporneD) {
                    return true;
                }
                else {
                    return false;
                }
            }
        }
    }
    return true;
}

std::ostream& operator<<(std::ostream& lhs, const BigNum& rhs) {
    for (int i = 0; i < int(rhs.cislo.length()); i++) {
        lhs << rhs.cislo[i];
    }
    return lhs;
}
/*
#if SUPPORT_IFSTREAM == 1
std::istream & operator>>(std::istream & lhs, BigNum & rhs); // bonus
#endif
*/