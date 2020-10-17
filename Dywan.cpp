#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <ctime>
#include <iostream>
#include <cstdlib>

using namespace std;

int poziomy; //Liczba poziomów dywanu 

float rnd_kolor() // funkcja generuj¹ca losowy kolor  
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX); 
}

float rnd_nierownosc(float mnoznik) // funkcja generuj¹ca losow¹ nierównoœæ ("poturbowanie" dywanu)
{
	return mnoznik * 0.02 * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
}

void rysujKwadrat(float x, float y, float rozmiar) //  funkcja rysuj¹ca pojedynczy kwadrat
{
	glRectf(x, y, x + rozmiar, y - rozmiar);
	glColor3f(rnd_kolor(), rnd_kolor(), rnd_kolor()); // ustalanie koloru z u¿yciem funkcji losuj¹cej		
}

void rysujDywan(float x, float y, float rozmiar, float obecnyPoziom) // "g³ówna" funkcja, tworz¹ca Dywan Sierpiñskiego
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if ((i == 1) && (j == 1)) // sprawdzenie czy œrodek rozwa¿¹nego kwadratu
			{ }
			else
			{
				if (obecnyPoziom > poziomy - 2) //sprawdzenie czy funkcja jest na wymaganym poziomie :
				{	// jeœli tak, to tysowane s¹ kwadraty
					rysujKwadrat(x + i*(rozmiar/3) + rnd_nierownosc(rozmiar), y - j*(rozmiar/3) + rnd_nierownosc(rozmiar), rozmiar/3);
				}
				else
				{	//jeœli nie, ponowne wywo³anie funkcji rysuj¹cej dywan - zag³êbienie siê o jeden poziom dalej
					rysujDywan(x + i*(rozmiar/3) + rnd_nierownosc(rozmiar), y - j*(rozmiar/3) + rnd_nierownosc(rozmiar), rozmiar/3, obecnyPoziom+1);
				}
			}

		}

	}
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT); 
	// Czyszczenie okna aktualnym kolorem czyszcz¹cym

	rysujDywan(-50.0f, 50.0f, 100.0f, 0); // Wywo³anie stworzonej funkcji, rysuj¹cej Dywan Sierpiñskiego

	glFlush(); 
	// Przekazanie poleceñ rysuj¹cych do wykonania
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	GLfloat AspectRatio; 
	// Deklaracja zmiennej AspectRatio okreœlaj¹cej proporcjê wymiarów okna

	if (vertical == 0) vertical = 1;
	// Zabezpieczenie pzred dzieleniem przez 0

	glViewport(0, 0, horizontal, vertical); 
	// Ustawienie wielkoœciokna okna urz¹dzenia (Viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)

	glMatrixMode(GL_PROJECTION); 
	// Okreœlenie uk³adu wspó³rzêdnych obserwatora

	glLoadIdentity();  
	// Okreœlenie przestrzeni ograniczaj¹cej

	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical; 
	// Wyznaczenie wspó³czynnika proporcji okna

	if (horizontal <= vertical)
		glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);

	else
		glOrtho(-100.0 * AspectRatio, 100.0 * AspectRatio, -100.0, 100.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW); 
	// Okreœlenie uk³adu wspó³rzêdnych

	glLoadIdentity();
}

int main(int argc, char** argv)
{
	cout<<"Podaj liczbe poziomow dywanu: ";

	cin>>poziomy; 
	// Wczytanie rz¹danej liczby poziomów

	srand(time(NULL));
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);   
	// Ustawienie trybu wyœwietlania
	// GLUT_SINGLE - pojedynczy bufor wyœwietlania
    // GLUT_RGBA - model kolorów RGB

	glutCreateWindow("Grafika komputerowa - Dywan Sierpiñskiego");
	// Utworzenie okna i okreœlenie treœci napisu w nag³ówku okna

	glutDisplayFunc(RenderScene); 
	// Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹ (callback)

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹ za zmiany rozmiaru okna

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}