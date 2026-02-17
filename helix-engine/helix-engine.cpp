#define STB_IMAGE_IMPLEMENTATION

#include <chrono>
#include <iostream>
#include <thread>
#include <bit>

#include "graphics.hpp"
#include "gltf.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Windows.h>

#include "mesh.hpp"
#include "os.hpp"
#include "png.hpp"
#include "util.hpp"

#include "stb/stb_image.h"
int main(
    [[maybe_unused]] int argc,
    [[maybe_unused]] char* argv[]
) {
    initGraphics();

    u32 number = 0x12345678;
    number = byteswap(number);
    printf("%X\n", number);

    {
        auto path = os::getCurrentDirectory();
        //std::wcout << path << TEXT('\n');

        constexpr char fuck[5] = "IHDR";
        u32 v = charsToType<u32>(fuck);
        u32 m = 0;
        m += fuck[0];
        m += fuck[1] << 8;
        m += fuck[2] << 16;
        m += fuck[3] << 24;
        printf("%u %u %u\n", v, *(u32*)fuck, m);
        
        std::string path_to_test_resource = wstringToString(path);// + ;
        path_to_test_resource.back() = '\\';
        path_to_test_resource += "test-resources\\silver.gltf";
        //std::cout << path_to_test_resource << '\n';
        auto s = simdjson::padded_string::load(path_to_test_resource).value();
        /*
        path_to_test_resource = wstringToString(path);// + ;
        path_to_test_resource.back() = '\\';
        path_to_test_resource += "test-resources\\silver-textures\\default_mask_tga_344101f8.png";
        png::result<std::vector<u8>> result = png::decode(path_to_test_resource);
        std::cout << (result.has_value ? "win" : "losse") << '\n';
        std::cout << result.failed_at << ' ' << result.unwrap().size() << '\n';
        */
        auto gltf_test_data = gltf::parse(path_to_test_resource,std::move(s));
        window_config config{
            .transparent    = false,
            .resizable      =  true,
            .fullscreen     = false,
            .decorated      =  true,
            .videoMode      = std::nullopt
        };
        
        // raii
        CWindow mainWindow(
           glm::ivec2(1920, 1080),
           "hello",
           std::nullopt,
           std::nullopt
        );
        glfwShowWindow(mainWindow.window);
        mainWindow.makeContextCurrent();

        glDebugMessageCallback(open_gl_debug_proc, nullptr);
        glViewport(0, 0, 1920, 1080);
        
        CMesh mesh(gltf_test_data);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        CProgram programObject;
        CShader vertexStage(gl::ShaderType::VertexShader), fragmentStage(gl::ShaderType::FragmentShader);

        vertexStage.setSource(R"(#version 330 core
layout (location = 0) in vec3 aPos; // w is ignored, better spacing for cache lines.
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aUv0;
uniform mat4 mvp;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 pos;
out vec3 nor;
out vec2 uv;
out vec3 camPos;

void main() {
    gl_Position =  proj * view * model * vec4(aPos, 1.0);
    pos = view * model * vec4(aPos.xyz, 1.0);
    nor = normalize(transpose(inverse(mat3(view*model))) * aNor);
    uv = aUv0;
    camPos = view[3].xyz;
})");

        
        fragmentStage.setSource(R"(#version 330 core
out vec4 FragColor;
uniform vec4 color;
in vec4 pos;
in vec3 nor;
in vec2 uv;
in vec3 camPos;

void main() {
    FragColor = vec4(
        vec3(
            (
                dot(normalize(pos.xyz-camPos), nor)
                //dot(vec3(-1.0, 0.0, 0.0), nor)
            )
        ),
        1.0
    ); // vec4(gl_FragPos.xyz, 1.0);

    //FragColor = vec4(abs(nor), 1.0);
})");
        

        vertexStage.compile();
        vertexStage.compileStatus();

        std::string const infoLogVert = vertexStage.infoLog();
        std::cout << infoLogVert << "\n\n\n";
        
        fragmentStage.compile();
        fragmentStage.compileStatus();
        
        std::string const infoLogFrag = fragmentStage.infoLog();
        std::cout << infoLogFrag << '\n';

        programObject.attach(vertexStage);
        programObject.attach(fragmentStage);
        programObject.link();
        /*
        CVertexArray vertexArray;
        CBuffer positionBuffer, normalBuffer, texcoordsBuffer, indexBuffer;

        glm::vec4 const vertices[] = {
            glm::vec4(-1.0f, -1.0f, -1.0f, +1.0f),
            glm::vec4(+1.0f, -1.0f, -1.0f, +1.0f),
            glm::vec4(+1.0f, +1.0f, -1.0f, +1.0f),
            glm::vec4(-1.0f, +1.0f, -1.0f, +1.0f),
        };

        u8 const indices[] = {
            0, 1, 2,
            2, 3, 0,
            1, 2, 0
        };

        
        positionBuffer.setData(
            sizeof(glm::vec3) * gltf_test_data.accessors[4].count(), &gltf_test_data.buffers[0][358224], gl::BufferUsageARB::DynamicDraw);
        indexBuffer.setData(
            sizeof(u16) * gltf_test_data.accessors[7].count(), &gltf_test_data.buffers[0][2111056], gl::BufferUsageARB::StaticDraw);

        vertexArray.bind();
        vertexArray.setVertexBuffer(0, positionBuffer, sizeof(glm::vec3), 0);
        vertexArray.setElementBuffer(indexBuffer);

        */
        programObject.use();

        glm::mat4
            model = glm::lookAt(glm::vec3(0.0f, -20.0f, -5.0f), glm::vec3(0.0f, 20.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
            view = glm::lookAt(glm::vec3(1.0f,5.0f, -0.0f), glm::vec3(1.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
            proj = glm::perspective(glm::radians(130.0f), 16.0f / 9.0f, 0.1f, 100.0f);

        i32 const
            uMvp = programObject.uniformLocation("mvp"),
            uModel = programObject.uniformLocation("model"),
            uView = programObject.uniformLocation("view"),
            uProj = programObject.uniformLocation("proj");
        programObject.setUniform(uMvp, model * view * proj);
        programObject.setUniform(uModel, model);
        programObject.setUniform(uView, view);
        programObject.setUniform(uProj, proj);

        i32 const uColor = programObject.uniformLocation("color");
        programObject.setUniform(uColor, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        //vertexArray.enableAttribute(0);

        for (size_t n = 0; n < 0xFFFFFFFFuL; n++) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            f32 const time = static_cast<f32>(glfwGetTime());

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            view  = glm::lookAt(glm::vec3(glm::cos(time * 8.0f) * 200.0f, glm::sin(time * 8.0f) * 200.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));//glm::vec3((glm::cos(time * .80f) * 10.0f), 20.0f * glm::tan(glm::cos(time * 8.0) * glm::sin(time * 8.0)), (glm::sin(time * 8.0f) * 10.0f)), glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            proj  = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 300.0f);
            
            programObject.setUniform(uModel, model);
            programObject.setUniform(uView, view);
            programObject.setUniform(uProj, proj);
            
            glfwPollEvents();
            /*
            vertexArray.drawArrays(
                gl::PrimitiveType::Triangles,
                0,
                3
            );

            vertexArray.drawElements(
                gl::PrimitiveType::Triangles,
                gl::DrawElementsType::UnsignedShort,
                266757
            );
            */

            mesh.drawAllSubMeshes();

            mainWindow.swapBuffers();
        }
    }


    terminateGraphics();

    for (auto &thread : gltf_worker_threads_) {
        thread.join(); // FIXME
    }
    
    return 0;
}
