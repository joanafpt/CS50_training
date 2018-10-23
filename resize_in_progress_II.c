#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage    //JOANA -
    if (argc != 4)
    {
        fprintf(stderr, "Usage: copy infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    //JOANA -
    int n = atoi(argv[1]);

    //JOANA -
    if (n < 1 || n >= 100)

    {
        fprintf(stderr, "Invalid number. Please choose a number between 1 and 100\n");
        return 1; // retorna erro
    }

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    //JOANA - create outfile headers: define valor como copia do BI para depois manipular os valores

    BITMAPINFOHEADER out_bi = bi;
    BITMAPFILEHEADER out_bf = bf;

    out_bi.biWidth = bi.biWidth * n;

    out_bi.biHeight = bi.biHeight * n;

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int out_padding = (4 - (out_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4; //joana

    out_bi.biSizeImage = ((sizeof(RGBTRIPLE) * out_bi.biWidth) + out_padding) * abs(out_bi.biHeight);

    out_bf.bfSize = out_bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&out_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&out_bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    /*
    //Horizontal

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            for (int l = 0; l < n; l++)

            {
                // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);

            }
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < out_padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    */

    //vertical

    // iteração para cada linha
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)

    {
        //iteração para a largura
        for (int j = 0; j < bi.biWidth; j++)

        {

            // declaração RGBTRIPLE temporary storage
            RGBTRIPLE triple;

            //lê cada px uma vez
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            //escreve cada px n vezes
            for (int m = 0; m < n; m++)

            {

                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);

                /*move cursor: largura out - 1 (1 é o pixel q foi escrito * tamanho do rgbtriple
             para converter o pixel em bytes, + padding do outfile*/

                if (m < n - 1) //  para executar o fseek apenas n-1 e depois passar a coluna seguinte

                {
                    fseek(outptr, (out_bi.biWidth -1 ) * sizeof(RGBTRIPLE) + out_padding, SEEK_CUR);
                }
            }
            //aqui ja escreveu os n pixeis do pixel original
            //agora vamos preparar para escrever o px da img original seguinte na nova img

             if (j < (bi.biWidth - 1))
             {
                 fseek(outptr, -((out_bi.biWidth  ) * sizeof(RGBTRIPLE) + out_padding) * (n-1), SEEK_CUR);
             }
        }
        // ja escreveu linhan - vai preparar linha seguinte
              fseek(outptr, ((out_bi.biWidth - bi.biWidth ) * sizeof(RGBTRIPLE) ), SEEK_CUR);



        //
        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        //output image - desloca a largura que falta (imagem out) e coloca padding (preto)
        // fseek(outptr, (out_bi.biWidth - bi.biWidth) * sizeof(RGBTRIPLE), SEEK_CUR);

        // then add it back (to demonstrate how)
       for (int k = 0; k < out_padding; k++)
       {
            fputc(0x00, outptr);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
