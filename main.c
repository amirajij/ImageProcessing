#include <stdio.h>
#include <stdlib.h>

#define CHANNELS 3
#define NAME_LENGTH 100

int mediaRGB(int r, int g, int b)
{
    int media = (r + g + b) / 3;
    return media;
}

int abre_arquivo(char filename[NAME_LENGTH], unsigned char **image_data, int *width, int *height) {
    FILE *file;
    char magic_number[3];
    int max_color;
    int tamanho = 0;

    file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Não foi possível abrir o arquivo.\n");
        return 0;
    }

    fscanf(file, "%2s\n%d %d\n%d\n", magic_number, width, height, &max_color);
    tamanho = *width * *height;

    *image_data = (unsigned char *) malloc(tamanho * CHANNELS * sizeof(unsigned char));
    if (*image_data == NULL) {
        fprintf(stderr, "Erro ao alocar memória.\n");
        fclose(file);
        return 0;
    }

    fread(*image_data, sizeof(unsigned char), tamanho * CHANNELS, file);

    fclose(file);

    return tamanho;
}

void converte_para_PB(unsigned char *image_data, int width, int height)
{
    int i, j;
    unsigned char r, g, b, media;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            r = image_data[i * width * CHANNELS + j * CHANNELS]; 
            g = image_data[i * width * CHANNELS + j * CHANNELS + 1]; 
            b = image_data[i * width * CHANNELS + j * CHANNELS + 2]; 

            media = mediaRGB(r, g, b);

            image_data[i * width * CHANNELS + j * CHANNELS] = media;
            image_data[i * width * CHANNELS + j * CHANNELS + 1] = media;
            image_data[i * width * CHANNELS + j * CHANNELS + 2] = media;
        }
    }
}

void converte_media(unsigned char *image_data, int width, int height) {
    unsigned char *temp = (unsigned char *)malloc(width * height * CHANNELS * sizeof(unsigned char));
    if (temp == NULL) {
        fprintf(stderr, "Erro ao alocar memória para temp.\n");
        return;
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < CHANNELS; k++) {
                temp[i * width * CHANNELS + j * CHANNELS + k] = image_data[i * width * CHANNELS + j * CHANNELS + k];
            }
        }
    }

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            for (int k = 0; k < CHANNELS; k++) {
                int soma = 0;
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        soma += temp[(i + di) * width * CHANNELS + (j + dj) * CHANNELS + k];
                    }
                }
                image_data[i * width * CHANNELS + j * CHANNELS + k] = soma / 9;
            }
        }
    }

    free(temp);
}

void converte_negativo(unsigned char *image_data, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image_data[i * width * CHANNELS + j * CHANNELS] = 255 - image_data[i * width * CHANNELS + j * CHANNELS]; // Red
            image_data[i * width * CHANNELS + j * CHANNELS + 1] = 255 - image_data[i * width * CHANNELS + j * CHANNELS + 1]; // Green
            image_data[i * width * CHANNELS + j * CHANNELS + 2] = 255 - image_data[i * width * CHANNELS + j * CHANNELS + 2]; // Blue
        }
    }
}

int escreve_arquivo(char filename[NAME_LENGTH], unsigned char *image_data, int width, int height, int tamanho) {
    FILE *file;

    file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Não foi possível criar o arquivo.\n");
        return 0;
    }

    fprintf(file, "P6\n%d %d\n255\n", width, height);
    fwrite(image_data, sizeof(unsigned char), tamanho * CHANNELS, file);

    fclose(file);

    return tamanho;
}

int main()
{
    unsigned char *imagem;
    char filename[NAME_LENGTH], filename2[NAME_LENGTH], filtro;
    int largura, altura, tamanho_imagem;

    printf("Nome do ficheiro in: ");
    scanf("%s", filename);

    printf("Nome do ficheiro out: ");
    scanf("%s", filename2);

    tamanho_imagem = abre_arquivo(filename, &imagem, &largura, &altura);

    if (tamanho_imagem == 0) {
        return 1;
    }

    printf("Qual opcao: ");
    scanf(" %c", &filtro);

    switch (filtro) {
        case 'P':
            converte_para_PB(imagem, largura, altura);
            break;
        case 'M':
            converte_media(imagem, largura, altura);
            break;
        case 'N':
            converte_negativo(imagem, largura, altura);
            break;
        default:
            fprintf(stderr, "Filtro invalido.\n");
            free(imagem);
            return 1;
    }

    escreve_arquivo(filename2, imagem, largura, altura, tamanho_imagem);

    free(imagem);

    return 0;
}
