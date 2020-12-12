// Copyright[2020] <Micha³ Skrok>
#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <string>
#include <iostream> // NOLINT



typedef float point3[3];
typedef float point2[2];  // do podzia³u tekstury

int rozmiar;
int kolor = 0;  // 0-biale, 1-czerwone, 2-zielone, 3-niebieskie
int kolor2 = 0;  // 0-biale, 1-czerwone, 2-zielone, 3-niebieskie

double u, v;

static GLfloat theta[] = { 0.0, 0.0, 0.0 };  // obroty dla du¿ego jajka

int obrot = 0;  // 1 - wokó³ osi X, 2 - wokó³ osi Y, 0 - brak obrotu

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

point2 macierzTex[500][500];

GLbyte* pBytes;
GLint ImWidth, ImHeight, ImComponents;
GLenum ImFormat;

time_t czas1, czas2;

int licznik = 1;
int tryb = 0;

GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth,
    GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat) {

    // Struktura dla nag³ówka pliku  TGA

#pragma pack(1)
    typedef struct {
        GLbyte idlength;
        GLbyte colormaptype;
        GLbyte datatypecode;
        unsigned short colormapstart;  // NOLINT
        unsigned short colormaplength;  // NOLINT
        unsigned char  colormapdepth;
        unsigned short x_orgin;  // NOLINT
        unsigned short y_orgin;  // NOLINT
        unsigned short width;  // NOLINT
        unsigned short height;  // NOLINT
        GLbyte    bitsperpixel;
        GLbyte    descriptor;
    } TGAHEADER;
#pragma pack(8)

    FILE* pFile;
    TGAHEADER tgaHeader;
    unsigned long lImageSize;  // NOLINT
    short sDepth;  // NOLINT
    GLbyte* pbitsperpixel = NULL;

    // Wartoœci domyœlne zwracane w przypadku b³êdu

    *ImWidth = 0;
    *ImHeight = 0;
    *ImFormat = GL_BGR_EXT;
    *ImComponents = GL_RGB8;

    pFile = fopen(FileName, "rb");
    if (pFile == NULL)
        return NULL;

    /*********************/
    // Przeczytanie nag³ówka pliku

    fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

    /*********************/

    // Odczytanie szerokoœci, wysokoœci i g³êbi obrazu

    *ImWidth = tgaHeader.width;
    *ImHeight = tgaHeader.height;
    sDepth = tgaHeader.bitsperpixel / 8;

    /***************************/
    // Sprawdzenie, czy g³êbia spe³nia za³o¿one warunki (8, 24, lub 32 bity)

    if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 &&
        tgaHeader.bitsperpixel != 32)
        return NULL;

    /***************************/

    // Obliczenie rozmiaru bufora w pamiêci

    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

    /***************************/

    // Alokacja pamiêci dla danych obrazu

    pbitsperpixel = reinterpret_cast<GLbyte*>
        (malloc(lImageSize * sizeof(GLbyte)));

    if (pbitsperpixel == NULL)
        return NULL;

    if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1) {
        free(pbitsperpixel);
        return NULL;
    }

    /***************************/
    // Ustawienie formatu OpenGL

    switch (sDepth) {
    case 3:
        *ImFormat = GL_BGR_EXT;
        *ImComponents = GL_RGB8;
        break;

    case 4:
        *ImFormat = GL_BGRA_EXT;
        *ImComponents = GL_RGBA8;
        break;

    case 1:
        *ImFormat = GL_LUMINANCE;
        *ImComponents = GL_LUMINANCE8;
        break;
    };

    fclose(pFile);

    return pbitsperpixel;
}



// generowanie tablicy duzego jajka
void generujTabliceJajo() {
    for (int i = 0; i < rozmiar; i++) {
        for (int j = 0; j < rozmiar; j++) {
            u = static_cast<double>(i) / (rozmiar -1);
            v = static_cast<double>(j) / (rozmiar -1);

            double uT = static_cast<double>(i) / (rozmiar +1);
            double vT = static_cast<double>(j) / (rozmiar +1);

            macierzTex[i][j][0] = u;
            macierzTex[i][j][1] = vT;

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


// funkcja rysujaca du¿e jajko
void rysujJajo() {
    for (int i = 0; i < rozmiar - 1; i++) {
        for (int j = 0; j < rozmiar - 1; j++) {
                glColor3f(255.0f, 255.0f, 255.0f);
                glBegin(GL_TRIANGLES);

                glNormal3fv(normalny[i][j+1]);
                glTexCoord2fv(macierzTex[i][j+1]);
                glVertex3fv(macierz[i][j+1]);

                glNormal3fv(normalny[i+1][j]);
                glTexCoord2fv(macierzTex[i+1][j]);
                glVertex3fv(macierz[i+1][j]);

                glNormal3fv(normalny[i+1][j+1]);
                glTexCoord2fv(macierzTex[i+1][j+1]);
                glVertex3fv(macierz[i+1][j+1]);

                glNormal3fv(normalny[i+1][j]);
                glTexCoord2fv(macierzTex[i+1][j]);
                glVertex3fv(macierz[i+1][j]);

                glNormal3fv(normalny[i][j+1]);
                glTexCoord2fv(macierzTex[i][j+1]);
                glVertex3fv(macierz[i][j+1]);

                glNormal3fv(normalny[i][j]);
                glTexCoord2fv(macierzTex[i][j]);
                glVertex3fv(macierz[i][j]);

                glEnd();
        }
    }
}


// funkcja obrotów
void spinEgg() {
    czas1 = time(0);
    if ((czas1 - czas2 != 0) && tryb == 0) {  // zmiana co sekunde
        if (licznik == 1)
            pBytes = LoadTGAImage("car_1t.tga", &ImWidth,
            &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 2)
            pBytes = LoadTGAImage("car_2t.tga", &ImWidth,
            &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 3)
            pBytes = LoadTGAImage("car_3t.tga", &ImWidth,
            &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 4)
            pBytes = LoadTGAImage("car_4t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 5)
            pBytes = LoadTGAImage("car_5t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 6)
            pBytes = LoadTGAImage("car_6t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 7)
            pBytes = LoadTGAImage("car_7t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 8)
            pBytes = LoadTGAImage("car_8t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 9)
            pBytes = LoadTGAImage("car_9t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 10)
            pBytes = LoadTGAImage("car_10t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 11)
            pBytes = LoadTGAImage("car_11t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 12)
            pBytes = LoadTGAImage("car_12t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 13)
            pBytes = LoadTGAImage("car_13t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 14)
            pBytes = LoadTGAImage("car_14t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 15)
            pBytes = LoadTGAImage("car_15t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 16)
            pBytes = LoadTGAImage("car_16t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);
        else if (licznik == 17)
            pBytes = LoadTGAImage("car_17t.tga", &ImWidth,
                &ImHeight, &ImComponents, &ImFormat);

        glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth,
            ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
        free(pBytes);
        czas2 = czas1;
        licznik++;
        if (licznik == 18) licznik = 1;
    }

    if (tryb == 1)
        pBytes = LoadTGAImage("D1_t.tga", &ImWidth,
            &ImHeight, &ImComponents, &ImFormat);
    else if (tryb == 2)
        pBytes = LoadTGAImage("D2_t.tga", &ImWidth,
            &ImHeight, &ImComponents, &ImFormat);
    else if (tryb == 3)
        pBytes = LoadTGAImage("D3_t.tga", &ImWidth,
            &ImHeight, &ImComponents, &ImFormat);

    if (tryb != 0) {
        glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth,
            ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
        free(pBytes);
    }

    if (obrot == 1) {
        theta[0] -= 0.5;
        if (theta[0] > 360.0) theta[0] -= 360.0;
    } else if (obrot == 2) {
        theta[1] -= 0.5;
        if (theta[1] > 360.0) theta[1] -= 360.0;
    } else { }




    glutPostRedisplay();  // odœwie¿enie zawartoœci aktualnego okna
}



// Funkcja rysuj¹ca osie uk³adu wspó³rzêdnych
void Axes(void) {
    point3  x_min = { -5.0, 0.0, 0.0 };
    point3  x_max = { 5.0, 0.0, 0.0 };
    // pocz¹tek i koniec obrazu osi x

    point3  y_min = { 0.0, -5.0, 0.0 };
    point3  y_max = { 0.0,  5.0, 0.0 };
    // pocz¹tek i koniec obrazu osi y

    point3  z_min = { 0.0, 0.0, -5.0 };
    point3  z_max = { 0.0, 0.0,  5.0 };
    //  pocz¹tek i koniec obrazu osi y
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

// Funkcja okreœlaj¹ca co ma byæ rysowane (zawsze wywo³ywana gdy trzeba
// przerysowaæ scenê)


void RenderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Czyszczenie okna aktualnym kolorem czyszcz¹cym

    glLoadIdentity();
    // Czyszczenie macierzy bie¿¹cej

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
    // Narysowanie osi przy pomocy funkcji zdefiniowanej wy¿ej

    glColor3f(1.0f, 1.0f, 1.0f);  // Ustawienie koloru rysowania na bia³y

    glRotatef(theta[0], 1.0, 0.0, 0.0);

    glRotatef(theta[1], 0.0, 1.0, 0.0);

    glRotatef(theta[2], 0.0, 0.0, 1.0);

    rysujJajo();

    glFlush();
    // Przekazanie poleceñ rysuj¹cych do wykonania

    glutSwapBuffers();
}

void keys(unsigned char key, int x, int y) {
    // obroty du¿ego jajka
    if (key == '1') obrot = 1;
    if (key == '2') obrot = 2;
    if (key == '0') obrot = 0;

    if (key == 'q') tryb = 0;
    if (key == 'w') tryb = 1;
    if (key == 'e') tryb = 2;
    if (key == 'r') tryb = 3;

    RenderScene();  // przerysowanie obrazu sceny
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
    // Kolor czyszc¹cy (wype³nienia okna) ustawiono na czarny

    czas2 = time(0);

    // Definicja materia³u
    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    // wspó³czynniki ka =[kar,kag,kab] dla œwiat³a otoczenia

    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    // wspó³czynniki kd =[kdr,kdg,kdb] œwiat³a rozproszonego

    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    // wspó³czynniki ks =[ksr,ksg,ksb] dla œwiat³a odbitego

    GLfloat mat_shininess = { 20.0 };
    // wspó³czynnik n opisuj¹cy po³ysk powierzchni

// Definicja Ÿród³a œwiat³a
    /////// œwiat³o ruchome myszk¹
    GLfloat light1_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    // sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a otoczenia
    // Ia = [Iar,Iag,Iab]

    GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    // sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
    // odbicie dyfuzyjne Id = [Idr,Idg,Idb]

    GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    // sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
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
    // sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji
    // odleg³oœci od Ÿród³a

    GLfloat att1_linear = { 0.05 };
    // sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji
    // odleg³oœci od Ÿród³a

    GLfloat att1_quadratic = { 0.001 };
    // sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji
    // odleg³oœci od Ÿród³a

    ///////////////////// swiatlo kr¹¿¹ce

    GLfloat light2_specular[] = {1.0, 1.0, 1.0, 1.0};
    // sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
    // odbicie kierunkowe Is = [Isr,Isg,Isb]

    GLfloat light2_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    // sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
    // odbicie dyfuzyjne Id = [Idr,Idg,Idb]

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
    // sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji
    // odleg³oœci od Ÿród³a

    GLfloat att2_linear = { 0.05 };
    // sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji
    // odleg³oœci od Ÿród³a

    GLfloat att2_quadratic = { 0.001 };
    // sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji
    // odleg³oœci od Ÿród³a

    GLfloat light_position[] = { 0.0, 10.0, 10.0, 1.0 };
    // po³o¿enie Ÿród³a

    // swaitlo
    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    // sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a otoczenia
    // Ia = [Iar,Iag,Iab]

    //// œwiat³o sta³e
    GLfloat light_diffuse[] = { 0.5, 0.0, 0.0, 1.0 };
    // sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
    // odbicie dyfuzyjne Id = [Idr,Idg,Idb]

    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    // sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
    // odbicie kierunkowe Is = [Isr,Isg,Isb]

    GLfloat att_constant = { 1.0 };
    // sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji
    // odleg³oœci od Ÿród³a

    GLfloat att_linear = { 0.05 };
    // sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji
    // odleg³oœci od Ÿród³a

    GLfloat att_quadratic = { 0.001 };
    // sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji
    // odleg³oœci od Ÿród³a

/********************************/
// Ustawienie parametrów materia³u i Ÿród³a œwiat³a

/*********************************/
// Ustawienie patrametrów materia³u


    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


    /****************************/
    // Ustawienie parametrów Ÿród³a

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
    // Ustawienie opcji systemu oœwietlania sceny

    glShadeModel(GL_SMOOTH);  // w³aczenie ³agodnego cieniowania
    glEnable(GL_LIGHTING);   // w³aczenie systemu oœwietlenia sceny
    glEnable(GL_LIGHT0);     // w³¹czenie Ÿród³a o numerze 0
    glEnable(GL_LIGHT1);     // w³¹czenie Ÿród³a o numerze 1
    glEnable(GL_LIGHT2);     // w³¹czenie Ÿród³a o numerze 2
    glEnable(GL_DEPTH_TEST);  // w³¹czenie mechanizmu z-bufora


    // teksturki

    pBytes = LoadTGAImage("car_1t.tga", &ImWidth,
        &ImHeight, &ImComponents, &ImFormat);

    // Zdefiniowanie tekstury 2-D
    glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth,
        ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

    // Zwolnienie pamiêci
    free(pBytes);

    // W³¹czenie mechanizmu teksturowania
    glEnable(GL_TEXTURE_2D);

    // Ustalenie trybu teksturowania
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Okreœlenie sposobu nak³adania tekstur
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
}

/**************************************/

// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokoœæ i szerokoœæ okna) s¹
// przekazywane do funkcji za ka¿dym razem gdy zmieni siê rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical) {
    if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
        vertical = 1;

    pix2angle_poziom = 360.0 / static_cast<float>(horizontal);
    pix2angle_pion = 360.0 / static_cast<float>(vertical);

    glMatrixMode(GL_PROJECTION);
    // Prze³¹czenie macierzy bie¿¹cej na macierz projekcji
    glLoadIdentity();
    // Czyszcznie macierzy bie¿¹cej

    gluPerspective(70, 1.0, 0.01, 200.0);
    // Ustawienie parametrów dla rzutu perspektywicznego

    if (horizontal <= vertical)
        glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

    else
        glViewport((horizontal - vertical) / 2, 0, vertical, vertical);

    glMatrixMode(GL_MODELVIEW);
    // Prze³¹czenie macierzy bie¿¹cej na macierz widoku modelu

    glLoadIdentity();
    // Czyszcenie macierzy bie¿¹cej
}

/*********************/

// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli



int main(void) {
    setlocale(LC_CTYPE, "Polish");
    // instrukcja przypisanych klawiszy

    std::cout << "  Obroty:" << std::endl;
    std::cout << "      1 - wokó³ osi X" << std::endl;
    std::cout << "      2 - wokó³ osi Y" << std::endl;
    std::cout << "      0 - nie obraca siê" << std::endl;
    std::cout << "  Tekstury:" << std::endl;
    std::cout << "      Q - je¿d¿¹cy dooko³a samochodzik" << std::endl;
    std::cout << "      W - D1_t.tga" << std::endl;
    std::cout << "      E - D2_t.tga" << std::endl;
    std::cout << "      R - D3_t.tga" << std::endl;
    std::cout << "Podaj liczbê punktów chmury: ";
    int n;
    std::cin >> n;
    rozmiar = static_cast<int>(sqrt(n));

    std::cout << std::endl << "0-bia³y, 1-czerwony";
    std::cout << " 2-zielony, 3-niebieski, 4-¿ó³ty" << std::endl;

    std::cout << "Podaj kolor swiatla sterowanego myszka: ";
    std::cin >> n;
    kolor = n;

    std::cout << "Podaj kolor swiatla kr¹¿¹cego dooko³a jajka: ";
    std::cin >> n;
    kolor2 = n;

    // generowanie tablic
    generujTabliceJajo();

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(400, 400);

    glutCreateWindow("Tekstury");

    glutDisplayFunc(RenderScene);
    // Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹
    // (callback function).  Bedzie ona wywo³ywana za ka¿dym razem
    // gdy zajdzie potrzba przeryswania okna
    glutReshapeFunc(ChangeSize);
    // Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹
    // zazmiany rozmiaru okna

    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);

    MyInit();
    // Funkcja MyInit() (zdefiniowana powy¿ej) wykonuje wszelkie
    // inicjalizacje konieczne  przed przyst¹pieniem do renderowania
    glEnable(GL_DEPTH_TEST);
    // W³¹czenie mechanizmu usuwania powierzchni niewidocznych

    glutKeyboardFunc(keys);

    glutIdleFunc(spinEgg);

    glutMainLoop();
    // Funkcja uruchamia szkielet biblioteki GLUT
}
