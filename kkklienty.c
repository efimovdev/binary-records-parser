
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
    uint64_t id;
    char     imya[32];
    char     telefon[32];
    uint8_t  region;
    uint8_t  rezerv;
    uint8_t  vozrast;
} Klient;
#pragma pack(pop)

uint8_t chitat_versiyu(FILE *f)
{
    uint8_t versiya = 0;
    fread(&versiya, sizeof(versiya), 1, f);
    printf("Versiya fayla: %u\n", versiya);
    return versiya;
}

Klient *chitat_klientov(FILE *f, uint64_t *count)
{
    fread(count, sizeof(*count), 1, f);

    Klient *klienty = malloc((*count) * sizeof(Klient));
    if (klienty == NULL) {
        printf("Ne khvatilo pamyati\n");
        exit(1);
    }

    fread(klienty, sizeof(Klient), *count, f);
    return klienty;
}


int sravnit_po_vozrastu(const void *a, const void *b)
{
    const Klient *k1 = a;
    const Klient *k2 = b;
    return (int)k1->vozrast - (int)k2->vozrast;
}

void vyvesti_klienta(const Klient *k)
{
    printf("id=%-3llu  imya=%-10.32s  telefon=%-14.32s  region=%-3u  vozrast=%u\n",
           (unsigned long long)k->id, k->imya, k->telefon, k->region, k->vozrast);
}


void vyvesti_klientov(const Klient *klienty, uint64_t count)
{
    for (uint64_t i = 0; i < count; i++) {
        vyvesti_klienta(&klienty[i]);
    }
}


void poisk_po_vozrastu(const Klient *klienty, uint64_t count, int vozrast)
{
    int naydeno = 0;
    for (uint64_t i = 0; i < count; i++) {
        if (klienty[i].vozrast == vozrast) {
            vyvesti_klienta(&klienty[i]);
            naydeno = 1;
        }
    }
    if (!naydeno) {
        printf("Klienty s takim vozrastom ne naydeny\n");
    }
}


void poisk_po_regionu(const Klient *klienty, uint64_t count, int region)
{
    int naydeno = 0;
    for (uint64_t i = 0; i < count; i++) {
        if (klienty[i].region == region) {
            vyvesti_klienta(&klienty[i]);
            naydeno = 1;
        }
    }
    if (!naydeno) {
        printf("Klienty s takim regionom ne naydeny\n");
    }
}

void interaktivny_rezhim(const Klient *klienty, uint64_t count)
{
    int vybor;
    int znachenie;

    while (1) {
        printf("\n--- Poisk klientov ---\n");
        printf("1 - poisk po vozrastu\n");
        printf("2 - poisk po regionu\n");
        printf("0 - vykhod\n");
        printf("Vybor: ");

        if (scanf("%d", &vybor) != 1) {
            break;
        }

        if (vybor == 0) {
            break;
        } else if (vybor == 1) {
            printf("Vvedite vozrast: ");
            scanf("%d", &znachenie);
            poisk_po_vozrastu(klienty, count, znachenie);
        } else if (vybor == 2) {
            printf("Vvedite region: ");
            scanf("%d", &znachenie);
            poisk_po_regionu(klienty, count, znachenie);
        } else {
            printf("Neizvestny punkt menyu\n");
        }
    }
}

int main(int argc, char *argv[])
{
    const char *put = (argc > 1) ? argv[1] : "data.bin";

    FILE *f = fopen(put, "rb");
    if (f == NULL) {
        printf("Ne udalos' otkryt' fayl %s\n", put);
        return 1;
    }

    chitat_versiyu(f);

    uint64_t count = 0;
    Klient *klienty = chitat_klientov(f, &count);
    fclose(f);

    printf("\nKlienty (otsortirovany ot mladshego k starshemu):\n");
    qsort(klienty, count, sizeof(Klient), sravnit_po_vozrastu);
    vyvesti_klientov(klienty, count);

    interaktivny_rezhim(klienty, count);

    free(klienty);
    return 0;
}
