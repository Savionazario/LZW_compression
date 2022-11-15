#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>

using namespace std;

vector<int>
encoding(ifstream &input)
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

    input.open("test.txt");

    input >> a;
    cout << "VALOR DO CHAR a: " << a << endl;

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

    ifstream input;

    vector<int> output_code = encoding(input);

    cout << "Output Codes are: ";

    for (int i = 0; i < output_code.size(); i++)
    {
        cout << output_code[i] << " ";
    }
    cout << endl;
}
