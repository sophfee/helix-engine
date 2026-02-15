#include <chrono>
#include <iostream>
#include <thread>

#include "graphics.hpp"
#include "gltf.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main(
    [[maybe_unused]] int argc,
    [[maybe_unused]] char* argv[]
) {
    initGraphics();

    auto s = simdjson::padded_string::load(R"(S:\C++ Projects\helix-engine\helix-engine\build-output\Debug\x64\silver.gltf)");
    auto gltf_test_data = gltf::parse(std::move(s.value()));//R"({"asset":{"copyright":"","generator":"Source 2 Viewer 18.0.0.0 - https://valveresourceformat.github.io","version":"2.0"},"accessors":[{"bufferView":0,"componentType":5126,"count":13579,"max":[29.01854133605957,18.968040466308594,98.9421615600586],"min":[-19.928876876831055,-18.96805191040039,-8.716897964477539],"type":"VEC3"},{"bufferView":1,"componentType":5126,"count":13579,"max":[1,1,1],"min":[-1,-1,-1],"type":"VEC3"},{"bufferView":2,"componentType":5126,"count":13579,"max":[1,1],"min":[-0.3985775411128998,-0.1718275398015976],"type":"VEC2"},{"bufferView":3,"componentType":5125,"count":70788,"type":"SCALAR"}],"bufferViews":[{"buffer":0,"byteLength":162948,"byteOffset":283152,"target":34962},{"buffer":0,"byteLength":162948,"byteOffset":446100,"target":34962},{"buffer":0,"byteLength":108632,"byteOffset":609048,"target":34962},{"buffer":0,"byteLength":283152,"target":34963}],"buffers":[{"byteLength":717680,"uri":"werewolf_physics.bin"}],"images":[{"name":"physics_default","uri":"physics_default.png"}],"materials":[{"name":"physics_group_material","pbrMetallicRoughness":{"baseColorFactor":[0.5,0.5,1,1],"baseColorTexture":{"index":0},"metallicFactor":0}}],"meshes":[{"name":"physics_group","primitives":[{"attributes":{"POSITION":0,"NORMAL":1,"TEXCOORD_0":2},"indices":3,"material":0}]}],"nodes":[{"extras":{"SurfaceProperty":"default","InteractAs":[]},"name":"physics_group","matrix":[3.027916E-09,0,0.025399996,0,0.025399996,3.027916E-09,0,0,0,0.025399996,3.027916E-09,0,0,0,0,1],"mesh":0}],"samplers":[{"magFilter":9729,"minFilter":9987}],"scenes":[{"name":"werewolf.vmdl_c","nodes":[0]}],"textures":[{"name":"physics_default","sampler":0,"source":0}]})"_padded);

    for (auto &buffers : gltf_test_data.buffers) {
        std::cout << buffers.data().size() << '\n';
    }

    {
        window_config config{
            false,
            true,
            false,
            true,
            std::nullopt
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

        glViewport(0, 0, 1920, 1080);

        CProgram programObject;
        CShader vertexStage(gl::ShaderType::VertexShader), fragmentStage(gl::ShaderType::FragmentShader);

        vertexStage.setSource(R"(#version 330 core
layout (location = 0) in vec3 attrib_position; // w is ignored, better spacing for cache lines.
uniform mat4 mvp;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 pos;

void main() {
    gl_Position =  proj * view * model * vec4(attrib_position, 1.0);
    pos = vec4(attrib_position.xyz, 1.0);
})");

        
        fragmentStage.setSource(R"(#version 330 core
out vec4 FragColor;
uniform vec4 color;
in vec4 pos;
void main() {
    FragColor = abs(pos); // vec4(gl_FragPos.xyz, 1.0);
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

        programObject.use();

        glm::mat4
            model = glm::lookAt(glm::vec3(0.0f, -10.0f, -5.0f), glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
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

        vertexArray.enableAttribute(0);

        for (size_t n = 0; n < 0xFFFFFFFFuL; n++) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            f32 const time = static_cast<f32>(glfwGetTime());

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            view  = glm::lookAt(glm::vec3(100.0f, 0.0f, 60.0f), glm::vec3(0.0f, 0.0f, 60.0f), glm::vec3(0.0f, 0.0f, 1.0f));//glm::vec3((glm::cos(time * .80f) * 10.0f), 20.0f * glm::tan(glm::cos(time * 8.0) * glm::sin(time * 8.0)), (glm::sin(time * 8.0f) * 10.0f)), glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            proj  = glm::perspective(70.0f, 16.0f / 9.0f, 0.1f, 300.0f);
            
            programObject.setUniform(uModel, model);
            programObject.setUniform(uView, view);
            programObject.setUniform(uProj, proj);
            
            glfwPollEvents();
            
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

            mainWindow.swapBuffers();
        }
    }


    terminateGraphics();
    
    return 0;
}
