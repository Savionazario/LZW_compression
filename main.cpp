#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>

using namespace std;
#define SIZE 4096

unsigned char *buffer = new unsigned char[SIZE];
int pos = 0; // posicao em bits
FILE *output;
const char *name = "output.lzw";

void addBits(int value, int qBits)
{
    int byte = pos / 8;   // posicao do proximo byte com bits disponiveis
    int shift = pos % 8;  // quant de bits utilizados
    int disp = 8 - shift; // quant de bits disponiveis
    unsigned int mask = 0;
    int falta = qBits - disp; // bits que faltam para o proximo byte
    int falta2 = 0;           // bits que faltan para o segundo proximo byte
    int i;
    if (falta > 8)
    {
        falta2 = falta - 8;
        falta = 8;
    }

    // printf("value=%3d::byte=%d, shift=%d, disp=%d, mask=%d, falta=%d, falta2=%d\n", value, byte, shift, disp, mask, falta, falta2);

    mask = 0;
    for (i = 0; i < disp; i++)
        mask = mask | (1 << i);
    buffer[byte] = (buffer[byte] & (~mask & 0xFF)) | ((value >> (falta + falta2)) & mask);
    // printf("mask1=0x%02X (~0x%02X)\n", mask, (~mask & 0xFF));

    if (falta > 0)
    {
        mask = 0;
        for (i = 0; i < (8 - falta); i++)
            mask = mask | (1 << i);
        if (falta2 == 0)
            buffer[byte + 1] = (buffer[byte + 1] & mask) | ((value << (8 - falta)) & (~mask & 0xFF));
        else
            buffer[byte + 1] = ((value >> falta2) & 0xFF);
        // printf("mask2=0x%02X (~0x%02X)\n", mask, (~mask & 0xFF));
    }

    if (falta2 > 0)
    {
        mask = 0;
        for (i = 0; i < (8 - falta2); i++)
            mask = mask | (1 << i);
        buffer[byte + 2] = (buffer[byte + 2] & mask) | ((value << (8 - falta2)) & (~mask & 0xFF));
        // printf("mask3=0x%02X (~0x%02X)\n", mask, (~mask & 0xFF));
    }

    pos += qBits;

    // Adicionado para salvar os bytes as poucos.
    byte = pos / 8; // posicao do proximo byte com bits disponiveis
    if (byte >= SIZE - 3)
    {
        fwrite(buffer, 1, byte, output);
        // printf("salvei %d bytes\n", byte);
        pos -= byte * 8;
        shift = pos % 8; // quant de bits utilizados
        for (i = 0; i < (8 - shift); i++)
            mask = mask | (1 << i);
        buffer[0] = buffer[byte] & ~mask;
    }
}

int getBits(int qBits)
{
    int value = 0;
    int byte = pos / 8;   // posicao do proximo byte com bits disponiveis
    int shift = pos % 8;  // quant de bits utilizados
    int disp = 8 - shift; // quant de bits disponiveis
    unsigned int mask = 0;
    int falta = qBits - disp; // bits que faltam para o proximo byte
    int falta2 = 0;           // bits que faltan para o segundo proximo byte
    int i;
    if (falta > 8)
    {
        falta2 = falta - 8;
        falta = 8;
    }

    // printf("value=%3d::byte=%d, shift=%d, disp=%d, mask=%d, falta=%d, falta2=%d\n", value, byte, shift, disp, mask, falta, falta2);

    mask = 0;
    for (i = 0; i < disp; i++)
        mask = mask | (1 << i);
    value = (buffer[byte] & mask) << (falta + falta2);
    // printf("mask1=0x%02X (~0x%02X)\n", mask, (~mask & 0xFF));

    if (falta > 0)
    {
        if (falta2 == 0)
            value = value | ((buffer[byte + 1]) >> (8 - falta));
        else
            value = value | (buffer[byte + 1] << falta2);
        // printf("mask2=0x%02X (~0x%02X)\n", mask, (~mask & 0xFF));
    }

    if (falta2 > 0)
    {
        // mask = 0;
        // for (i = 0 ; i < (8 - falta2) ; i++)
        //     mask = mask | (1 << i);
        value = value | (buffer[byte + 2] >> (8 - falta2));
        // printf("mask3=0x%02X (~0x%02X)\n", mask, (~mask & 0xFF));
    }

    pos += qBits;

    return value;
}

void salvaFim()
{
    int byte = pos / 8; // posicao do proximo byte com bits disponiveis
    int shift = pos % 8;
    if (shift > 0)
        byte++;
    if (byte > 0)
    {
        fwrite(buffer, 1, byte, output);
        // printf("salvei fim %d bytes\n", byte);
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
    int i, array_dicionario[500] = {0}, contador = 0; // MUDAR PRA VECTOR
    int qBits = 0;
    int value = 0;
    int j = 0;
    qBits = 16;

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
            array_dicionario[contador] = value;
            // cout << array_dicionario[contador] << " ";
            contador++;
            // addBits(value, qBits);
        }
    }
    // cout << "\n";
    input.close();

    /*for (int k = 0; k < contador - 1; k++)
    {
        cout << array_dicionario[k] << " ";
    }*/

    // cout << " ------------------ DICIONARIO ANTES DA DECODIFICAÇAO ------------------------------" << endl;
    // cout << "\n";s
    // cout << "\n";
    output = fopen(name, "wb");
    if (output <= 0)
    {
        printf("Erro abrindo o arquivo %s\n", name);
        return 0;
    }

    for (int k = 0; k < contador - 1; k++)
    {
        addBits(array_dicionario[k], qBits);
    }

    // addBits(10, qBits);
    // addBits(8, qBits);
    // addBits(127, qBits);
    // addBits(32, qBits);
    // addBits(64, qBits);
    // addBits(255, qBits);
    // addBits(20, qBits);
    // addBits(3, qBits);
    //  Sa'ida esperada:
    //  0x05 0x02 0x0F 0xE2 0x02 0x03 0xFC 0x28 0x03

    /*qBits = 10;
    addBits(10,  qBits);
    addBits(8,   qBits);
    addBits(127, qBits);
    addBits(32,  qBits);
    addBits(64,  qBits);
    addBits(255, qBits);
    addBits(20,  qBits);
    addBits(3,   qBits);
    addBits(79,  qBits);
    addBits(1020,qBits);
    addBits(523, qBits);*/
    // Sa'ida esperada:
    // 0x02 0x80 0x81 0xFC 0x20 0x10 0x0F 0xF0 0x50 0x03 0x13 0xFF 0xC8 0x2C

    salvaFim();
    // printf("arquivo lido: \n");
    // lerArquivo(name);
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
        // return;
    }
    fseek(output, 0, SEEK_END);
    tam = ftell(output);
    fseek(output, 0, SEEK_SET);
    // printf("tamanho do arquivo: %d\n", tam);
    if (buffer)
        delete buffer;
    buffer = new unsigned char[tam];
    fread(buffer, 1, tam, output);

    vector<int> inteiros_decodificados;
    // i = 0;
    qBits = 16;

    for (i = 0; i < tam; i++)
    {
        // cout << i;
        inteiros_decodificados.push_back(getBits(qBits));
        // cout << i;
    }

    // printf("teste leitura:\n");
    int j;
    /*for (j = 0; j < contador; j++)
    {
        cout << inteiros_decodificados[j] << endl;
    }*/

    printf("\n");
    return inteiros_decodificados;
}

template <typename K, typename V>
void print_map(std::unordered_map<K, V> const &m)
{
    for (auto const &pair : m)
    {
        std::cout << "{" << pair.first << ": " << pair.second << "}\n";
    }
}

vector<int> encoding(ifstream &input)
{
    unordered_map<string, int> table;
    vector<int> output_code;
    int code = 256;
    char a;

    cout << "Encoding\n";

    for (int i = 0; i <= 255; i++)
    {
        string ch = "";
        ch += char(i);
        table[ch] = i;
    }

    string p = "", c = "";

    input.open("aa.txt");

    input >> a;
    // cout << "VALOR DO CHAR a: " << a << endl;

    p += a;
    char b;
    if (!input.good())
    {
        input.close();
    }
    else
    {
        while (!input.eof())
        {
            // input >> b;
            // cout << "VALOR DO CHAR p: " << p << endl;
            // c += b;
            // cout << "VALOR DO CHAR c: " << c << endl;
            input >> b;
            c += b;

            if (input.eof())
            {
                c = "";
            }

            if (table.find(p + c) != table.end())
            {
                p = p + c;
            }
            else
            {
                cout << p << "\t" << table[p] << "\t\t";
                cout << p + c << "\t" << code << endl;
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
        input.close();
    }
    // cout << p << "\t" << table[p] << endl;
    output_code.push_back(table[p]);
    // print_map(table);
    return output_code;
}

void decoding(vector<int> op, int contador)
{
    cout << "\nDecoding\n";
    unordered_map<int, string> table;
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
    cout << s;
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
        cout << s;
        c = "";
        c += s[0];
        table[count] = table[old] + c;
        count++;
        old = n;
    }
    // print_map(table);
}

int main()
{
    int contador = 0;
    setlocale(LC_ALL, "");

    ifstream input;
    vector<int> output_code = encoding(input);
    vector<int> inteiros_decodificados;

    // cout << "Output Codes are: ";

    for (int i = 0; i < output_code.size(); i++)
    {
        cout << output_code[i] << " ";
    }

    // decoding(output_code);

    ofstream output;

    output.open("dicionario.txt");

    for (int i : output_code)
    {
        output << i << endl;
    }

    output.close();

    contador = testeSalva();
    inteiros_decodificados = testeLeitura(contador);
    decoding(inteiros_decodificados, contador);
}
