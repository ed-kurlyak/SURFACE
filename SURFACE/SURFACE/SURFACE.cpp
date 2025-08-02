#include <math.h>
#include <stdio.h>
#include <windows.h>

HWND g_hWnd = NULL;

const int win_x1 = 0;
const int win_y1 = 0;
const int win_x2 = 799;
const int win_y2 = 599;
const double Big = 9.999999E+10;
const double Margin = 0.1;

double xMin, xMax, yMin, yMax;
int xCount, yCount;
double x, y, z, dx, dy, Ax, Ay, Bx, By;
double dxMin, dxMax, dyMin, dyMax;
double xSize, ySize;
int xOld, yOld, xNew, yNew;
double xStep, yStep;
double Rad, Theta, Phi, D;
double sinT, cosT, sinP, cosP;

double Fun(double x, double y)
{
	return cos(sqrt(x * x + y * y));
}

void FindEyeCoordinates()
{
	z = Fun(x, y);
	double xx = x * sinT + y * cosT;
	double yy = x * cosT * cosP - y * sinT * cosP + z * sinP;
	double zz = x * cosT * sinP - y * sinT * sinP - z * cosP + Rad;
	dx = D * xx / zz;
	dy = D * yy / zz;
}

void FindScreenCoordinates()
{
	xNew = (int)(Ax + Bx * dx);
	yNew = win_y2 - (int)(Ay + By * dy);
}

void FindLimits()
{
	if (dx > dxMax)
		dxMax = dx;
	if (dx < dxMin)
		dxMin = dx;
	if (dy > dyMax)
		dyMax = dy;
	if (dy < dyMin)
		dyMin = dy;
}

void FindWindow()
{
	xSize = dxMax - dxMin;
	ySize = dyMax - dyMin;

	dxMin = dxMin - Margin * xSize;
	dxMax = dxMax + Margin * xSize;
	dyMin = dyMin - Margin * ySize;
	dyMax = dyMax + Margin * ySize;
	
	Bx = (win_x2 - win_x1) / (dxMax - dxMin);
	By = (win_y2 - win_y1) / (dyMax - dyMin);
	
	Ax = win_x1 - dxMin * Bx;
	Ay = win_y1 - dyMin * By;
}

void Draw_Surface()
{

	HDC hDC = GetDC(g_hWnd);
	
	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);


	// Инициализация параметров
	xCount = 20;
	yCount = 20;
	xMin = -5;
	xMax = 5;
	yMin = -5;
	yMax = 5;
	Rad = 30;
	Theta = 0.5;
	Phi = 0.6;
		D = 10;

	// Рисуем границу
	MoveToEx(hDC, 0, 0, NULL);
	LineTo(hDC, 799, 0);
	LineTo(hDC, 799, 599);
	LineTo(hDC, 0, 599);
	LineTo(hDC, 0, 0);

	sinT = sin(Theta);
	cosT = cos(Theta);
	sinP = sin(Phi);
	cosP = cos(Phi);
	xStep = (xMax - xMin) / xCount;
	yStep = (yMax - yMin) / yCount;
	dxMin = Big;
	dxMax = -Big;
	dyMin = Big;
	dyMax = -Big;

	for (int Show = 0; Show < 2; Show++)
	{
		for (int i = 0; i <= xCount; i++)
		{
			x = xMin + i * xStep;
			y = yMin;

			FindEyeCoordinates();

			if (Show)
			{
				FindScreenCoordinates();

				xOld = xNew;
				yOld = yNew;

				MoveToEx(hDC, xOld, yOld, NULL);
				LineTo(hDC, xOld, yOld);
			}
			else
			{
				FindLimits();
			}

			for (int j = 0; j <= yCount; j++)
			{
				y = yMin + j * yStep;
				FindEyeCoordinates();
				if (Show)
				{
					FindScreenCoordinates();
					LineTo(hDC, xNew, yNew);
					xOld = xNew;
					yOld = yNew;
				}
				else
				{
					FindLimits();
				}
			}
		}

		for (int i = 0; i <= yCount; i++)
		{
			y = yMin + i * yStep;
			x = xMin;

			FindEyeCoordinates();

			if (Show)
			{
				FindScreenCoordinates();

				xOld = xNew;
				yOld = yNew;

				MoveToEx(hDC, xOld, yOld, NULL);
				LineTo(hDC, xOld, yOld);
			}
			else
			{
				FindLimits();
			}

			for (int j = 0; j <= xCount; j++)
			{
				x = xMin + j * xStep;

				FindEyeCoordinates();

				if (Show)
				{
					FindScreenCoordinates();

					LineTo(hDC, xNew, yNew);

					xOld = xNew;
					yOld = yNew;
				}
				else
				{
					FindLimits();
				}
			}
		}

		if (!Show)
			FindWindow();
	}

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);

	ReleaseDC(g_hWnd, hDC);

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc = {0};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = "SurfaceWindowClass";

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
				   MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	g_hWnd =
		CreateWindow("SurfaceWindowClass", "3D Surface Plot",
					 WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
					 CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

	if (g_hWnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
				   MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	Draw_Surface();

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (GetAsyncKeyState(VK_ESCAPE) & 0xFF00)
			break;
	}

	return (int)msg.wParam;
}