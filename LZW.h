#ifndef LZW_H
#define LZW_H

class LZW{
    private:
        int dicionarioSize;

    public:
        LZW();
        void lerArquivo();
        void encode();
        void decode();
};

#endif