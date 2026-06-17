cmake_minimum_required(VERSION 3.10)
project("Grafico")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw-3.4)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/glm-master)
file(GLOB FONTES_MEU_OPEN "${CMAKE_CURRENT_SOURCE_DIR}/lib/meuOpen/*.cpp")

add_executable(meuAPP main.cpp ${FONTES_MEU_OPEN})

target_include_directories(meuAPP PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(meuAPP PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/meuOpen")

target_compile_definitions(meuAPP PRIVATE GLEW_STATIC)

target_include_directories(meuAPP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew-2.3.1/include")

target_link_libraries(meuAPP PRIVATE 
    glfw 
    glm
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew-2.3.1/lib/Release/x64/glew32s.lib"
    opengl32
)

# Garante que a pasta static exista no build antes de copiar
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/static")

# Comando para copiar o Vertex Shader
add_custom_command(TARGET meuAPP POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/static/vMapShader.glsl"
    "${CMAKE_BINARY_DIR}/static/vMapShader.glsl"
    COMMENT "Copiando Vertex Shader para a pasta de build..."
)

# Comando para copiar o Fragment Shader
add_custom_command(TARGET meuAPP POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/static/fMapShader.glsl"
    "${CMAKE_BINARY_DIR}/static/fMapShader.glsl"
    COMMENT "Copiando Fragment Shader para a pasta de build..."
)



