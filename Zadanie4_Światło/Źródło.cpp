// Copyright[2020] <Michał Skrok>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <iostream>

typedef float point3[3];

int rozmiar;
int kolor = 0;  // 0-biale, 1-czerwone, 2-zielone, 3-niebieskie
int kolor2 = 0;  // 0-biale, 1-czerwone, 2-zielone, 3-niebieskie

double u, v;

static GLfloat theta[] = { 0.0, 0.0, 0.0 };  // obroty dla dużego jajka

int obrot = 0;  // 1 - wokół osi X, 2 - wokół osi Y, 0 - brak obrotu

// swiatla
GLfloat light_1_pos[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_2_pos[] = { 0.0, 0.0, 0.0, 0.0 };


static GLfloat poziom_1 = 0.0;
static GLfloat pion_1 = 0.0;

static GLfloat poziom_2 = 0.0;
static GLfloat pion_2 = 0.0;

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

// macierze dla duzego jajka
point3 macierz[500][500];
point3 normalny[500][500];


// generowanie tablicy duzego jajka
void generujTabliceJajo() {
    for (int i = 0; i < rozmiar; i++) {
        for (int j = 0; j < rozmiar; j++) {
            u = static_cast<double>(i) / (rozmiar - 1);
            v = static_cast<double>(j) / (rozmiar - 1);

            float xu, xv, yu, yv, zu, zv;
            float lng;

            macierz[i][j][0] = (-(90 * pow(u, 5)) + (225 * pow(u, 4)) -
            (270 * pow(u, 3)) + (180 * pow(u, 2)) - 45 * u) * cos(M_PI * v);

            macierz[i][j][1] = (160 * pow(u, 4)) -
            (320 * pow(u, 3)) + (160 * pow(u, 2)) - 5;

            macierz[i][j][2] = (-(90 * pow(u, 5)) + (225 * pow(u, 4)) -
            (270 * pow(u, 3)) + (180 * pow(u, 2)) - 45 * u) * sin(M_PI * v);

            xu = (-450 * pow(u, 4) + 900 * pow(u, 3) -
            810 * pow(u, 2) + 360 * u - 45) * cos(M_PI * v);

            xv = 3.14 * (90 * pow(u, 5) - 225 * pow(u, 4) +
            270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(M_PI * v);

            yu = (640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u);

            yv = 0;

            zu = (-450 * pow(u, 4) + 900 * pow(u, 3) -
            810 * pow(u, 2) + 360 * u - 45) * sin(M_PI * v);

            zv = -3.14 * (90 * pow(u, 5) - 225 * pow(u, 4) +
            270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(M_PI * v);

            normalny[i][j][0] = (yu * zv) - (zu * yv);
            normalny[i][j][1] = (zu * xv) - (xu * zv);
            normalny[i][j][2] = (xu * yv) - (yu * xv);

            lng = sqrt((normalny[i][j][0] * normalny[i][j][0]) +
            (normalny[i][j][1] * normalny[i][j][1]) +
            (normalny[i][j][2] * normalny[i][j][2]));

            normalny[i][j][0] = (yu * zv - zu * yv) / lng;
            normalny[i][j][1] = (zu * xv - xu * zv) / lng;
            normalny[i][j][2] = (xu * yv - yu * xv) / lng;

            if (i >= rozmiar / 2) {
                normalny[i][j][0] *= -1;
                normalny[i][j][1] *= -1;
                normalny[i][j][2] *= -1;
            }
        }
    }
}


// funkcja rysujaca duże jajko
void rysujJajo() {
    for (int i = 0; i < rozmiar - 1; i++) {
        for (int j = 0; j < rozmiar - 1; j++) {
                glColor3f(255.0f, 255.0f, 255.0f);
                glBegin(GL_TRIANGLES);

                glNormal3fv(normalny[i][j]);
                glVertex3fv(macierz[i][j]);

                glNormal3fv(normalny[i+1][j+1]);
                glVertex3fv(macierz[i+1][j+1]);

                glNormal3fv(normalny[i+1][j]);
                glVertex3fv(macierz[i+1][j]);

                glNormal3fv(normalny[i][j]);
                glVertex3fv(macierz[i][j]);

                glNormal3fv(normalny[i+1][j+1]);
                glVertex3fv(macierz[(i + 1)][(j + 1)]);

                glNormal3fv(normalny[i][j+1]);
                glVertex3fv(macierz[i][(j + 1)]);

                glEnd();
        }
    }
}


// funkcja obrotów
void spinEgg() {
    if (obrot == 1) {
        theta[0] -= 0.5;
        if (theta[0] > 360.0) theta[0] -= 360.0;
    } else if (obrot == 2) {
        theta[1] -= 0.5;
        if (theta[1] > 360.0) theta[1] -= 360.0;
    } else { }

    glutPostRedisplay();  // odświeżenie zawartości aktualnego okna
}



// Funkcja rysująca osie układu współrzędnych
void Axes(void) {
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
    glBegin(GL_LINES);  // rysowanie osi x
    glVertex3fv(x_min);
    glVertex3fv(x_max);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
    glBegin(GL_LINES);  // rysowanie osi y

    glVertex3fv(y_min);
    glVertex3fv(y_max);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
    glBegin(GL_LINES);  // rysowanie osi z

    glVertex3fv(z_min);
    glVertex3fv(z_max);
    glEnd();
}

/*****************************/

// Funkcja określająca co ma być rysowane (zawsze wywoływana gdy trzeba
// przerysować scenę)


void RenderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Czyszczenie okna aktualnym kolorem czyszczącym

    glLoadIdentity();
    // Czyszczenie macierzy bieżącej

    gluLookAt(0, 0, -10.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);

    if (status == 1) {
        poziom_1 = poziom_1 + delta_x * pix2angle_poziom * 0.01;
        pion_1 = pion_1 + delta_y * pix2angle_pion * 0.01;
    }

    poziom_2 = poziom_2 + 0.03 * pix2angle_poziom;

    light_1_pos[0] = R * cos(poziom_1) * cos(pion_1);
    light_1_pos[1] = R * sin(pion_1);
    light_1_pos[2] = R * sin(poziom_1) * cos(pion_1);

    light_2_pos[0] = R * cos(poziom_2) * cos(pion_2);
    light_2_pos[1] = R * sin(pion_2);
    light_2_pos[2] = R * sin(poziom_2) * cos(pion_2);

    glLightfv(GL_LIGHT0, GL_POSITION, light_1_pos);
    glLightfv(GL_LIGHT1, GL_POSITION, light_2_pos);

    Axes();
    // Narysowanie osi przy pomocy funkcji zdefiniowanej wyżej

    glColor3f(1.0f, 1.0f, 1.0f);  // Ustawienie koloru rysowania na biały

    glRotatef(theta[0], 1.0, 0.0, 0.0);

    glRotatef(theta[1], 0.0, 1.0, 0.0);

    glRotatef(theta[2], 0.0, 0.0, 1.0);

    rysujJajo();

    glFlush();
    // Przekazanie poleceń rysujących do wykonania

    glutSwapBuffers();
}

void keys(unsigned char key, int x, int y) {
    // obroty dużego jajka
    if (key == '1') obrot = 1;
    if (key == '2') obrot = 2;
    if (key == '0') obrot = 0;

    RenderScene();  // przerysowanie obrazu sceny
    std::cout << key << std::endl;
}

void Mouse(int btn, int state, int x, int y) {
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        x_pos_old = x;
        y_pos_old = y;
        status = 1;
    } else {
        status = 0;
    }
}


void Motion(GLsizei x, GLsizei y) {
    delta_x = -(x - x_pos_old);
    x_pos_old = x;

    delta_y = -(y - y_pos_old);
    y_pos_old = y;

    glutPostRedisplay();
}

// light1-sterowane myszka, 2-krazace dookola, 3-stojace w miejscu (biale)
void MyInit(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Kolor czyszcący (wypełnienia okna) ustawiono na czarny

    // Definicja materiału
    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    // współczynniki ka =[kar,kag,kab] dla światła otoczenia

    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    // współczynniki kd =[kdr,kdg,kdb] światła rozproszonego

    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    // współczynniki ks =[ksr,ksg,ksb] dla światła odbitego

    GLfloat mat_shininess = { 20.0 };
    // współczynnik n opisujący połysk powierzchni

// Definicja źródła światła
    /////// światło ruchome myszką
    GLfloat light1_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    // składowe intensywności świecenia źródła światła otoczenia
    // Ia = [Iar,Iag,Iab]

    GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie dyfuzyjne Id = [Idr,Idg,Idb]

    GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie kierunkowe Is = [Isr,Isg,Isb]
    if (kolor == 0) {
        light1_diffuse[0] = 1.0;
        light1_diffuse[1] = 1.0;
        light1_diffuse[2] = 1.0;
    } else if (kolor == 1) {
        light1_diffuse[0] = 1.0;
        light1_diffuse[1] = 0.0;
        light1_diffuse[2] = 0.0;
    } else if (kolor == 2) {
        light1_diffuse[0] = 0.0;
        light1_diffuse[1] = 1.0;
        light1_diffuse[2] = 0.0;
    } else if (kolor == 3) {
        light1_diffuse[0] = 0.0;
        light1_diffuse[1] = 0.0;
        light1_diffuse[2] = 1.0;
    } else {
        light1_diffuse[0] = 1.0;
        light1_diffuse[1] = 1.0;
        light1_diffuse[2] = 0.0;
    }

    GLfloat att1_constant = { 1.0 };
    // składowa stała ds dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    GLfloat att1_linear = { 0.05 };
    // składowa liniowa dl dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    GLfloat att1_quadratic = { 0.001 };
    // składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    ///////////////////// swiatlo krążące

    GLfloat light2_specular[] = {1.0, 1.0, 1.0, 1.0};
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie kierunkowe Is = [Isr,Isg,Isb]

    if (kolor2 == 0) {
        light2_specular[0] = 1.0;
        light2_specular[1] = 1.0;
        light2_specular[2] = 1.0;
    } else if (kolor2 == 1) {
        light2_specular[0] = 1.0;
        light2_specular[1] = 0.0;
        light2_specular[2] = 0.0;
    } else if (kolor2 == 2) {
        light2_specular[0] = 0.0;
        light2_specular[1] = 1.0;
        light2_specular[2] = 0.0;
    } else if (kolor2 == 3) {
        light2_specular[0] = 0.0;
        light2_specular[1] = 0.0;
        light2_specular[2] = 1.0;
    } else {
        light2_specular[0] = 1.0;
        light2_specular[1] = 1.0;
        light2_specular[2] = 0.0;
    }

    GLfloat att2_constant = { 1.0 };
    // składowa stała ds dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    GLfloat att2_linear = { 0.05 };
    // składowa liniowa dl dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    GLfloat att2_quadratic = { 0.001 };
    // składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    GLfloat light_position[] = { 0.0, 10.0, 10.0, 1.0 };
    // położenie źródła

    // swaitlo
    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    // składowe intensywności świecenia źródła światła otoczenia
    // Ia = [Iar,Iag,Iab]

    //// światło stałe
    GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie dyfuzyjne Id = [Idr,Idg,Idb]

    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie kierunkowe Is = [Isr,Isg,Isb]

    GLfloat att_constant = { 1.0 };
    // składowa stała ds dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    GLfloat att_linear = { 0.05 };
    // składowa liniowa dl dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    GLfloat att_quadratic = { 0.001 };
    // składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

/********************************/
// Ustawienie parametrów materiału i źródła światła

/*********************************/
// Ustawienie patrametrów materiału


    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


    /****************************/
    // Ustawienie parametrów źródła

    glLightfv(GL_LIGHT0, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_1_pos);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att1_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att1_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att1_quadratic);

    glLightfv(GL_LIGHT1, GL_SPECULAR, light2_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light_2_pos);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att2_constant);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att2_linear);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att2_quadratic);

    glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT2, GL_POSITION, light_position);
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, att_quadratic);
    /*************************************/
    // Ustawienie opcji systemu oświetlania sceny

    glShadeModel(GL_SMOOTH);  // właczenie łagodnego cieniowania
    glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny
    glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
    glEnable(GL_LIGHT1);     // włączenie źródła o numerze 1
    glEnable(GL_LIGHT2);     // włączenie źródła o numerze 2
    glEnable(GL_DEPTH_TEST);  // włączenie mechanizmu z-bufora
}

/**************************************/

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są
// przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical) {
    if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
        vertical = 1;

    pix2angle_poziom = 360.0 / static_cast<float>(horizontal);
    pix2angle_pion = 360.0 / static_cast<float>(vertical);

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

/*********************/

// Główny punkt wejścia programu. Program działa w trybie konsoli



int main(void) {
    setlocale(LC_CTYPE, "Polish");
    // instrukcja przypisanych klawiszy

    std::cout << "  Obroty:" << std::endl;
    std::cout << "      1 - wokół osi X" << std::endl;
    std::cout << "      2 - wokół osi Y" << std::endl;
    std::cout << "      0 - nie obraca się" << std::endl;
    std::cout << "Podaj liczbę punktów chmury: ";
    int n;
    std::cin >> n;
    rozmiar = static_cast<int>(sqrt(n));

    std::cout << std::endl << "0-biały, 1-czerwony";
    std::cout << " 2-zielony, 3-niebieski, 4-żółty" << std::endl;

    std::cout << "Podaj kolor swiatla sterowanego myszka: ";
    std::cin >> n;
    kolor = n;

    std::cout << "Podaj kolor swiatla krążącego dookoła jajka: ";
    std::cin >> n;
    kolor2 = n;

    // generowanie tablic
    generujTabliceJajo();

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(400, 400);

    glutCreateWindow("I stalo sie swiatlo");

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

