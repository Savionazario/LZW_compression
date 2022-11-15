#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

vector<int> encoding(ifstream &input, int tamanhoArq)
{
    unordered_map<string, int> table;
    vector<int> output_code;
    int code = 256;
    char a;
    int count = 0;

    cout << "Encoding\n";

    for (int i = 0; i <= 255; i++)
    {
        string ch = "";
        ch += char(i);
        table[ch] = i;
    }

    string p = "", c = "";

    input.open("teste.txt");

    input >> a;
    //cout << "VALOR DO CHAR a: " << a << endl;

    p += a;
    if (!input.good())
    {
        input.close();
    }
    else
    {
        while (!input.eof())
        {
            count ++;
            char b;
            input >> b;
            //cout << "VALOR DO CHAR b: " << b << endl;
            if(count != tamanhoArq){
               c += b; 
            }
            

            if (table.find(p + c) != table.end())
            {
                p = p + c;
            }
            else
            {
                cout << p << "\t" << table[p] << "\t\t"
                     << p + c << "\t" << code << endl;
                output_code.push_back(table[p]);
                table[p + c] = code;
                code++;
                p = c;
            }
            c = "";
        }
        input.close();
    }
    cout << p << "\t" << table[p] << endl;
    output_code.push_back(table[p]);
    return output_code;
}

int main()
{
    // opening the file in read mode
    FILE* fp = fopen("teste.txt", "r");
  
    // checking if the file exist or not
    if (fp == NULL) {
        printf("File Not Found!\n");
        return -1;
    }
  
    fseek(fp, 0L, SEEK_END);
  
    // calculating the size of the file
    long int tam = ftell(fp);
    cout << "Tamanho do arquivo: " << tam << endl;
  
    // closing the file
    fclose(fp);

    ifstream input;

    vector<int> output_code = encoding(input, tam);

    cout << "Output Codes are: ";

    for (int i = 0; i < output_code.size(); i++)
    {
        cout << output_code[i] << " ";
    }
    cout << endl;
}
