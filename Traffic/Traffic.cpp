// Traffic.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Traffic.h"

#include <windows.h>
#include <windowsx.h>
#include <d2d1.h>
#include <random>
#pragma comment(lib, "d2d1")
#include "basewin.h"

using namespace std;

int light = 0; //light state
int index = 0; 
FLOAT spawn[4][4] {0}; //car spawn coords
random_device ranNum; //used to randomly generate the positions of cars, and the placement of the roads
FLOAT ran1 = ranNum() % 400 + 300;
FLOAT ran2 = ranNum() % 400 + 300;
int spawnpoint[20] = {}; //keeps track of spawn road/direction
int turn[20] = {}; //generates whether car will turn
int spawncheck[4] = {}; //checks to see if spawn point is open
int tick = 0; //used to change lights
int rcoord, rturn;

void setSpawn() //sets the spawning points for the cars
{
    spawn[0][0] = 20.0f;
    spawn[0][1] = ran1 + 30.0f;
    spawn[0][2] = 80.0f;
    spawn[0][3] = ran1 + 70.0f;
    spawn[1][0] = 920.0f;
    spawn[1][1] = ran1 - 70.0f;
    spawn[1][2] = 980.0f;
    spawn[1][3] = ran1 - 30.0f;
    spawn[2][0] = ran2 - 70.0f;
    spawn[2][1] = 20.0f;
    spawn[2][2] = ran2 - 30.0f;
    spawn[2][3] = 80.0f;
    spawn[3][0] = ran2 + 30.0f;
    spawn[3][1] = 920.0f;
    spawn[3][2] = ran2 + 70.0f;
    spawn[3][3] = 980.0f;
}

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

class MainWindow : public BaseWindow<MainWindow>
{
    ID2D1Factory* pFactory;
    ID2D1HwndRenderTarget* pRenderTarget;
    ID2D1SolidColorBrush* pBrush;
    D2D1_ELLIPSE            light1, light2, light3, light4, light5, light6;
    D2D1_RECT_F             rect1, rect2, box;
    D2D1_RECT_F             cars[20]{};

    void    CalculateLayout() { }
    HRESULT CreateGraphicsResources();
    void    DiscardGraphicsResources();
    void    CreateRoads(int direction, FLOAT ran);
    void    UpdateLights();
    void    UpdateCars();
    void    UpdateBox();
    void    OnPaint();

public:
    void    Update();
    void    Render();

    MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL),
    rect1(), rect2(), box(), light1(), light2(), light3(), light4(), light5(), light6()
    {}
    PCWSTR  ClassName() const { return L"Traffic Window Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

void MainWindow::CreateRoads(int direction, FLOAT ran)
{
    FLOAT left = 0.0f, top = 0.0f, right = 0.0f, bottom = 0.0f;
    if (direction == 1) //horizontal road
    {
        left = 0.0f;
        top = ran - 100.0f;
        right = 1000.0f;
        bottom = ran + 100.0f;
        rect1 = D2D1::RectF(left, top, right, bottom);
    }
    if (direction == 2) //vertical road
    {
        left = ran - 100.0f;
        top = 0.0f;
        right = ran + 100.0f;
        bottom = 1000.0f;
        rect2 = D2D1::RectF(left, top, right, bottom);
    }
    box = D2D1::RectF(ran2 - 110.0f, ran1 - 110.0f, ran2 + 110.0f, ran1 + 110.0f);
}

void MainWindow::UpdateLights()
{   //cycles through the light states
    if (tick <= 799)
    {
        tick++;
        light = 0; //horizontal road green, vertical road red
        light1 = D2D1::Ellipse(D2D1::Point2F(ran2 - 100.0f, ran1), 13.0f, 13.0f);
        light2 = D2D1::Ellipse(D2D1::Point2F(ran2 + 100.0f, ran1), 13.0f, 13.0f);
        light3 = D2D1::Ellipse(D2D1::Point2F(ran2, ran1 - 100.0f), 13.0f, 13.0f);
        light4 = D2D1::Ellipse(D2D1::Point2F(ran2, ran1 + 100.0f), 13.0f, 13.0f);
        light5 = D2D1::Ellipse(D2D1::Point2F(ran2 - 100.0f, ran1), 0.0f, 0.0f);
        light6 = D2D1::Ellipse(D2D1::Point2F(ran2 + 100.0f, ran1), 0.0f, 0.0f);
    }
    else if (tick <= 1199)
    {
        tick++;
        light = 1; //horizontal road yellow, vertical road red
        light1 = D2D1::Ellipse(D2D1::Point2F(ran2 - 100.0f, ran1), 0.0f, 0.0f);
        light2 = D2D1::Ellipse(D2D1::Point2F(ran2 + 100.0f, ran1), 0.0f, 0.0f);
        light3 = D2D1::Ellipse(D2D1::Point2F(ran2, ran1 - 100.0f), 13.0f, 13.0f);
        light4 = D2D1::Ellipse(D2D1::Point2F(ran2, ran1 + 100.0f), 13.0f, 13.0f);
        light5 = D2D1::Ellipse(D2D1::Point2F(ran2 - 100.0f, ran1), 13.0f, 13.0f);
        light6 = D2D1::Ellipse(D2D1::Point2F(ran2 + 100.0f, ran1), 13.0f, 13.0f);
    }
    else if (tick <= 1999)
    {
        tick++;
        light = 2; //horizontal road red, vertical road green
        light1 = D2D1::Ellipse(D2D1::Point2F(ran2, ran1 - 100.0f), 13.0f, 13.0f);
        light2 = D2D1::Ellipse(D2D1::Point2F(ran2, ran1 + 100.0f), 13.0f, 13.0f);
        light3 = D2D1::Ellipse(D2D1::Point2F(ran2 - 100.0f, ran1), 13.0f, 13.0f);
        light4 = D2D1::Ellipse(D2D1::Point2F(ran2 + 100.0f, ran1), 13.0f, 13.0f);
        light5 = D2D1::Ellipse(D2D1::Point2F(ran2, ran1 - 100.0f), 0.0f, 0.0f);
        light6 = D2D1::Ellipse(D2D1::Point2F(ran2, ran1 + 100.0f), 0.0f, 0.0f);
    }
    else if (tick <= 2399)
    {
        tick++;
        light = 3; //horizontal road red, vertical road yellow
        light1 = D2D1::Ellipse(D2D1::Point2F(ran2, ran1 - 100.0f), 0.0f, 0.0f);
        light2 = D2D1::Ellipse(D2D1::Point2F(ran2, ran1 + 100.0f), 0.0f, 0.0f);
        light3 = D2D1::Ellipse(D2D1::Point2F(ran2 - 100.0f, ran1), 13.0f, 13.0f);
        light4 = D2D1::Ellipse(D2D1::Point2F(ran2 + 100.0f, ran1), 13.0f, 13.0f);
        light5 = D2D1::Ellipse(D2D1::Point2F(ran2, ran1 - 100.0f), 13.0f, 13.0f);
        light6 = D2D1::Ellipse(D2D1::Point2F(ran2, ran1 + 100.0f), 13.0f, 13.0f);
    }
    if (tick >= 2400) //reset the ticker
    {
        tick = 0;
    }
}

void MainWindow::UpdateCars()
{
    if (tick % 300 == 0) //every 300 ticks a car will spawn
    {
        if (cars[index].bottom == NULL || cars[index].top == NULL || cars[index].left == NULL || cars[index].right == NULL)
        {
            int rcoord = ranNum() % 4;
            int rturn = ranNum() % 10;
            if (rturn < 10)
            {
                turn[index] = 0;
            }
            if (rturn < 4)
            {
                turn[index] = 1; // ~40% of cars will turn
            }
            if (spawncheck[rcoord] != 1) //spawn if there is no car in the spawnpoint
            {
                cars[index] = D2D1::RectF(spawn[rcoord][0], spawn[rcoord][1], spawn[rcoord][2], spawn[rcoord][3]);
                spawnpoint[index] = rcoord;
                index++;
            }
            if (index == 19) //20 max cars
            {
                index = 0;
            }
        }
        if (cars[index].bottom < 0 || cars[index].top < 0 || cars[index].left < 0 || cars[index].right < 0 || cars[index].bottom > 1000 || cars[index].top > 1000 || cars[index].left > 1000 || cars[index].right > 1000) //NULL cars that are at the bounds of screen
        {
            cars[index].bottom = NULL;
            cars[index].top = NULL;
            cars[index].left = NULL;
            cars[index].right = NULL;
        }
    }
 
    for (int i = 0; i < 20; i++) //cycle through all the cars and update their positions
    {
        if (spawnpoint[i] == 0) //random number from 0 to 3, determines which road to spawn to
        {
            if (light == 0) //light is 0 to 3, which state the lights are in stated earlier, 0 = green for this road
            {
                if (turn[i] == 1) //if turn is true, the car will turn
                {
                    if (box.left - cars[i].right == -100.0f) //if the car turns, it will turn when it aligns with the new road, then switch to that spawnpoint
                    {
                        cars[i].right = cars[i].right - 20.0f;
                        cars[i].bottom = cars[i].bottom + 20.0f;
                        spawnpoint[i] = 2;
                        break;
                    }
                    else //if it hasn't reached that point yet, the car will continue moving
                    {
                        cars[i].left = cars[i].left + 0.5f;
                        cars[i].right = cars[i].right + 0.5f;
                    }
                }
                else //if the car isn't turning, it will continue moving
                {
                    cars[i].left = cars[i].left + 0.5f; 
                    cars[i].right = cars[i].right + 0.5f;
                }
            }
            if (light == 1 || light == 2 || light == 3) //1 and 3 are yellow, 2 is red for this road
            {
                if (box.left - cars[i].right <= 20.0f) //when the car is 20 ticks behind the box constraint
                {
                    if (ran2 - cars[i].right <= 110.0f) //if it is past the light, keep going
                    {
                            cars[i].left = cars[i].left + 0.5f;
                            cars[i].right = cars[i].right + 0.5f;
                    }
                    else //if it is not past the light, stop and adjust the box constraint
                    {
                        if (cars[i].left != NULL)
                        {
                            box.left = cars[i].left;
                        }
                    }
                }
                else //move the car until we have approched the box constraint
                {
                    cars[i].left = cars[i].left + 0.5f;
                    cars[i].right = cars[i].right + 0.5f;
                }
            }
        }
        if (spawnpoint[i] == 1) //repeat for the remaining spawns:
        {
            if (light == 0)
            {
                if (turn[i] == 1)
                {
                    if (cars[i].left - box.right == -100.0f)
                    {
                        cars[i].left = cars[i].left + 20.0f;
                        cars[i].top = cars[i].top - 20.0f;
                        spawnpoint[i] = 3;
                        break;
                    }
                    else
                    {
                        cars[i].left = cars[i].left - 0.5f;
                        cars[i].right = cars[i].right - 0.5f;
                    }
                }
                else
                {
                    cars[i].left = cars[i].left - 0.5f;
                    cars[i].right = cars[i].right - 0.5f;
                }
            }
            if (light == 1 || light == 2 || light == 3)
            {
                if (cars[i].left - box.right <= 20.0f)
                {
                    if (cars[i].left - ran2 <= 110.0f)
                    {
                        cars[i].left = cars[i].left - 0.5f;
                        cars[i].right = cars[i].right - 0.5f;
                    }
                    else
                    {
                        if (cars[i].right != NULL)
                        {
                            box.right = cars[i].right;
                        }
                    }
                }
                else
                {
                    cars[i].left = cars[i].left - 0.5f;
                    cars[i].right = cars[i].right - 0.5f;
                }
            }
        }
        if (spawnpoint[i] == 2)
        {
            if (light == 0 || light == 1 || light == 3)
            {
                if (box.top - cars[i].bottom <= 20.0f)
                {
                    if (ran1 - cars[i].bottom <= 110.0f)
                    {
                        cars[i].top = cars[i].top + 0.5;
                        cars[i].bottom = cars[i].bottom + 0.5;
                    }
                    else
                    {
                        if (cars[i].top != NULL)
                        {
                            box.top = cars[i].top;
                        }
                    }
                }
                else
                {
                    cars[i].top = cars[i].top + 0.5;
                    cars[i].bottom = cars[i].bottom + 0.5;
                }
            }
            if (light == 2)
            {
                if (turn[i] == 1)
                {
                    if (box.top - cars[i].bottom == -100.0f)
                    {
                        cars[i].bottom = cars[i].bottom - 20.0f;
                        cars[i].left = cars[i].left - 20.0f;
                        spawnpoint[i] = 1;
                        break;
                    }
                    else
                    {
                        cars[i].top = cars[i].top + 0.5;
                        cars[i].bottom = cars[i].bottom + 0.5;
                    }
                }
                else
                {
                    cars[i].top = cars[i].top + 0.5;
                    cars[i].bottom = cars[i].bottom + 0.5;
                }
                
            }
        }
        if (spawnpoint[i] == 3)
        {
            if (light == 0 || light == 1 || light == 3)
            {
                if (cars[i].top - box.bottom <= 20.0f)
                {
                    if (cars[i].top - ran1 <= 110.0f)
                    {
                        cars[i].top = cars[i].top - 0.5;
                        cars[i].bottom = cars[i].bottom - 0.5;
                    }
                    else
                    {
                        if (cars[i].bottom != NULL)
                        {
                            box.bottom = cars[i].bottom;
                        }
                    }
                }
                else
                {
                    cars[i].top = cars[i].top - 0.5;
                    cars[i].bottom = cars[i].bottom - 0.5;
                }
            }
            if (light == 2)
            {
                if (turn[i] == 1)
                {
                    if (cars[i].top - box.bottom == -100.0f)
                    {
                        cars[i].top = cars[i].top + 20.0f;
                        cars[i].right = cars[i].right + 20.0f;
                        spawnpoint[i] = 0;
                        break;
                    }
                    else
                    {
                        cars[i].top = cars[i].top - 0.5;
                        cars[i].bottom = cars[i].bottom - 0.5;
                    }
                }
                else
                {
                    cars[i].top = cars[i].top - 0.5;
                    cars[i].bottom = cars[i].bottom - 0.5;
                }
            }
        }
    }
    if (spawn[0][2] - box.left > -20.0f) //spawn check will be 1 when a car is in the spawn area, and cars will not spawn there
    {
        spawncheck[0] = 1;
    }
    else
    {
       spawncheck[0] = 0;
    }
    if (box.right - spawn[1][0] > -20.0f)
    {
        spawncheck[1] = 1;
    }
    else
    {
        spawncheck[1] = 0;
    }
    if (spawn[2][3] - box.top > -20.0f)
    {
        spawncheck[2] = 1;
    }
    else
    {
        spawncheck[2] = 0;
    }
    if (box.bottom - spawn[3][1] > -20.0f)
    {
        spawncheck[3] = 1;
    }
    else
    {
        spawncheck[3] = 0;
    }
}

void MainWindow::UpdateBox() //update the position of the box constraint
{
    if (box.left < ran2 - 110.0f)
    {
        box.left = box.left + 0.5f;
    }
    if (box.right > ran2 + 110.0f)
    {
        box.right = box.right - 0.5f;
    }
    if (box.top < ran1 - 110.0f)
    {
        box.top = box.top + 0.5f;
    }
    if (box.bottom > ran1 + 110.0f)
    {
        box.bottom = box.bottom - 0.5f;
    }
}

HRESULT MainWindow::CreateGraphicsResources() 
{

    HRESULT hr = S_OK;
    if (pRenderTarget == NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &pRenderTarget);

        if (SUCCEEDED(hr))
        {
            D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 1.0f);
            hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

            if (SUCCEEDED(hr))
            {
                CalculateLayout();
            }
        }
    }
    return hr;
}

void MainWindow::DiscardGraphicsResources()
{
    SafeRelease(&pRenderTarget);
    SafeRelease(&pBrush);
}

void MainWindow::OnPaint() //Paint all shapes after each update
{
    HRESULT hr = CreateGraphicsResources();
    if (SUCCEEDED(hr))
    {
        PAINTSTRUCT ps;
        BeginPaint(m_hwnd, &ps);
        pRenderTarget->BeginDraw();
        pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::MediumSeaGreen));
        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBrush);
        pRenderTarget->FillRectangle(rect1, pBrush);
        pRenderTarget->FillRectangle(rect2, pBrush);
        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &pBrush);
        pRenderTarget->FillEllipse(light1, pBrush);
        pRenderTarget->FillEllipse(light2, pBrush);
        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pBrush);
        pRenderTarget->FillEllipse(light3, pBrush);
        pRenderTarget->FillEllipse(light4, pBrush);
        //pRenderTarget->DrawRectangle(box, pBrush); //                                             <--------------------------  Uncomment to see the box constraint
        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &pBrush);
        pRenderTarget->FillEllipse(light5, pBrush);
        pRenderTarget->FillEllipse(light6, pBrush);
        pRenderTarget->FillRectangle(cars[0], pBrush);
        pRenderTarget->FillRectangle(cars[1], pBrush);
        pRenderTarget->FillRectangle(cars[2], pBrush);
        pRenderTarget->FillRectangle(cars[3], pBrush);
        pRenderTarget->FillRectangle(cars[4], pBrush);
        pRenderTarget->FillRectangle(cars[5], pBrush);
        pRenderTarget->FillRectangle(cars[6], pBrush);
        pRenderTarget->FillRectangle(cars[7], pBrush);
        pRenderTarget->FillRectangle(cars[8], pBrush);
        pRenderTarget->FillRectangle(cars[9], pBrush);
        pRenderTarget->FillRectangle(cars[10], pBrush);
        pRenderTarget->FillRectangle(cars[11], pBrush);
        pRenderTarget->FillRectangle(cars[12], pBrush);
        pRenderTarget->FillRectangle(cars[13], pBrush);
        pRenderTarget->FillRectangle(cars[14], pBrush);
        pRenderTarget->FillRectangle(cars[15], pBrush);
        pRenderTarget->FillRectangle(cars[16], pBrush);
        pRenderTarget->FillRectangle(cars[17], pBrush);
        pRenderTarget->FillRectangle(cars[18], pBrush);
        pRenderTarget->FillRectangle(cars[19], pBrush);
        hr = pRenderTarget->EndDraw();
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(m_hwnd, &ps);
    }
}

void MainWindow::Update()
{
    UpdateLights();
    UpdateCars();
    UpdateBox();
}

void MainWindow::Render()
{
    OnPaint();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    MainWindow win;
    if (!win.Create(L"Traffic Simulation", WS_OVERLAPPEDWINDOW))
    {
        return 0;
    }

    ShowWindow(win.Window(), nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            win.Update();
            win.Render();
        }
    }

    return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        if (FAILED(D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
        {
            return -1;  // Fail CreateWindowEx.
        }
        setSpawn();
        CreateRoads(1, ran1);
        CreateRoads(2, ran2);
        return 0;

    case WM_DESTROY:
        DiscardGraphicsResources();
        SafeRelease(&pFactory);
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        OnPaint();
        return 0;

    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}