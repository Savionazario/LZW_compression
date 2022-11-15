#include "LZW.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

LZW::LZW(){
    dicionarioSize = 256;
}

void LZW::lerArquivo(){
    FILE *arquivo;
    char c;

    arquivo = fopen("teste.txt", "rt");

    if(arquivo == NULL){
        cout << "Não foi possível ler do arquivo" << endl;
    }else{
        while(c != EOF){
            c = fgetc(arquivo);
            cout << "caractere lido: " << c << endl;
        }
        fclose(arquivo);
    }
}