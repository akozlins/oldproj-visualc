
#include <stdio.h>
#include <stdlib.h>

#define STRICT 1
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include <gl/gl.h>

const unsigned int WIDTH = 512;
const unsigned int HEIGHT = 512;

unsigned __int8 cells[HEIGHT][WIDTH] = { 0 };
unsigned __int8 cells_[HEIGHT][WIDTH] = { 0 };

HDC hdc;
HGLRC hglrc;

LRESULT CALLBACK fproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
  switch(msg)
  {
  case WM_KEYDOWN:
    switch(wp)
    {
    case 'q':
    case 'Q':
      PostMessage(hwnd, WM_CLOSE, 0, 0);
      break;
    case 'r':
    case 'R':
      for(int y = 0; y < HEIGHT; y++) for(int x = 0; x < WIDTH; x++)
      {
        cells[y][x] = (11 *  rand()) / (10 * RAND_MAX);
      }
      break;
    case 'g':
    case 'G':
      memset(cells, 0, WIDTH * HEIGHT);
      cells[HEIGHT / 2 + 0][WIDTH / 2 + 0] = 1;
      cells[HEIGHT / 2 + 1][WIDTH / 2 + 0] = 1;
      cells[HEIGHT / 2 + 2][WIDTH / 2 + 0] = 1;
      cells[HEIGHT / 2 + 2][WIDTH / 2 + 1] = 1;
      cells[HEIGHT / 2 + 1][WIDTH / 2 - 1] = 1;
      break;
    } // switch(wp)
    break; // case WM_KEYDOWN
  case WM_DESTROY:
    PostQuitMessage(0);
    break; // case WM_DESTROY
  }

  return DefWindowProc(hwnd, msg, wp, lp);
}

const char* g_title = "opengl_life_app";

int CALLBACK WinMain(HINSTANCE hinst, HINSTANCE hprev, LPSTR cmd, int show)
{
//  AllocConsole();
//  AttachConsole(GetCurrentProcessId());
#pragma warning(push)
#pragma warning( disable : 4996 )
//  freopen("CON", "w", stdout);
#pragma warning(pop)

  WNDCLASSEX wc;
  memset(&wc, 0, sizeof(wc));
  wc.cbSize        = sizeof(wc);
  wc.style         = CS_VREDRAW | CS_HREDRAW;
  wc.lpfnWndProc   = fproc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor       = LoadCursor(0, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName  = 0;
  wc.lpszClassName = g_title;
  wc.hInstance     = hinst;
  wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

  if(!RegisterClassEx(&wc)) return 0;

  RECT rect; SetRect(&rect, 0, 0, WIDTH, HEIGHT);
  AdjustWindowRectEx(&rect, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE, WS_EX_CLIENTEDGE);

  HWND hwnd = CreateWindowEx(
    WS_EX_CLIENTEDGE,
    g_title, g_title,
    WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    0, 0, rect.right - rect.left, rect.bottom - rect.top,
    0, 0, hinst, 0);

  HDC hdc = GetDC(hwnd);
  PIXELFORMATDESCRIPTOR pfd;
  memset(&pfd, 0, sizeof(pfd));
  pfd.nSize = sizeof(pfd);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;
  pfd.cColorBits = 24;
  pfd.cDepthBits = 16;
  int fmt = ChoosePixelFormat(hdc, &pfd);
  SetPixelFormat(hdc, fmt, &pfd);
  HGLRC hglrc = wglCreateContext(hdc);
  wglMakeCurrent(hdc, hglrc);

  glViewport(0, 0, WIDTH, HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, WIDTH, HEIGHT, 0, 0, 1);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.5, 0.5, 0);

  ShowWindow(hwnd, show);
  UpdateWindow(hwnd);

  __int64 it = 0;

  MSG msg = { 0 };
  while(true)
  {
    while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE) && msg.message != WM_QUIT)
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    if(msg.message == WM_QUIT) break;

    for(int x = 1; x < WIDTH - 1; x++)
    {
      cells[0][x] = cells[HEIGHT - 2][x];
      cells[HEIGHT - 1][x] = cells[1][x];
    }
    for(int y = 1; y < HEIGHT - 1; y++)
    {
      cells[y][0] = cells[y][WIDTH - 2];
      cells[y][WIDTH - 1] = cells[y][1];
    }
    cells[0][0] = cells[HEIGHT - 2][WIDTH - 2];
    cells[HEIGHT - 1][WIDTH - 1] = cells[1][1];
    cells[HEIGHT - 1][0] = cells[1][WIDTH - 2];
    cells[0][WIDTH - 1] = cells[HEIGHT - 2][1];

    for(int y = 1; y < HEIGHT - 1; y++) for(int x = 1; x < WIDTH - 1; x++)
    {
      int n = cells[y - 1][x - 1] + cells[y - 1][x] + cells[y - 1][x + 1] +
              cells[y    ][x - 1] +                   cells[y    ][x + 1] +
              cells[y + 1][x - 1] + cells[y + 1][x] + cells[y + 1][x + 1];
      cells_[y][x] = (n == 3) + (n == 2) * cells[y][x];
    }
    memcpy(cells, cells_, WIDTH * HEIGHT);

    if(it++ % 1 == 0)
    {
      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT);

      glColor3d(1, 1, 1);
      glBegin(GL_POINTS);
      for(int y = 1; y < HEIGHT - 1; y++) for(int x = 1; x < WIDTH - 1; x++)
      {
        if(cells[y][x])
        {
          glVertex2d(x, y);
        }
      }
      glEnd();
      glFlush();

      SwapBuffers(hdc);
    }
  }

  wglMakeCurrent(hdc, NULL);
  wglDeleteContext(hglrc);
  ReleaseDC(hwnd, hdc);

  return 0;
}

//---------------------------------------------------------------------------
