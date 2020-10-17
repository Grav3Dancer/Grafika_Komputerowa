#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <ctime>
#include <iostream>
#include <cstdlib>

using namespace std;

int poziomy; //Liczba poziom�w dywanu 

float rnd_kolor() // funkcja generuj�ca losowy kolor  
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX); 
}

float rnd_nierownosc(float mnoznik) // funkcja generuj�ca losow� nier�wno�� ("poturbowanie" dywanu)
{
	return mnoznik * 0.02 * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
}

void rysujKwadrat(float x, float y, float rozmiar) //  funkcja rysuj�ca pojedynczy kwadrat
{
	glRectf(x, y, x + rozmiar, y - rozmiar);
	glColor3f(rnd_kolor(), rnd_kolor(), rnd_kolor()); // ustalanie koloru z u�yciem funkcji losuj�cej		
}

void rysujDywan(float x, float y, float rozmiar, float obecnyPoziom) // "g��wna" funkcja, tworz�ca Dywan Sierpi�skiego
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if ((i == 1) && (j == 1)) // sprawdzenie czy �rodek rozwa��nego kwadratu
			{ }
			else
			{
				if (obecnyPoziom > poziomy - 2) //sprawdzenie czy funkcja jest na wymaganym poziomie :
				{	// je�li tak, to tysowane s� kwadraty
					rysujKwadrat(x + i*(rozmiar/3) + rnd_nierownosc(rozmiar), y - j*(rozmiar/3) + rnd_nierownosc(rozmiar), rozmiar/3);
				}
				else
				{	//je�li nie, ponowne wywo�anie funkcji rysuj�cej dywan - zag��bienie si� o jeden poziom dalej
					rysujDywan(x + i*(rozmiar/3) + rnd_nierownosc(rozmiar), y - j*(rozmiar/3) + rnd_nierownosc(rozmiar), rozmiar/3, obecnyPoziom+1);
				}
			}

		}

	}
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT); 
	// Czyszczenie okna aktualnym kolorem czyszcz�cym

	rysujDywan(-50.0f, 50.0f, 100.0f, 0); // Wywo�anie stworzonej funkcji, rysuj�cej Dywan Sierpi�skiego

	glFlush(); 
	// Przekazanie polece� rysuj�cych do wykonania
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	GLfloat AspectRatio; 
	// Deklaracja zmiennej AspectRatio okre�laj�cej proporcj� wymiar�w okna

	if (vertical == 0) vertical = 1;
	// Zabezpieczenie pzred dzieleniem przez 0

	glViewport(0, 0, horizontal, vertical); 
	// Ustawienie wielko�ciokna okna urz�dzenia (Viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)

	glMatrixMode(GL_PROJECTION); 
	// Okre�lenie uk�adu wsp�rz�dnych obserwatora

	glLoadIdentity();  
	// Okre�lenie przestrzeni ograniczaj�cej

	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical; 
	// Wyznaczenie wsp�czynnika proporcji okna

	if (horizontal <= vertical)
		glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);

	else
		glOrtho(-100.0 * AspectRatio, 100.0 * AspectRatio, -100.0, 100.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW); 
	// Okre�lenie uk�adu wsp�rz�dnych

	glLoadIdentity();
}

int main(int argc, char** argv)
{
	cout<<"Podaj liczbe poziomow dywanu: ";

	cin>>poziomy; 
	// Wczytanie rz�danej liczby poziom�w

	srand(time(NULL));
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);   
	// Ustawienie trybu wy�wietlania
	// GLUT_SINGLE - pojedynczy bufor wy�wietlania
    // GLUT_RGBA - model kolor�w RGB

	glutCreateWindow("Grafika komputerowa - Dywan Sierpi�skiego");
	// Utworzenie okna i okre�lenie tre�ci napisu w nag��wku okna

	glutDisplayFunc(RenderScene); 
	// Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn� (callback)

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn� za zmiany rozmiaru okna

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}