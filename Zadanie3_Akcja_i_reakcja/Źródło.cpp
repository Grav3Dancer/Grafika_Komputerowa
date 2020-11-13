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

//zmienne do "ruchu kamerą"
static GLfloat viewer[] = { 0.0, 0.0, 15.0 };
// inicjalizacja położenia obserwatora

static GLfloat poziom = 0.0;
static GLfloat pion = 0.0;

static GLfloat pix2angle_poziom;
static GLfloat pix2angle_pion;

static int status = 0;

static int x_pos_old = 0;
static int delta_x = 0;
static int y_pos_old = 0;
static int delta_y = 0;

static GLfloat R_old = 0.0;
static GLfloat delta_R = 0.0;
static GLfloat R = 15.0;

bool res = false; // zmienna do ustawienia resetu widoku


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

    if (status == 1)
    {
        poziom = poziom + delta_x * pix2angle_poziom * 0.01;
        pion = pion + delta_y * pix2angle_pion * 0.01;
    }
    else if (status == 2)
    {
        R = R + delta_R * pix2angle_pion;
        if (R < 0) R = 0;
    }

    if (res == true)
    {
        pion = 0;
        poziom = 0;
        R = 15.0;
    }

    viewer[0] = R * cos(poziom) * cos(pion);
    viewer[1] = R * sin(pion);
    viewer[2] = R * sin(poziom) * cos(pion);

    gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    Axes();
    // Narysowanie osi przy pomocy funkcji zdefiniowanej wyżej

    glColor3f(1.0f, 1.0f, 1.0f); // Ustawienie koloru rysowania na biały

    glRotatef(theta[0], 1.0, 0.0, 0.0);

    glRotatef(theta[1], 0.0, 1.0, 0.0);

    glRotatef(theta[2], 0.0, 0.0, 1.0);

    rysujJajo();
    
    glPushMatrix(); // duże jajko pomijane przy obracaniu małego

    glRotatef(thetaMale, 0.0, 1.0, 0.0);

    rysujMaleJajo();

    glFlush();
    // Przekazanie poleceń rysujących do wykonania

    glPopMatrix();

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

    if (key == 'r') res = true; // reset

    RenderScene(); // przerysowanie obrazu sceny

    res = false;
}


void Mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        x_pos_old = x;
        y_pos_old = y;
        status = 1;
    }
    else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        R_old = y;
        status = 2;
    }
    else
    {
        status = 0;
    }
}


void Motion(GLsizei x, GLsizei y)
{
    delta_x = x - x_pos_old;
    x_pos_old = x;

    delta_y = y - y_pos_old;
    y_pos_old = y;

    delta_R = (y - R_old) / 3; // spowolniony zoom in&out, zbyt szybki
    R_old = y;

    glutPostRedisplay();
}


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
    if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
        vertical = 1;

    pix2angle_poziom = 360.0 / (float)horizontal;
    pix2angle_pion = 360.0 / (float)vertical;

    glMatrixMode(GL_PROJECTION);
    // Przełączenie macierzy bieżącej na macierz projekcji 
    glLoadIdentity();
    // Czyszcznie macierzy bieżącej   

    gluPerspective(70, 1.0, 0.01, 200.0);
    // Ustawienie parametrów dla rzutu perspektywicznego
          
    if (horizontal <= vertical)
        glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

    else
        glViewport((horizontal - vertical) / 2, 0, vertical, vertical);

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
    std::cout << "Ruch obserwatora :" << std::endl;
    std::cout << "  LPM - przemieszczanie kamery" << std::endl;
    std::cout << "  PPM - zoom in & out" << std::endl;
    std::cout << "  R - reset widoku" << std::endl;
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

    glutInitWindowSize(400, 400);

    glutCreateWindow("Akcja i reakcja");

    glutDisplayFunc(RenderScene);
    // Określenie, że funkcja RenderScene będzie funkcją zwrotną
    // (callback function).  Bedzie ona wywoływana za każdym razem
    // gdy zajdzie potrzba przeryswania okna 
    glutReshapeFunc(ChangeSize);
    // Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
    // zazmiany rozmiaru okna   

    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);

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

