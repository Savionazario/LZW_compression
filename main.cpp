#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <chrono>

using namespace std;
#define SIZE 4096

unsigned char *buffer = new unsigned char[SIZE];
int pos = 0;
FILE *output;
const char *name = "output.lzw";

void addBits(int value, int qBits)
{
    int byte = pos / 8;
    int shift = pos % 8;
    int disp = 8 - shift;
    unsigned int mask = 0;
    int falta = qBits - disp;
    int falta2 = 0;
    int i;
    if (falta > 8)
    {
        falta2 = falta - 8;
        falta = 8;
    }

    mask = 0;
    for (i = 0; i < disp; i++)
        mask = mask | (1 << i);
    buffer[byte] = (buffer[byte] & (~mask & 0xFF)) | ((value >> (falta + falta2)) & mask);

    if (falta > 0)
    {
        mask = 0;
        for (i = 0; i < (8 - falta); i++)
            mask = mask | (1 << i);
        if (falta2 == 0)
            buffer[byte + 1] = (buffer[byte + 1] & mask) | ((value << (8 - falta)) & (~mask & 0xFF));
        else
            buffer[byte + 1] = ((value >> falta2) & 0xFF);
    }

    if (falta2 > 0)
    {
        mask = 0;
        for (i = 0; i < (8 - falta2); i++)
            mask = mask | (1 << i);
        buffer[byte + 2] = (buffer[byte + 2] & mask) | ((value << (8 - falta2)) & (~mask & 0xFF));
    }

    pos += qBits;

    byte = pos / 8;

    if (byte >= SIZE - 3)
    {
        fwrite(buffer, 1, byte, output);
        pos -= byte * 8;
        shift = pos % 8;
        for (i = 0; i < (8 - shift); i++)
            mask = mask | (1 << i);
        buffer[0] = buffer[byte] & ~mask;
    }
}

int getBits(int qBits)
{
    int value = 0;
    int byte = pos / 8;
    int shift = pos % 8;
    int disp = 8 - shift;
    unsigned int mask = 0;
    int falta = qBits - disp;
    int falta2 = 0;
    int i;
    if (falta > 8)
    {
        falta2 = falta - 8;
        falta = 8;
    }

    mask = 0;

    for (i = 0; i < disp; i++)
        mask = mask | (1 << i);
    value = (buffer[byte] & mask) << (falta + falta2);

    if (falta > 0)
    {
        if (falta2 == 0)
            value = value | ((buffer[byte + 1]) >> (8 - falta));
        else
            value = value | (buffer[byte + 1] << falta2);
    }

    if (falta2 > 0)
    {
        value = value | (buffer[byte + 2] >> (8 - falta2));
    }

    pos += qBits;

    return value;
}

void salvaFim()
{
    int byte = pos / 8;
    int shift = pos % 8;
    if (shift > 0)
        byte++;
    if (byte > 0)
    {
        fwrite(buffer, 1, byte, output);
    }
    fclose(output);
}

void lerArquivo(const char *name)
{
    unsigned char byte;
    FILE *input = fopen(name, "rb");
    if (input <= 0)
    {
        printf("Erro abrindo o arquivo %s\n", name);
        return;
    }

    while (!feof(input))
    {
        fread(&byte, 1, 1, input);
        if (feof(input))
            break;
        printf("0x%02X ", byte);
    }
    printf("\n");
    fclose(input);
}

int testeSalva()
{
    int qBits = 0, i, contador = 0;
    int value = 0;
    int j = 0;
    qBits = 16;

    vector<int> array_dict;

    ifstream input;
    input.open("dicionario.txt");

    if (!input.good())
    {
        input.close();
    }
    else
    {
        while (!input.eof())
        {
            input >> value;
            array_dict.push_back(value);
            contador++;
        }
    }
    cout << "\n";
    input.close();

    output = fopen(name, "wb");
    if (output <= 0)
    {
        printf("Erro abrindo o arquivo %s\n", name);
        return 0;
    }

    for (int k = 0; k < contador - 1; k++)
    {
        addBits(array_dict[k], qBits);
    }

    salvaFim();
    return contador - 1;
}

vector<int> testeLeitura(int contador)
{
    int i = 0;
    int qBits = 0;
    int tam = 0;
    pos = 0;
    output = fopen(name, "rb");
    if (output <= 0)
    {
        printf("Erro abrindo o arquivo %s\n", name);
        vector<int> vazio;
        return vazio;
    }
    fseek(output, 0, SEEK_END);
    tam = ftell(output);
    fseek(output, 0, SEEK_SET);
    printf("tamanho do arquivo: %d\n", tam);
    if (buffer)
        delete buffer;
    buffer = new unsigned char[tam];
    fread(buffer, 1, tam, output);

    vector<int> vet;
    qBits = 16;
    int value;

    for (i = 0; i < tam;)
    {
        i++;
        if (vet.size() == contador)
        {
            break;
        }
        else
        {
            value = getBits(qBits);
            vet.push_back(value);
        }
    }
    return vet;
}

template <typename K, typename V>
void print_map(unordered_map<K, V> const &m)
{
    for (auto const &pair : m)
    {

        cout << "{" << pair.first << ": " << pair.second << "} " << endl;
    }
}

vector<int> encoding(ifstream &input)
{
    unordered_map<string, int> table;
    vector<int> output_code;
    int code = 256;
    char a;
    int count = 0;

    FILE *fp = fopen("texto.txt", "rb");

    if (fp == NULL)
    {
        printf("File Not Found!\n");
    }

    fseek(fp, 0L, SEEK_END);

    long int tam = ftell(fp);

    fclose(fp);

    vector<char> vector_char(tam);

    cout << "Encoding\n";

    for (int i = 0; i <= 255; i++)
    {
        string ch = "";
        ch += char(i);
        table[ch] = i;
    }

    string p = "", c = "";

    ifstream arquivo("texto.txt", ios::out | ios::binary);

    char b;
    if (!arquivo.good())
    {
        cout << "N??o foi poss??vel ler do arquivo" << endl;
    }
    else
    {
        arquivo.read((char *)&vector_char[0], sizeof(char));

        p += vector_char[0];
        for (int j = 1; j < vector_char.size(); j++)
        {
            arquivo.read((char *)&vector_char[j], sizeof(char));

            c += vector_char[j];

            if (table.find(p + c) != table.end())
            {
                p = p + c;
            }
            else
            {
                output_code.push_back(table[p]);
                if (code <= 65535)
                {
                    table[p + c] = code;
                    code++;
                }
                p = c;
            }
            c = "";
        }
        arquivo.close();
    }
    output_code.push_back(table[p]);
    return output_code;
}

void decoding(vector<int> op, int contador)
{
    unordered_map<int, string> table;
    cout << "\nDecoding\n";
    ofstream arquivo_decodificado;

    arquivo_decodificado.open("decodificado.txt", ios::out | ios::binary);

    if (!arquivo_decodificado.good())
    {
        arquivo_decodificado.close();
    }
    else
    {
        for (int i = 0; i <= 255; i++)
        {
            string ch = "";
            ch += char(i);
            table[i] = ch;
        }
        int old = op[0], n;
        string s = table[old];
        string c = "";
        c += s[0];
        arquivo_decodificado << s;
        int count = 256;
        for (int i = 0; i < contador - 1; i++)
        {
            n = op[i + 1];
            if (table.find(n) == table.end())
            {
                s = table[old];
                s = s + c;
            }
            else
            {
                s = table[n];
            }
            arquivo_decodificado << s;
            c = "";
            c += s[0];
            if (count <= 65535)
            {
                table[count] = table[old] + c;
                count++;
            }
            old = n;
        }
    }
    arquivo_decodificado.close();
    // print_map(table);
}

int main()
{
    time_t start, end;
    int contador = 0;
    // setlocale(LC_ALL, "");

    ifstream input;
    auto start_time = chrono::high_resolution_clock::now();
    vector<int> output_code = encoding(input);

    ofstream output;

    output.open("dicionario.txt");

    for (int i : output_code)
    {
        output << i << endl;
    }

    output.close();

    contador = testeSalva();

    vector<int> vector_depois_leitura;
    vector_depois_leitura = testeLeitura(contador);

    decoding(vector_depois_leitura, contador);
    auto current_time = chrono::high_resolution_clock::now();

    cout << "O tempo de execu????o foi: " << chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count() << " milisegundos" << endl;
}
