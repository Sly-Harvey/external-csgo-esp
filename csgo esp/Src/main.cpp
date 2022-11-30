#include <iostream>
#include <GLFW/glfw3.h>
#include "memory.h"
#include "helper.h"
#include "offsets.h"

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

int updatesPerSecond = 30;
bool toggle = false;

int main(void)
{
    int procId = GetProcessId(L"csgo.exe");
    while (!procId)
    {
        procId = GetProcessId(L"csgo.exe");
        system("cls");
        std::cout << "Game not found." << std::endl;
        Sleep(300);
    }
    system("cls");
    uintptr_t moduleBase = GetModuleBaseAddress(procId, L"client.dll");
    uintptr_t playerBase = moduleBase + offsets::aLocalPlayer;

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

    std::vector<unsigned int> teamOffsets{ offsets::oTeamNum };
    uintptr_t teamAddress = FindDMAAddy(hProc, playerBase, teamOffsets);


    std::cout << "Game Found!" << std::endl;
    std::cout << "Starting the program." << std::endl;
    Sleep(3000);
    system("cls");
    std::cout << "Program Started." << std::endl;

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_FLOATING, true);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
    glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, true);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "CSGO ESP", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


        ViewMatrix matrix = Memory::Read<ViewMatrix>(hProc, (moduleBase + offsets::aViewMatrix));

        for (int i = 0; i < 64; ++i)
        {
            uintptr_t entity = Memory::Read<uintptr_t>(hProc, (moduleBase + offsets::aEntityList) + i * 0x10);

            if (entity == NULL) continue;

            uintptr_t health = Memory::Read<uintptr_t>(hProc, (entity + offsets::oHealth));

            if (health <= 0) continue;

            uintptr_t entityTeam = Memory::Read<uintptr_t>(hProc, entity + offsets::oTeamNum);

            uintptr_t localTeam = Memory::Read<uintptr_t>(hProc, teamAddress);

            Vector3 vectorOrigin = Memory::Read<Vector3>(hProc, entity + offsets::oVecOrigin);
            Vector3 headOrigin;
            headOrigin.X = vectorOrigin.X;
            headOrigin.Y = vectorOrigin.Y;
            headOrigin.Z = vectorOrigin.Z + 75.0f;

            Vector2 entityHeadCoords;
            Vector2 entityFeetCoords;

            if (!WorldToScreen(vectorOrigin, entityFeetCoords, matrix.Matrix)) continue;

            if (!WorldToScreen(headOrigin, entityHeadCoords, matrix.Matrix)) continue;

            float height = entityHeadCoords.Y - entityFeetCoords.Y;
            float width = height / 3.8f;

            if (entityTeam != localTeam)
            {
                glBegin(GL_LINES);
                glColor3f(0.0f, 1.0f, 0.0f);
                glVertex2f(0.0f, -1.0f);
                glVertex2f(entityFeetCoords.X, entityFeetCoords.Y);
                //glEnd();

                //glBegin(GL_LINES);
                glVertex2f(entityFeetCoords.X - (width / 2), entityFeetCoords.Y);
                glVertex2f(entityFeetCoords.X - (width / 2), entityHeadCoords.Y);
                //glEnd();

                //glBegin(GL_LINES);
                glVertex2f(entityFeetCoords.X - (width / 2), entityHeadCoords.Y);
                glVertex2f(entityFeetCoords.X + (width / 2), entityHeadCoords.Y);
                //glEnd();

                //glBegin(GL_LINES);
                glVertex2f(entityFeetCoords.X + (width / 2), entityHeadCoords.Y);
                glVertex2f(entityFeetCoords.X + (width / 2), entityFeetCoords.Y);
                //glEnd();

                //glBegin(GL_LINES);
                glVertex2f(entityFeetCoords.X + (width / 2), entityFeetCoords.Y);
                glVertex2f(entityFeetCoords.X - (width / 2), entityFeetCoords.Y);
                glEnd();
            }
            else
            {
                glBegin(GL_LINES);
                glColor3f(0.0f, 0.0f, 1.0f);
                glVertex2f(0.0f, -1.0f);
                glVertex2f(entityFeetCoords.X, entityFeetCoords.Y);
                glEnd();

                glBegin(GL_LINES);
                glVertex2f(entityFeetCoords.X - (width / 2), entityFeetCoords.Y);
                glVertex2f(entityFeetCoords.X - (width / 2), entityHeadCoords.Y);
                glEnd();

                glBegin(GL_LINES);
                glVertex2f(entityFeetCoords.X - (width / 2), entityHeadCoords.Y);
                glVertex2f(entityFeetCoords.X + (width / 2), entityHeadCoords.Y);
                glEnd();

                glBegin(GL_LINES);
                glVertex2f(entityFeetCoords.X + (width / 2), entityHeadCoords.Y);
                glVertex2f(entityFeetCoords.X + (width / 2), entityFeetCoords.Y);
                glEnd();

                glBegin(GL_LINES);
                glVertex2f(entityFeetCoords.X + (width / 2), entityFeetCoords.Y);
                glVertex2f(entityFeetCoords.X - (width / 2), entityFeetCoords.Y);
                glEnd();
            }
        }


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        Sleep(1000 / updatesPerSecond);
    }

    glfwTerminate();
    return 0;
}