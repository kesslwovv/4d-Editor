//
//  main.cpp
//  4d projection
//
//  Created by David on 19.09.22.
//

#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "json.hpp"

//#include "stb_image.h"

#include <string>
#include <sstream>
#include <cerrno>
#include <math.h>

#include "shaderProgram.hpp"
#include "camera.hpp"
#include "renderer.hpp"
#include "mesh.hpp"
#include "editController.hpp"
#include "window.hpp"

#include "FileBrowser/ImGuiFileBrowser.h"

#include "timer.hpp"

#define START_WINDOW_WIDTH 800
#define START_WINDOW_HEIGHT 800

std::filesystem::path executable;

//int window_w = START_WINDOW_WIDTH;
//int window_h = START_WINDOW_HEIGHT;

//int framebuffer_width =  1024 * 8;
//int framebuffer_height = 1024 * 8;

Mesh* orientationCube[4] = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};

bool showGui = true;

Mesh* mainMesh = nullptr;
EditController editor;

//GLFWwindow* window = nullptr;
Camera cam;

RenderController renderer;
timer mainTimer("main timer");

bool showVertecies=true, showEdges=true, showFaces=false, showCells=true;

//float testFloat;

Texture tex;

Window window;

FramebufferHandler mainFramebuffer;
FramebufferHandler translucentFramebuffer;

Program combinerProgram;

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           "** GL ERROR **",
            type, severity, message );
}

void error_callback( int error, const char *msg ) {
    std::string s;
    s = " [" + std::to_string(error) + "] " + msg + '\n';
    std::cerr << s << std::endl;
    exit(-1);
}

void resize_callback(GLFWwindow* win, int w, int h)
{
    cam.window_size_callback(win, w, h);
}

void cleanup()
{
//    glDeleteVertexArrays(1, &lineVAO);
//    glDeleteBuffers(1, &lineVBO);
//    lineProgram.destroy();
    
    timer::showAll();
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    window.close();
    glfwTerminate();
}


int main(int argc, const char* argv[])
{
//    timer::timerLookup = std::unordered_map<std::string, timer*>();
    std::cout << argv[0] << std::endl;
    executable = std::filesystem::path(argv[0]).parent_path();
    
    // Initialize GLFW
    glfwInit();

    // Tell GLFW what version of OpenGL we are using
    // In this case we are using OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Tell GLFW we want forward compatibility (required for MacOS)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // Tell GLFW we are using the CORE profile
    // So that means we only have the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = Window(START_WINDOW_WIDTH, START_WINDOW_HEIGHT, "4D projection");
    
    glfwSetWindowSizeCallback(window.getWindow(), resize_callback);
    
    window.makeContextCurrent();

    //Load GLAD so it configures OpenGL
    gladLoadGL();
    // Specify the viewport of OpenGL in the Window
    // In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
    glViewport(0, 0, START_WINDOW_WIDTH*2, START_WINDOW_HEIGHT*2);
    glfwSetErrorCallback(error_callback);
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigDockingWithShift = true;
//    io.ConfigFlags |= ImGuiConfigFlags_;
//    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    
    
    
    GLfloat lineWidthRange[2];
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
    
    std::cout << "Min Line width: " << lineWidthRange[0] << "\nMax Line width: " << lineWidthRange[1] << "\n";
    

    renderer = RenderController(
                                ShaderController(
                                    Program(PATH_SHADER "/point_vert.glsl",
                                            PATH_SHADER "/point_frag.glsl"
                                            ),
                                    RenderMode::vertecies,
                                    {
                                        {UniformType::color4, "color", "color", {1, 1, 1, 1}},
                                        {UniformType::lsize, "draw offset", "bleed_size", {.2, 0, 0, 0}},
                                        {UniformType::lsize, "color offset", "color_bleed", {.5, 0, 0, 0}},
                                        {UniformType::lsize, "point size", "sizeMul", {3, 0, 0, 0}},
                                        {UniformType::lsize, "max size", "maxSize", {30, 0, 0, 0}}
                                        
                                    },
                                    ShaderController::makeFlag(true, true, false)
                                ),
                                ShaderController(
                                   Program(PATH_SHADER "/line_vert.glsl",
                                           PATH_SHADER "/line_frag.glsl"),
                                   RenderMode::edges,
                                   {
                                       {UniformType::color4, "color", "color", {1, 1, 1, 1}},
                                       {UniformType::lsize, "draw offset", "bleed_size", {.05, 0, 0, 0}},
                                       {UniformType::ratio, "intensity", "intensity", {.4, 0, 0, 0}}
                                   },
                                   ShaderController::makeFlag(true, true, false)
                               ),
                               ShaderController(
                                    Program(PATH_SHADER "/translucent_vert.glsl",
                                            PATH_SHADER "/translucent_frag.glsl"),
                                    RenderMode::faces,
                                    {
                                        {UniformType::color4, "color", "color", {.8, .8, .8, .8}},
                                        {UniformType::size, "draw offset", "bleed_size", {1, 0, 0, 0}},
                                        {UniformType::ratio, "intensity", "intensity", {.6, 0, 0, 0}}
                                        
                                    },
                                    ShaderController::makeFlag(true, true, false)
                                ),
                                ShaderController(
                                    Program(PATH_SHADER "/slice_vert.glsl",
                                            PATH_SHADER "/slice_frag.glsl",
                                            PATH_SHADER "/slice_geom.glsl"),
                                    RenderMode::cells,
                                    {
                                        {UniformType::color4, "color", "color", {.8, .8, .8, 1}},
                                        {UniformType::dir4, "light direction", "lightDir", {0, 1, 0, 0}},
                                        {UniformType::boolean, "enable lighting", "useLight"}
                                        
                                    },
                                    ShaderController::makeFlag(false, true, true)
                                ));


    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glBlendEquation(GL_FUNC_ADD);
//    glEnable(GL_LINE_SMOOTH);
//    glEnable(GL_POLYGON_SMOOTH);
    
    glEnable(GL_DITHER);
    
    
    Mesh* mesh = nullptr;
    
    tex = Texture(GL_TEXTURE_3D);
    tex.readFileBinary(PATH_REC "/striped_tex8.texture");
    
    TextureUniformHandler mainTexHandler{0, "mainTex"};

    renderer._vertecies.addTextureHandler(mainTexHandler);
    renderer._edges.addTextureHandler(mainTexHandler);
    renderer._faces.addTextureHandler(mainTexHandler);
    renderer._slice.addTextureHandler(mainTexHandler);
    
//    int currentTime = (int)time(nullptr);
//    printf("random seed: %d\n", currentTime);
//    srand(1667329612);

    mesh = new Mesh(PATH_REC "/plane 2.obj4d");
//    mesh->initColorRandom();
//    mesh->initColorConstant({1, 1, 1, 1});
    mesh->_position = vec4(0, -3, 0, 0);
    mesh->_tex = &tex;
    renderer.addMesh(mesh);
    
    mesh = new Mesh(PATH_REC "/hc.obj4d");
    renderer.addMesh(mesh);
    mesh->_tex = &tex;
    mainMesh = mesh;
    
    for (int i = 0; i < 4; i++) {
        orientationCube[i] = new Mesh(PATH_REC "/orientation_block.obj4d",
                        FileFormat::ascii,
                        ColorInitData::File(PATH_REC "/orientation_block.obj4d.colordata.bin"));
//        orientationCube[i]->_useTex = false;
        orientationCube[i]->_transform = glm::mat4(0.002);
        orientationCube[i]->add_flags(MeshFlags::disableVert);
        renderer.addMesh(orientationCube[i]);
    }

    renderer.setPtr(cam.getViewPtr(), cam.getPosPtr(), cam.getProjPtr());
        
    editor = EditController(&cam, mainMesh, &renderer);
    
    
    mainFramebuffer = FramebufferHandler(START_WINDOW_WIDTH, START_WINDOW_HEIGHT);
    translucentFramebuffer = FramebufferHandler(START_WINDOW_WIDTH, START_WINDOW_HEIGHT, mainFramebuffer.getDepthTexID());
        
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    combinerProgram = Program(PATH_SHADER "/fulscreen_quad_vert.glsl", PATH_SHADER "/tex_combiner_frag.glsl");
    
    
    mainTimer.start();

    // Main while loop
    while (!window.shouldClose())
    {
        window.makeContextCurrent();

//        glfwGetFramebufferSize(window, &window_w, &window_h);
//        glDepthMask(true);
//        glClearColor(0.2, 0.05, 0.3, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        int res_sampler = 2; // 2x2 antialiasing
        mainFramebuffer.resize(window.getFbWidth() * res_sampler, window.getFbHeight() * res_sampler);
        translucentFramebuffer.resize(window.getFbWidth() * res_sampler, window.getFbHeight() * res_sampler);
        
        if (!io.WantCaptureKeyboard) {
            if (window.getKey(GLFW_KEY_ESCAPE)) {
                cleanup();
                return 0;
            }
            
            cam.handleInput(window.getWindow());
            
            showVertecies = !window.getKey(GLFW_KEY_U);
            
            showEdges = !window.getKey(GLFW_KEY_O);
            
            showFaces = !window.getKey(GLFW_KEY_P);
            
            showCells = !window.getKey(GLFW_KEY_I);
            
            if (window.getKey(GLFW_KEY_V)) {
                timer::resetAll();
            }
            
//            if (glfwGetKey(window, GLFW_KEY_Z)) {
//                dragVertexDepth += dragSpeed;
//            }
//            if (glfwGetKey(window, GLFW_KEY_X)) {
//                dragVertexDepth -= dragSpeed;
//            }
//            if (glfwGetKey(window, GLFW_KEY_C)) {
//                dragVertexPerp += dragSpeed;
//            }
//            if (glfwGetKey(window, GLFW_KEY_V)) {
//                dragVertexPerp -= dragSpeed;
//            }
            
//            keyControl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
            
            
            editor.handleInput(window.getWindow());
            
            

        }
        
        {
            for (int q = 0; q < 4; q++) {
                float xOff = 0.035 * cam.getAspectRatio() * (q & 1 ? -1 : 1);
                float yOff = 0.035 * (q & 2 ? -1 : 1);
                vec4 newpos = cam.getPos()
                            + cam.getFwd() * 0.1f
                            + cam.getRight() * xOff
                            + cam.getUp() * yOff;
                orientationCube[q]->_position = newpos;
            }
        }
        
//        glViewport(0, 0, framebuffer_width, framebuffer_height);
        
//        glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
        
//        glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);
        mainFramebuffer.bind();
        
        glDepthMask(true);
        glClearColor(0.2, 0.05, 0.3, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (showCells) {
            START_TIMER("drawing tetrahedra");
            renderer.setMode(RenderMode::cells);
            renderer.render();
            END_TIMER("drawing tetrahedra");
            checkError();
        }
        
        if (showVertecies) {
            renderer.setMode(RenderMode::vertecies);
            renderer.render();
            checkError();
        }
        
        if (showEdges) {
            renderer.setMode(RenderMode::edges);
            renderer.render();
            checkError();
        }
        
        translucentFramebuffer.bind();
        
        glDepthMask(true);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        if (showFaces) {
            renderer.setMode(RenderMode::faces);
            renderer.render();
            checkError();
        }
        
        glViewport(0, 0, window.getFbWidth(), window.getFbHeight());
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glDepthMask(true);
        glClearColor(0.2, 0.05, 0.3, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        combinerProgram.activate();
        
        GLuint
        colorA = combinerProgram.getUniformLocation("ColorA"),
        colorB = combinerProgram.getUniformLocation("ColorB"),
        depthA = combinerProgram.getUniformLocation("DepthA"),
        depthB = combinerProgram.getUniformLocation("DepthB");
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mainFramebuffer.getColorTexID());
        glUniform1i(colorA, 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, translucentFramebuffer.getColorTexID());
        glUniform1i(colorB, 1);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, mainFramebuffer.getDepthTexID());
        glUniform1i(depthA, 2);
        
        
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, translucentFramebuffer.getDepthTexID());
        glUniform1i(depthB, 3);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);

        
        cam.update();
        
//        renderer.renderHighlight();

        
        
        ImGui::ShowDemoWindow();
        

        double dt = mainTimer.update();
        
        timer::showAll();
        
        printf("miliseconds passed: %lf\n", dt * 1000);
        
        editor.handleGUI();
        renderer.handleGUI();
        cam.handleGUI();
        
        ImGui::Begin("time info");
        ImGui::Text("miliseconds passed: %lf\n", dt * 1000);
        ImGui::Text("FPS: %lf\n", 1 / dt);
        
        if (ImGui::BeginTable("table1", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit))
        {
            std::vector<timer*> timers = timer::orderedTimers();
            
            ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthStretch, 0.45);
            ImGui::TableSetupColumn("calls", ImGuiTableColumnFlags_WidthStretch, 0.15);
            ImGui::TableSetupColumn("total time (s)", ImGuiTableColumnFlags_WidthStretch, 0.2);
            ImGui::TableSetupColumn("average (ms)", ImGuiTableColumnFlags_WidthStretch, 0.2);
            ImGui::TableHeadersRow();
                        
            
            for (int i = 0; i < timers.size(); i++)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", timers[i]->getName());
                
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%llu", timers[i]->getCount());
                
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%f", (double)timers[i]->getTotal()/CLOCKS_PER_SEC);
                
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%f", (double)timers[i]->getTotal()/timers[i]->getCount()/CLOCKS_PER_SEC*1000);
                
            }
            ImGui::EndTable();
        }
        
        ImGui::End();
        
//        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        window.swapBuffers();
        
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        
        window.update();
    }

    cleanup();
    return 0;
}


