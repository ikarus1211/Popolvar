
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Struktura reprezentujuca vrchol grafu
 * Do tejto strukturi ukladam informacie o vrchole ktory prave spracuvavam a nasledovne
 * ho ulozim do svojej vlastnej mapy aby som vedel kde som uz bol a aby som sa na koci vedel vratit
 * spat a tak poskladat celu cestu
 *
 */
typedef struct vortex
{
    char info;                      // informacia ci som dany vrcgol navstivil
    short x,y;                      // suradnice kde sa nachadza vrchol v mape
    short parrentX,parrentY,parrentDim; // suradnice a dimenzia predosleho vrcholu
    int value;                      // dlzka cesty
    char drak;                      // premenna drak sluzi na ulozenie typu policka
    short dim;                      // dimenzia v ktorej sa nachadzam
}VOR;


VOR minHeap[100000];                // minimalna halda na vyberanie vrcholu s najlacnejiou cestou
int heapCount = 0;                  // pocet prvkov v minHeape
VOR my_map[1000][1000][33];         // Moja vlastna mapa do ktorej sa pridavaju vrcholi slizi mi ako referencia
int road_len;                       // dlzka cesty
short princes_adres[6];             // adresy vsetkych princezien
short teleports[10][201];           // adresy vsetkych teleportov s tym ze na poslednom poli je pocet teleportov
// daneho typu


/*
 * Doplnkova funkcia k minHeap ktora pri vkladani prvku prebubla vlozeny prvok na jeho spravne miesto
 */
void heapify_BtoT(int index)
{
    int parent = (index-1)/2;

    if (minHeap[parent].value > minHeap[index].value)
    {
        VOR temp = minHeap[parent];
        minHeap[parent] = minHeap[index];
        minHeap[index] = temp;
        if (parent <= 0)
            return;
        heapify_BtoT(parent);
    }

}
/*
 * Funkcia ktora jednoducho vymeni dva vrcholy v minheape
 */
void swap(int index, int jeidex)
{
    VOR temp = minHeap[index];
    minHeap[index] = minHeap[jeidex];
    minHeap[jeidex] = temp;
}
/*
 * Podobna funkcia ako heapify_BtoT s tym rozdielom ze to robi naopak
 * a to z vrchu na dol toto sa pouziva pri vyberani prvku z heapu
 * kedy sa na vrch dostane väčši prvok nez tam ma byt
 */
void heapify_TtoB(int index)
{


    if (minHeap[index * 2 + 1].value != 0 && minHeap[index].value > minHeap[index * 2 + 1].value && minHeap[index].value <= minHeap[index * 2 + 2].value)
    {
        swap(index, index*2+1);
        heapify_TtoB(index*2+1);
    }
    if (minHeap[index * 2 + 2].value != 0 && minHeap[index].value <= minHeap[index * 2 + 1].value && minHeap[index].value > minHeap[index * 2 + 2].value)
    {
        swap(index, index*2+2);
        heapify_TtoB(index*2+2);
    }
    if (minHeap[index * 2 + 1].value != 0 && minHeap[index * 2 + 2].value != 0 && minHeap[index].value > minHeap[index * 2 + 1].value && minHeap[index].value > minHeap[index * 2 + 2].value)
    {
        if (minHeap[index * 2 + 1].value < minHeap[index * 2 + 2].value ) {
            swap(index, index * 2 + 1);
            heapify_TtoB(index * 2 + 1);
        }
        else {
            swap(index, index * 2 + 2);
            heapify_TtoB(index*2+2);
        }
    }
}
/*
 * Najdenie a nasledny vyber vrchola z minimalnej haldy
 */
VOR minHeap_find()
{
    VOR retvalue = minHeap[0];
    minHeap[0] = minHeap[heapCount - 1];
    VOR temp;
    temp.y=0;
    temp.info = '0';
    temp.value = 0;
    temp.x = 0;
    temp.parrentX = 0;
    temp.parrentY = 0;
    minHeap[heapCount - 1] = temp;
    heapCount--;
    heapify_TtoB(0);
    return  retvalue;
}
/*
 * Vkladanie do minimalnej haldy
 */
void minHeap_insert(VOR isertable)
{
    minHeap[heapCount] = isertable;
    heapify_BtoT(heapCount);
    heapCount++;
}
//#####################################################
// Zaciatok samotneho Dijkstra
//#####################################################

/*
 * Funkcia dostane ako argument  preskumavany vrchol, mapu, a suradnice jedneho suseda
 *
 * Pozrie sa pomocou mapu na suseda a vyhodnoti o ake policko ide.
 * Podla toho pripocita k dlzke cesty a prida novy vrchol(suseda) do minHeapu
 *
 */
void check_side(char **map, short x, short y, VOR temp)
{
    VOR new1;
    new1.parrentY = temp.y;
    new1.parrentX = temp.x;
    new1.parrentDim = temp.dim;
    new1.x = x;
    new1.y = y;
    new1.value = temp.value;
    new1.dim = temp.dim;
    new1.drak = '\0';
    int len = temp.value;
    if (map[y][x] == 'G')
    {
        len += 1;
        new1.drak = 'G';
    }
    if ((map[y][x] - 48) >= 0 && (map[y][x] - 48) <= 9)  {
        len += 1;
        new1.drak = map[y][x];
    }
    if (map[y][x] == 'C') {
        len += 1;
    }
    if (map[y][x] == 'H') {
        len += 2;
    }
    if (map[y][x] == 'N') {
        return;
    }
    if (map[y][x] == 'D') {
        len = len+1;
        new1.drak = 'D';
    }
    if (map[y][x] == 'P') {
        new1.drak = 'P';
        len += 1;
    }
    if ((my_map[y][x][temp.dim].info == '1' && my_map[y][x][temp.dim].value > len) || my_map[y][x][temp.dim].info != '1'  ) {
        new1.value = len;
        minHeap_insert(new1);
    }
}
/*
 * Funckia ktora spusti prehladavanie do kazdej strany daneho vrcholu
 *
 */
void look_around(char **map, VOR node, int height, int width)
{
    short currentX = node.x;
    short  currentY = node.y;

    if (currentX + 1 < width)
    {
        check_side(map,currentX + 1, currentY, node);
    }
    if (currentX - 1 >= 0)
    {
        check_side(map,currentX - 1, currentY, node);
    }
    if (currentY + 1 < height)
    {
        check_side(map, currentX, currentY + 1, node);
    }
    if (currentY - 1 >= 0)
    {
        check_side(map, currentX, currentY - 1, node);
    }
}
/*
 * Inicializacna funkcia ktora na zaciatok vlozi do minHeapu prvy prvok (start)
 * zo suradnicami [0, 0]
 *
 */
void start_init()
{
    VOR start;
    start.x = 0;
    start.y = 0;
    start.value=0;
    start.parrentX = 0;
    start.parrentY = 0;
    start.dim = 0;
    start.parrentDim = 0;
    start.drak = '/0';
    start.info = '0';
    minHeap_insert(start);
}
/*
 *  Tato funkcia dostane konecny prvok ako argument + pocet vrcholov cez ktore som presiel (navratova hodnota)
 *  Funkcia zacne od posledneho vrcholu skaldat cestu naspat ku startu a to za pomoci udajov ulozenych v mojej mape
 *  a udajov v strukture
 *
 *  Druhou castou Funckue je ze cestu otoci od zaciatku do konca aby odpovedala spravnemu navratovemu stringu
 */
int* back_track(VOR node, int* cnt)
{
    int *road;
    road = calloc(node.value*4+2, sizeof(int));
    int temp = node.value;
    int counter = 2;
    for (int i = 0; i <temp*4; i += 2)
    {
        *(road+i) = node.x;
        counter++;
        *(road+i+1) = node.y;
        counter++;
        if (node.x == 0 && node.y == 0 && node.dim == 0) {
            *(road + i) = node.x;
            *(road + i + 1) = node.y;

            break;
        }

        node = my_map[node.parrentY][node.parrentX][node.parrentDim];

    }
    int *ret_str;
    counter -= 2;
    ret_str = calloc(sizeof(int), counter);
    for (int i = 0; i < counter; ++i) {
        *(ret_str + counter - i -2) = *(road+i);
        i++;
        *(ret_str + counter - i) = *(road+i);
    }
    *cnt = counter/2;
    return  ret_str;
}
/*
 * Funkcia ktora sa spusti pred prehladavanim
 *
 * Ulozi suradnice princezien a teleportov na neskorsie pouzitie
 *
*/
void search_map(char **map, int height, int width)
{
    int iter = 0;
    int iter2[10];
    for (int i = 0; i < 10; ++i) {
        iter2[i] = 0;
    }
    for(int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; ++x) {
            if (map[y][x] == 'P') {                      // Pocitanie a ukladanie princezien
                princes_adres[iter] = (short)x;
                iter++;
                princes_adres[iter] = (short)y;
                iter++;
            }
            int cmp = map[y][x] - 48;
            if (cmp >= 0 && cmp <= 9)
            {                                           // Pocitanie a ukladanie teleoortov
                teleports[cmp][iter2[cmp]] = x;
                iter2[cmp]++;
                teleports[cmp][iter2[cmp]] = y;
                iter2[cmp]++;
                teleports[cmp][200]++;
            }
        }
    }
}
/*
 * Ako argument dostane vrchol s princeznou a vrati cislo dimenzie ku ktorej dana princezna
 * prisluhuje
 *
 * Taktiez je osetrene ak sa druhy krat vratim v inej dimenzii na tu istu priznceznu tak ju
 * ju tam druhy krat nezaratam
 */
short princes_cmp(VOR node)
{
    short x = node.x;
    short y = node.y;
    short dim = node.dim;
    if ((node.dim & 1) == 1){
        // Prva
        if (x == princes_adres[0] && y == princes_adres[1] && (dim & 4) == 0)
            return dim + 4 ;
        // Druha
        if (x == princes_adres[2] && y == princes_adres[3] && (dim & 8) == 0)
            return dim + 8;
        //tretia
        if (x == princes_adres[4] && y == princes_adres[5] && (dim & 16) == 0)
            return dim + 16;
    }
    return node.dim;
}

/*
 * Riadiaca funkcia celeho Dijkstru ktora vybera prvky s minimalnou cestou a preskumavva ich
 *
 */
int* zachran_princezne(char **map, int height, int width, int time, int *roadlen)
{

    start_init();
    int *road;
    search_map(map,height,width);
    if (heapCount == 0){
        printf("Heap is empty\n");
    } else {
        int iteratr = 0;
        while (1) {

            VOR node = minHeap_find();                                      // Vyber z min haldi
            if (node.drak == 'G' && (node.dim & 2) == 0)                    // Kotrola ci sa jedna o generator
            {
                node.dim  += 2;
            }
            if ((node.drak == 'D' && node.dim == 0) || (node.drak == 'D' && node.dim == 2) ) // Kotrola ci sa jedna draka
            {
                printf("Dragon found");
                node.dim += 1;
            }
            if (node.drak == 'P' && (node.dim & 1) == 1)                    // Kotrola ci sa jedna princezne
            {
                node.dim = princes_cmp(node);
                road_len = node.value;
                printf("Princes found");

            }
            if ((node.dim & 29) == 29 && node.drak == 'P')                  // Kotrola ci som v konecnej dimenzii
            {                                                               // a teda ci mozem ukoncit vyhladavanie
                printf("Ende\n");
                road_len = node.value;
                int *ptr = calloc(road_len+2, sizeof(int));
                int size;
                return back_track(node,roadlen);

            }
            if ((my_map[node.y][node.x][node.dim].info == '1' && my_map[node.y][node.x][node.dim].value > node.value) || my_map[node.y][node.x][node.dim].info != '1')
            {
                if ((node.drak - 48) >= 0 && (node.drak - 48) <= 9 && (node.dim & 2) == 2) // Ak sa jedna o teleport
                {                                                           // Tak pridaj do heapu vsety vrcholi kde sa
                    VOR temp;                                               // mozem teleportnut
                    temp.drak = node.drak;                                  // a terajsi vrchol prehladaj dookola
                    temp.dim = node.dim;
                    temp.value = node.value;
                    temp.parrentX = node.x;
                    temp.parrentY = node.y;
                    temp.parrentDim = node.parrentDim;
                    for (int i = 0; i < teleports[node.drak-48][200]*2; i++) {

                        temp.x = teleports[node.drak-48][i];
                        i++;
                        temp.y = teleports[node.drak-48][i];
                        if (node.x == temp.x && node.y == temp.y)
                        {
                            my_map[node.y][node.x][node.dim] = node;
                            look_around(map, node, height, width);
                            my_map[node.y][node.x][node.dim].info = '1';
                        } else
                            minHeap_insert(temp);

                    }
                } else {                                                    // Ak sa nejedna o teleport
                    my_map[node.y][node.x][node.dim] = node;                // prehladaj dookola
                    look_around(map, node, height, width);
                    my_map[node.y][node.x][node.dim].info = '1';
                }
            }
            iteratr++;
        }
    }
    return road;
}

int main() {
    char  mapa[7][7]={  {'C','C','C','H','H','C','H'},
                        {'C','C','H','H','H','C','H'},
                        {'H','C','D','C','H','H','C'},
                        {'H','H','H','C','H','H','C'},
                        {'H','C','H','H','C','C','C'},
                        {'H','C','H','H','P','H','H'},
                        {'P','C','H','C','H','H','P'},};




    int road;
    char **lepsie;
    lepsie = (char**) malloc(7* sizeof(char*));
    for (int i = 0; i < 7; ++i) {
        lepsie[i] = (char*)malloc(7* sizeof(char));
    }
    for (int j = 0; j < 7; ++j) {
        for (int i = 0; i < 7; ++i) {
            lepsie[j][i] = mapa[j][i];
        }
    }
    int *fin_road;
    fin_road = calloc(road_len+1, sizeof(int));

    fin_road = zachran_princezne(lepsie,7,7,158, &road);
    for (int i = 0; i < road*2; i++)
    {
        if (i % 2 == 1)
            printf(" %d\n",fin_road[i]);
        else
            printf("%d",fin_road[i]);
    }
    printf("%d",road);

    return 0;
}