#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <ctime>
#include <iostream>
#include <cstdlib>

using namespace std;

int poziomy; //Liczba poziomów dywanu 

float rnd_kolor() // funkcja generująca losowy kolor  
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX); 
}

float rnd_nierownosc(float mnoznik) // funkcja generująca losową nierówność ("poturbowanie" dywanu)
{
	return mnoznik * 0.02 * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
}

void rysujKwadrat(float x, float y, float rozmiar) //  funkcja rysująca pojedynczy kwadrat
{
	glRectf(x, y, x + rozmiar, y - rozmiar);
	glColor3f(rnd_kolor(), rnd_kolor(), rnd_kolor()); // ustalanie koloru z użyciem funkcji losującej		
}

void rysujDywan(float x, float y, float rozmiar, float obecnyPoziom) // "główna" funkcja, tworząca Dywan Sierpińskiego
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if ((i == 1) && (j == 1)) // sprawdzenie czy środek rozważąnego kwadratu
			{ }
			else
			{
				if (obecnyPoziom > poziomy - 2) //sprawdzenie czy funkcja jest na wymaganym poziomie :
				{	// jeśli tak, to tysowane są kwadraty
					rysujKwadrat(x + i*(rozmiar/3) + rnd_nierownosc(rozmiar), y - j*(rozmiar/3) + rnd_nierownosc(rozmiar), rozmiar/3);
				}
				else
				{	//jeśli nie, ponowne wywołanie funkcji rysującej dywan - zagłębienie się o jeden poziom dalej
					rysujDywan(x + i*(rozmiar/3) + rnd_nierownosc(rozmiar), y - j*(rozmiar/3) + rnd_nierownosc(rozmiar), rozmiar/3, obecnyPoziom+1);
				}
			}

		}

	}
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT); 
	// Czyszczenie okna aktualnym kolorem czyszczącym

	rysujDywan(-50.0f, 50.0f, 100.0f, 0); // Wywołanie stworzonej funkcji, rysującej Dywan Sierpińskiego

	glFlush(); 
	// Przekazanie poleceń rysujących do wykonania
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	GLfloat AspectRatio; 
	// Deklaracja zmiennej AspectRatio określającej proporcję wymiarów okna

	if (vertical == 0) vertical = 1;
	// Zabezpieczenie pzred dzieleniem przez 0

	glViewport(0, 0, horizontal, vertical); 
	// Ustawienie wielkościokna okna urządzenia (Viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)

	glMatrixMode(GL_PROJECTION); 
	// Określenie układu współrzędnych obserwatora

	glLoadIdentity();  
	// Określenie przestrzeni ograniczającej

	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical; 
	// Wyznaczenie współczynnika proporcji okna

	if (horizontal <= vertical)
		glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);

	else
		glOrtho(-100.0 * AspectRatio, 100.0 * AspectRatio, -100.0, 100.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW); 
	// Określenie układu współrzędnych

	glLoadIdentity();
}

int main(int argc, char** argv)
{
	cout<<"Podaj liczbe poziomow dywanu: ";

	cin>>poziomy; 
	// Wczytanie rządanej liczby poziomów

	srand(time(NULL));
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);   
	// Ustawienie trybu wyświetlania
	// GLUT_SINGLE - pojedynczy bufor wyświetlania
    // GLUT_RGBA - model kolorów RGB

	glutCreateWindow("Grafika komputerowa - Dywan Sierpińskiego");
	// Utworzenie okna i określenie treści napisu w nagłówku okna

	glutDisplayFunc(RenderScene); 
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną (callback)

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną za zmiany rozmiaru okna

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}
