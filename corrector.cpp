/*****************************************************************************************************************
	UNIVERSIDAD NACIONAL AUTONOMA DE MEXICO
	FACULTAD DE ESTUDIOS SUPERIORES -ARAGON-

    Proyecto ICO
	
	323274603 Vega Gallardo Brandon Alexander
	
	Distancia de Levenstein - Corrector ortografico
	
******************************************************************************************************************/

#include <iostream>
#include <fstream>
#include <string.h>
#include <cctype>
#include "corrector.h"
using namespace std;

const char ABC[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    (char)0xF1, 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    (char)0xE1, (char)0xE9, (char)0xED, (char)0xF3, (char)0xFA, '\0'
};
const int SIZE_ABC = 32;

void toLowerCase(char* str) {
    int i = 0;
    while (str[i]) {
        str[i] = tolower(str[i]);
        i++;
    }
}

bool isDelimiter(char ch) {
    if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') return true;
    if (ch == ',' || ch == '.' || ch == ';') return true;
    if (ch == '(' || ch == ')') return true;
    return false;
}

void removePunctuation(char* word) {
    int len = strlen(word);
    while (len > 0) {
        char last = word[len-1];
        if (last == ',' || last == '.' || last == ';' || last == '(' || last == ')') {
            word[len-1] = '\0';
            len--;
        } else {
            break;
        }
    }
}

int binaryFind(char words[][TAMTOKEN], int size, const char* target) {
    int left = 0;
    int right = size - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int comp = strcmp(words[mid], target);
        
        if (comp == 0) {
            return mid;
        } else if (comp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

void insertSorted(char words[][TAMTOKEN], int counts[], int& size, const char* word) {
    int insertPos = 0;
    while (insertPos < size && strcmp(words[insertPos], word) < 0) {
        insertPos++;
    }
    
    for (int i = size; i > insertPos; i--) {
        strcpy_s(words[i], TAMTOKEN, words[i-1]);
        counts[i] = counts[i-1];
    }
    
    strcpy_s(words[insertPos], TAMTOKEN, word);
    counts[insertPos] = 1;
    size++;
}

void processWord(char words[][TAMTOKEN], int counts[], int& size, char* word) {
    removePunctuation(word);
    toLowerCase(word);
    
    if (strlen(word) == 0) return;
    
    int found = binaryFind(words, size, word);
    
    if (found >= 0) {
        counts[found]++;
    } else {
        insertSorted(words, counts, size, word);
    }
}

// ==================== FUNCION 1: DICCIONARIO ====================
void Diccionario(char* szNombre, char szPalabras[][TAMTOKEN], 
                 int iEstadisticas[], int& iNumElementos) {
    iNumElementos = 0;
    
    ifstream file(szNombre);
    if (!file.is_open()) return;
    
    char word[TAMTOKEN];
    int pos = 0;
    char ch;
    
    while (file.get(ch)) {
        if (isDelimiter(ch)) {
            if (pos > 0) {
                word[pos] = '\0';
                processWord(szPalabras, iEstadisticas, iNumElementos, word);
                pos = 0;
            }
        } else {
            if (pos < TAMTOKEN - 1) {
                word[pos] = ch;
                pos++;
            }
        }
    }
    
    if (pos > 0) {
        word[pos] = '\0';
        processWord(szPalabras, iEstadisticas, iNumElementos, word);
    }
    
    file.close();
}

void generateDeletions(char* original, char suggestions[][TAMTOKEN], int& count) {
    int len = strlen(original);
    
    for (int i = 0; i < len; i++) {
        char temp[TAMTOKEN];
        int k = 0;
        
        for (int j = 0; j < len; j++) {
            if (i != j) {
                temp[k] = original[j];
                k++;
            }
        }
        temp[k] = '\0';
        
        if (k > 0) {
            strcpy_s(suggestions[count], TAMTOKEN, temp);
            count++;
        }
    }
}

void generateTranspositions(char* original, char suggestions[][TAMTOKEN], int& count) {
    int len = strlen(original);
    
    for (int i = 0; i < len - 1; i++) {
        char temp[TAMTOKEN];
        strcpy_s(temp, TAMTOKEN, original);
        
        char swap = temp[i];
        temp[i] = temp[i+1];
        temp[i+1] = swap;
        
        strcpy_s(suggestions[count], TAMTOKEN, temp);
        count++;
    }
}

void generateReplacements(char* original, char suggestions[][TAMTOKEN], int& count) {
    int len = strlen(original);
    
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < SIZE_ABC; j++) {
            char temp[TAMTOKEN];
            strcpy_s(temp, TAMTOKEN, original);
            temp[i] = ABC[j];
            
            strcpy_s(suggestions[count], TAMTOKEN, temp);
            count++;
        }
    }
}

void generateInsertions(char* original, char suggestions[][TAMTOKEN], int& count) {
    int len = strlen(original);
    
    for (int i = 0; i <= len; i++) {
        for (int j = 0; j < SIZE_ABC; j++) {
            char temp[TAMTOKEN];
            int k = 0;
            
            for (int m = 0; m < i; m++) {
                temp[k] = original[m];
                k++;
            }
            
            temp[k] = ABC[j];
            k++;
            
            for (int m = i; m < len; m++) {
                temp[k] = original[m];
                k++;
            }
            temp[k] = '\0';
            
            strcpy_s(suggestions[count], TAMTOKEN, temp);
            count++;
        }
    }
}

void sortSuggestions(char suggestions[][TAMTOKEN], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(suggestions[i], suggestions[j]) > 0) {
                char temp[TAMTOKEN];
                strcpy_s(temp, TAMTOKEN, suggestions[i]);
                strcpy_s(suggestions[i], TAMTOKEN, suggestions[j]);
                strcpy_s(suggestions[j], TAMTOKEN, temp);
            }
        }
    }
}

// ==================== FUNCION 2: CLONAR PALABRAS ====================
void ClonaPalabras(char* szPalabraLeida, char szPalabrasSugeridas[][TAMTOKEN], 
                   int& iNumSugeridas) {
    iNumSugeridas = 0;
    
    strcpy_s(szPalabrasSugeridas[iNumSugeridas], TAMTOKEN, szPalabraLeida);
    iNumSugeridas++;
    
    generateDeletions(szPalabraLeida, szPalabrasSugeridas, iNumSugeridas);
    generateTranspositions(szPalabraLeida, szPalabrasSugeridas, iNumSugeridas);
    generateReplacements(szPalabraLeida, szPalabrasSugeridas, iNumSugeridas);
    generateInsertions(szPalabraLeida, szPalabrasSugeridas, iNumSugeridas);
    
    sortSuggestions(szPalabrasSugeridas, iNumSugeridas);
}

bool alreadyExists(char list[][TAMTOKEN], int size, const char* word) {
    for (int i = 0; i < size; i++) {
        if (strcmp(list[i], word) == 0) {
            return true;
        }
    }
    return false;
}

void sortByWeight(char list[][TAMTOKEN], int weights[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            if (weights[i] < weights[j]) {
                int tempWeight = weights[i];
                weights[i] = weights[j];
                weights[j] = tempWeight;
                
                char tempWord[TAMTOKEN];
                strcpy_s(tempWord, TAMTOKEN, list[i]);
                strcpy_s(list[i], TAMTOKEN, list[j]);
                strcpy_s(list[j], TAMTOKEN, tempWord);
            }
        }
    }
}

// ==================== FUNCION 3: LISTA CANDIDATAS ====================
void ListaCandidatas(char szPalabrasSugeridas[][TAMTOKEN], int iNumSugeridas,
                     char szPalabras[][TAMTOKEN], int iEstadisticas[], int iNumElementos,
                     char szListaFinal[][TAMTOKEN], int iPeso[], int& iNumLista) {
    iNumLista = 0;
    
    for (int i = 0; i < iNumSugeridas; i++) {
        int found = binaryFind(szPalabras, iNumElementos, szPalabrasSugeridas[i]);
        
        if (found >= 0) {
            if (!alreadyExists(szListaFinal, iNumLista, szPalabras[found])) {
                strcpy_s(szListaFinal[iNumLista], TAMTOKEN, szPalabras[found]);
                iPeso[iNumLista] = iEstadisticas[found];
                iNumLista++;
            }
        }
    }
    
    sortByWeight(szListaFinal, iPeso, iNumLista);
}

// ==================== PROGRAMA PRINCIPAL ====================
int main() {
    char vocabulary[NUMPALABRAS][TAMTOKEN];
    int frequencies[NUMPALABRAS];
    int totalWords = 0;
    
    cout << "=== CORRECTOR ORTOGRAFICO ===" << endl;
    cout << "Nombre del archivo: ";
    char fileName[100];
    cin >> fileName;
    
    Diccionario(fileName, vocabulary, frequencies, totalWords);
    
    if (totalWords == 0) {
        cout << "No se pudo cargar el archivo." << endl;
        return 1;
    }
    
    cout << "Total de palabras: " << totalWords << endl;
    
    char input[TAMTOKEN];
    while (true) {
        cout << "\nEscribe una palabra (o 'salir'): ";
        cin >> input;
        
        if (strcmp(input, "salir") == 0) break;
        
        toLowerCase(input);
        int idx = binaryFind(vocabulary, totalWords, input);
        
        if (idx >= 0) {
            cout << "La palabra es correcta" << endl;
        } else {
            cout << "Palabra incorrecta. Generando sugerencias..." << endl;
            
            char variations[5000][TAMTOKEN];
            int totalVariations = 0;
            ClonaPalabras(input, variations, totalVariations);
            
            char candidates[1000][TAMTOKEN];
            int weights[1000];
            int totalCandidates = 0;
            ListaCandidatas(variations, totalVariations, vocabulary, 
                           frequencies, totalWords, candidates, weights, totalCandidates);
            
            if (totalCandidates > 0) {
                cout << "\nSugerencias encontradas:" << endl;
                int maxShow = totalCandidates < 10 ? totalCandidates : 10;
                for (int i = 0; i < maxShow; i++) {
                    cout << "  " << (i+1) << ". " << candidates[i] 
                         << " [" << weights[i] << "]" << endl;
                }
            } else {
                cout << "Sin sugerencias." << endl;
            }
        }
    }
    
    cout << "\nPrograma terminado." << endl;
    return 0;
}
