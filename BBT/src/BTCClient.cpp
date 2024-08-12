#include <iostream>
#include "SDL3/SDL.h"
#include "utils/imgui/imgui.h"
#include "utils/imgui/imgui_impl_sdl3.h"
#include "utils/imgui/imgui_impl_opengl3.h"
#include "SDL3/SDL_opengl.h"
#include "bbt_log.h"
#include "bbt_assert.h"
#include "bbt_event_system.h"
#include "loguru.hpp"

#include <boost/asio.hpp>
#include <olc_net.h>

enum class CustomMsgTypes : uint32_t
{
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage,
};

class CustomClient : public olc::net::client_interface<CustomMsgTypes>
{
public:
    void PingServer()
    {
        olc::net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerPing;

        // Ojo esto.
        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

        msg << timeNow;
        Send(msg);
    }

    void MessageAll()
    {
        olc::net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::MessageAll;
        Send(msg);
    }
};

int main(int argc, char** argv)
{
    CustomClient c;

    c.Connect("127.0.0.1", 42069);

    bool key[3] = { false, false, false };
    bool old_key[3] = { false, false, false };

    bool bQuit = false;
    while (!bQuit)
    {
        if (GetForegroundWindow() == GetConsoleWindow())
        {
            key[0] = GetAsyncKeyState('1') & 0x8000;
            key[1] = GetAsyncKeyState('2') & 0x8000;
            key[2] = GetAsyncKeyState('3') & 0x8000;
        }

        if (key[0] && !old_key[0]) c.PingServer();
        if (key[1] && !old_key[1]) c.MessageAll();
        if (key[2] && !old_key[2]) bQuit = true;

        for (int i = 0; i < 3; i++) old_key[i] = key[i];

        if (c.IsConnected())
        {
            if (!c.Incoming().empty())
            {
                auto msg = c.Incoming().pop_front().msg;

                switch (msg.header.id)
                {
                case CustomMsgTypes::ServerAccept:
                {
                    // Respuesta en el cliente al ser aceptada la conexión.				
                    std::cout << "Server Accepted Connection\n";
                }
                break;

                case CustomMsgTypes::ServerPing:
                {
                    std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
                    std::chrono::system_clock::time_point timeThen;
                    msg >> timeThen;
                    std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
                }
                break;

                case CustomMsgTypes::ServerMessage:
                {
                    // Servidor manda un mensaje a todos los clientes conectados desde un clientes.	
                    uint32_t clientID;
                    msg >> clientID;
                    std::cout << "Hello from [" << clientID << "]\n";
                }
                break;
                }
            }
        }
        else
        {
            std::cout << "Server Down\n";
            bQuit = true;
        }
    }

    std::cout << "\033[36m"; // Establecer el color azul
    std::cout << R"(
    ===============================================================================================================
    __/\\\\\\\\\\\\\__________/\\\\\\\\\\\\\\\__________/\\\\\\\\\__________________________/\\\\\\\\\_____________
    ___\/\\\/////////\\\_______\///////\\\/////________/\\\////////________________________/\\\///////\\\__________
    ____\/\\\_______\/\\\_____________\/\\\___________/\\\/________________________________\///______\//\\\________
    _____\/\\\\\\\\\\\\\\______________\/\\\__________/\\\_________________/\\\____/\\\_______________/\\\/________
    ______\/\\\/////////\\\_____________\/\\\_________\/\\\________________\//\\\__/\\\_____________/\\\//_________
    _______\/\\\_______\/\\\_____________\/\\\_________\//\\\________________\//\\\/\\\___________/\\\//___________
    ________\/\\\_______\/\\\_____________\/\\\__________\///\\\_______________\//\\\\\__________/\\\/_____________
    _________\/\\\\\\\\\\\\\/______________\/\\\____________\////\\\\\\\\\_______\//\\\__________/\\\\\\\\\\\\\\\__
    __________\/////////////________________\///________________\/////////_________\///__________\///////////////__
    ===============================================================================================================
    )" << std::endl;
    std::cout << "\033[0m"; // Restablecer el color

    bbt::logger::init(argc, argv);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_SetLogPriority(SDL_LOG_CATEGORY_CUSTOM, SDL_LogPriority::SDL_LOG_PRIORITY_VERBOSE);

    //LOG_MACRO(bbt::logLevel::trace, "Usuario no registrado ID:{} SEXY:{}%", 1, 100);

    const char* glsl_version = "#version 430";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // Enable native IME.
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;

    SDL_Window* window;

    window = SDL_CreateWindow("BBT",600,400, window_flags);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    SDL_GL_MakeCurrent(window, glContext);

    SDL_ShowWindow(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);



    bool runing = true;

    while (runing)
    {

        SDL_Event event;

        while (SDL_PollEvent(&event))
        {

            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT)
            {
                runing = false;
            }



        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        bool pOpen;
        ImGui::ShowDemoWindow(&pOpen);

        ImGui::Begin("Mordi");
        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(glContext);

    return 0;
}