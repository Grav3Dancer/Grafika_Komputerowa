/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacją osi 
//  układu współrzednych

/*************************************************************************************/

#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <gl/gl.h>
#include <gl/glut.h>
#define _USE_MATH_DEFINES
#include <math.h>


typedef float point3[3];

/*************************************************************************************/

int rozmiar;

double u, v;

static GLfloat theta[] = { 0.0, 0.0, 0.0 }; // obroty dla dużego jajka
static GLfloat thetaMale = 0.0; // obrót małego jajka

int model = 1; // 1- punkty, 2- siatka, 3 - wypełnione trójkąty, dla dużego jajka
int modelMale = 1; // dla małego jajka
int obrot = 0; // 1 - wokół osi X, 2 - wokół osi Y, 0 - brak obrotu

struct point // zmienna pojedynczego punktu
{
    double x, y, z;
};

//macierze dla duzego jajka
point macierz[500][500];
point kolory[500][500];

//macierze dla malego jajka
point macierzMale[300][300];
point koloryMale[300][300];

//generowanie tablicy duzego jajka
void generujTabliceJajo()
{
    for (int i = 0; i < rozmiar; i++)
    {
        for (int j = 0; j < rozmiar; j++)
        {
            u = (double)i / (rozmiar - 1 );
            v = (double)j / (rozmiar - 1 );
            macierz[i][j].x = (-(90 * pow(u, 5)) + (225 * pow(u, 4)) - (270 * pow(u, 3)) + (180 * pow(u, 2)) - 45 * u) * cos(M_PI * v);
            macierz[i][j].y = (160 * pow(u, 4)) - (320 * pow(u, 3)) + (160 * pow(u, 2));
            macierz[i][j].z = (-(90 * pow(u, 5)) + (225 * pow(u, 4)) - (270 * pow(u, 3)) + (180 * pow(u, 2)) - 45 * u) * sin(M_PI * v);

        }
    }
}

//generowanie tablicy malego jajka
void generujTabliceMaleJajo()
{
    for (int i = 0; i < rozmiar/2; i++)
    {
        for (int j = 0; j < rozmiar/2; j++)
        {
            u = (double)i / (rozmiar/2 - 1);
            v = (double)j / (rozmiar/2 - 1);
            macierzMale[i][j].x = ( (-(90 * pow(u, 5)) + (225 * pow(u, 4)) - (270 * pow(u, 3)) + (180 * pow(u, 2)) - 45 * u) * cos(M_PI * v) )/4.0f + 5.0f ;
            macierzMale[i][j].y = ( (160 * pow(u, 4)) - (320 * pow(u, 3)) + (160 * pow(u, 2)) ) /4.0f + 5.0f;
            macierzMale[i][j].z = ( (-(90 * pow(u, 5)) + (225 * pow(u, 4)) - (270 * pow(u, 3)) + (180 * pow(u, 2)) - 45 * u) * sin(M_PI * v) )/4.0f ;

        }
    }
}

// generowanie kolorow dla duzego jajka
void generujKolory()
{
    for (int i = 0; i < rozmiar; i++)
    {
        for (int j = 0; j < rozmiar; j++)
        {
            kolory[i][j].x = (double)rand() / RAND_MAX;
            kolory[i][j].y = (double)rand() / RAND_MAX;
            kolory[i][j].z = (double)rand() / RAND_MAX;
        }
    }
}

// generowanie kolorow dla malego jajka
void generujKoloryMale()
{
    for (int i = 0; i < rozmiar/2; i++)
    {
        for (int j = 0; j < rozmiar/2; j++)
        {
            koloryMale[i][j].x = (double)rand() / RAND_MAX;
            koloryMale[i][j].y = (double)rand() / RAND_MAX;
            koloryMale[i][j].z = (double)rand() / RAND_MAX;
        }
    }
}

//funkcja rysujaca duże jajko
void rysujJajo()
{
    for (int i = 0; i < rozmiar - 1; i++)
    {
        for (int j = 0; j < rozmiar - 1; j++)
        {   
            if (model == 1) //punkty
            {
                glBegin(GL_POINTS);
                glVertex3f((float)macierz[i][j].x, (float)macierz[i][j].y - 5.0f, (float)macierz[i][j].z);

                glEnd();
            }
            else if (model == 2) //linie
            {
                glBegin(GL_LINES);
                glVertex3f((float)macierz[i][j].x, (float)macierz[i][j].y - 5.0f, (float)macierz[i][j].z);
                glVertex3f((float)macierz[(i + 1)][j].x, (float)macierz[(i + 1)][j].y - 5.0f, (float)macierz[(i + 1)][j].z);

                glVertex3f((float)macierz[i][j].x, (float)macierz[i][j].y - 5.0f, (float)macierz[i][j].z);
                glVertex3f((float)macierz[i][(j + 1)].x, (float)macierz[i][(j + 1)].y - 5.0f, (float)macierz[i][(j + 1)].z);

                glEnd();
            }
            else if (model == 3) //trójkąty
            {
                glBegin(GL_TRIANGLES);
                glColor3f(kolory[i][j].x, kolory[i][j].y, kolory[i][j].z);

                glVertex3f((float)macierz[i][j].x, (float)macierz[i][j].y - 5.0f, (float)macierz[i][j].z); 
                glVertex3f((float)macierz[(i + 1)][(j + 1)].x, (float)macierz[(i + 1)][(j + 1)].y - 5.0f, (float)macierz[(i + 1)][(j + 1)].z);
                glVertex3f((float)macierz[(i + 1)][j].x, (float)macierz[(i + 1)][j].y - 5.0f, (float)macierz[(i + 1)][j].z);

                glColor3f(kolory[i+1][j+1].x, kolory[i+1][j+1].y, kolory[i+1][j+1].z);
                glVertex3f((float)macierz[i][j].x, (float)macierz[i][j].y - 5.0f, (float)macierz[i][j].z);
                glVertex3f((float)macierz[(i + 1)][(j + 1)].x, (float)macierz[(i + 1)][(j + 1)].y - 5.0f, (float)macierz[(i + 1)][(j + 1)].z);
                glVertex3f((float)macierz[i][(j + 1)].x, (float)macierz[i][(j + 1)].y - 5.0f, (float)macierz[i][(j + 1)].z);

                glEnd();
            }
        }
    }
}

//funkcja rysujaca male jajko
void rysujMaleJajo()
{
    for (int i = 0; i < rozmiar/2 - 1; i++)
    {
        for (int j = 0; j < rozmiar/2 - 1; j++)
        {
            if (modelMale == 1) //punkty
            {
                glBegin(GL_POINTS);
                glVertex3f((float)macierzMale[i][j].x, (float)macierzMale[i][j].y - 5.0f, (float)macierzMale[i][j].z);

                glEnd();
            }
            else if (modelMale == 2) //linie
            {
                glBegin(GL_LINES);
                glVertex3f((float)macierzMale[i][j].x, (float)macierzMale[i][j].y - 5.0f, (float)macierzMale[i][j].z);
                glVertex3f((float)macierzMale[(i + 1)][j].x, (float)macierzMale[(i + 1)][j].y - 5.0f, (float)macierzMale[(i + 1)][j].z);

                glVertex3f((float)macierzMale[i][j].x, (float)macierzMale[i][j].y - 5.0f, (float)macierzMale[i][j].z);
                glVertex3f((float)macierzMale[i][(j + 1)].x, (float)macierzMale[i][(j + 1)].y - 5.0f, (float)macierzMale[i][(j + 1)].z);

                glEnd();
            }
            else if (modelMale == 3) //trójkąty
            {
                glBegin(GL_TRIANGLES);
                glColor3f(koloryMale[i][j].x, koloryMale[i][j].y, koloryMale[i][j].z);

                glVertex3f((float)macierzMale[i][j].x, (float)macierzMale[i][j].y - 5.0f, (float)macierzMale[i][j].z);
                glVertex3f((float)macierzMale[(i + 1)][(j + 1)].x, (float)macierzMale[(i + 1)][(j + 1)].y - 5.0f, (float)macierzMale[(i + 1)][(j + 1)].z);
                glVertex3f((float)macierzMale[(i + 1)][j].x, (float)macierzMale[(i + 1)][j].y - 5.0f, (float)macierzMale[(i + 1)][j].z);

                glColor3f(koloryMale[i + 1][j + 1].x, koloryMale[i + 1][j + 1].y, koloryMale[i + 1][j + 1].z);
                glVertex3f((float)macierzMale[i][j].x, (float)macierzMale[i][j].y - 5.0f, (float)macierzMale[i][j].z);
                glVertex3f((float)macierzMale[(i + 1)][(j + 1)].x, (float)macierzMale[(i + 1)][(j + 1)].y - 5.0f, (float)macierzMale[(i + 1)][(j + 1)].z);
                glVertex3f((float)macierzMale[i][(j + 1)].x, (float)macierzMale[i][(j + 1)].y - 5.0f, (float)macierzMale[i][(j + 1)].z);

                glEnd();
            }
        }
    }
}

//funkcja obrotów
void spinEgg()
{
    thetaMale -= 0.5;
    if (thetaMale > 360.0) thetaMale -= 360.0;

    if (obrot == 1)
    {
        theta[0] -= 0.5;
        if (theta[0] > 360.0) theta[0] -= 360.0;
    }

    else if (obrot == 2)
    {
        theta[1] -= 0.5;
        if (theta[1] > 360.0) theta[1] -= 360.0;
    }
    else
    { }

    glutPostRedisplay(); //odświeżenie zawartości aktualnego okna
}



// Funkcja rysująca osie układu współrzędnych
void Axes(void)
{

    point3  x_min = { -5.0, 0.0, 0.0 };
    point3  x_max = { 5.0, 0.0, 0.0 };
    // początek i koniec obrazu osi x

    point3  y_min = { 0.0, -5.0, 0.0 };
    point3  y_max = { 0.0,  5.0, 0.0 };
    // początek i koniec obrazu osi y

    point3  z_min = { 0.0, 0.0, -5.0 };
    point3  z_max = { 0.0, 0.0,  5.0 };
    //  początek i koniec obrazu osi y
    glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
    glBegin(GL_LINES); // rysowanie osi x
    glVertex3fv(x_min);
    glVertex3fv(x_max);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
    glBegin(GL_LINES);  // rysowanie osi y

    glVertex3fv(y_min);
    glVertex3fv(y_max);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
    glBegin(GL_LINES); // rysowanie osi z

    glVertex3fv(z_min);
    glVertex3fv(z_max);
    glEnd();

}

/*************************************************************************************/

// Funkcja określająca co ma być rysowane (zawsze wywoływana gdy trzeba
// przerysować scenę)



void RenderScene(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Czyszczenie okna aktualnym kolorem czyszczącym

    glLoadIdentity();
    // Czyszczenie macierzy bieżącej
    Axes();
    // Narysowanie osi przy pomocy funkcji zdefiniowanej wyżej

    glColor3f(1.0f, 1.0f, 1.0f); // Ustawienie koloru rysowania na biały

    
    glRotatef(theta[0], 1.0, 0.0, 0.0);

    glRotatef(theta[1], 0.0, 1.0, 0.0);

 //   glRotatef(theta[2], 0.0, 0.0, 1.0);

    rysujJajo();
    
    glPushMatrix(); // duże jajko pomijane przy obracaniu małego

    glRotatef(thetaMale, 0.0, 1.0, 0.0);

    rysujMaleJajo();

    glFlush();
    // Przekazanie poleceń rysujących do wykonania

    glutSwapBuffers();
}

void keys(unsigned char key, int x, int y)
{
    //składowe dużego jajka
    if (key == 'p') model = 1;
    if (key == 'l') model = 2;
    if (key == 't') model = 3;

    //składowe małego jajka
    if (key == 'q') modelMale = 1;
    if (key == 'w') modelMale = 2;
    if (key == 'e') modelMale = 3;

    //obroty dużego jajka
    if (key == '1') obrot = 1;
    if (key == '2') obrot = 2;
    if (key == '0') obrot = 0;

    RenderScene(); // przerysowanie obrazu sceny
}

/*************************************************************************************/

// Funkcja ustalająca stan renderowania



void MyInit(void)
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Kolor czyszcący (wypełnienia okna) ustawiono na czarny

}

/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są
// przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

    GLfloat AspectRatio;
    // Deklaracja zmiennej AspectRatio  określającej proporcję
    // wymiarów okna 
    if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
        vertical = 1;
    glViewport(0, 0, horizontal, vertical);
    // Ustawienie wielkościokna okna widoku (viewport)
    // W tym przypadku od (0,0) do (horizontal, vertical)  
    glMatrixMode(GL_PROJECTION);
    // Przełączenie macierzy bieżącej na macierz projekcji 
    glLoadIdentity();
    // Czyszcznie macierzy bieżącej            
    AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    // Wyznaczenie współczynnika  proporcji okna
    // Gdy okno nie jest kwadratem wymagane jest określenie tak zwanej
    // przestrzeni ograniczającej pozwalającej zachować właściwe
    // proporcje rysowanego obiektu.
    // Do okreslenia przestrzeni ograniczjącej służy funkcja
    // glOrtho(...)            
    if (horizontal <= vertical)

        glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
    else

        glOrtho(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5, 10.0, -10.0);
    glMatrixMode(GL_MODELVIEW);
    // Przełączenie macierzy bieżącej na macierz widoku modelu                                   

    glLoadIdentity();
    // Czyszcenie macierzy bieżącej
}

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli



int main(void)
{
    setlocale(LC_CTYPE, "Polish");
    //instrukcja przypisanych klawiszy
    std::cout << "Duże jajko:" << std::endl;
    std::cout << "  Wypełnienie:" << std::endl;
    std::cout << "      P - punkty" << std::endl;
    std::cout << "      L - linie" << std::endl;
    std::cout << "      T - trójkąty" << std::endl;
    std::cout << "  Obroty:" << std::endl;
    std::cout << "      1 - wokół osi X" << std::endl;
    std::cout << "      2 - wokół osi Y" << std::endl;
    std::cout << "      0 - nie obraca się" << std::endl;
    std::cout << "Małe jajko:" << std::endl;
    std::cout << "  Wypełnienie:" << std::endl;
    std::cout << "      Q - punkty" << std::endl;
    std::cout << "      W - linie" << std::endl;
    std::cout << "      E - trójkąty" << std::endl;
    std::cout << std::endl;


    std::cout << "Podaj liczbę punktów chmury: ";
    int n;
    std::cin >> n;
    rozmiar = (int)sqrt(n);

    //generowanie tablic
    generujKolory();
    generujTabliceJajo();
    generujKoloryMale();
    generujTabliceMaleJajo();


    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(300, 300);

    glutCreateWindow("Jajo male i duze");

    glutDisplayFunc(RenderScene);
    // Określenie, że funkcja RenderScene będzie funkcją zwrotną
    // (callback function).  Bedzie ona wywoływana za każdym razem
    // gdy zajdzie potrzba przeryswania okna 
    glutReshapeFunc(ChangeSize);
    // Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
    // zazmiany rozmiaru okna      
    MyInit();
    // Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
    // inicjalizacje konieczne  przed przystąpieniem do renderowania
    glEnable(GL_DEPTH_TEST);
    // Włączenie mechanizmu usuwania powierzchni niewidocznych

    glutKeyboardFunc(keys);

    glutIdleFunc(spinEgg);

    glutMainLoop();
    // Funkcja uruchamia szkielet biblioteki GLUT

    
}

/*************************************************************************************/

