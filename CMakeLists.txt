cmake_minimum_required(VERSION 3.22)
project(Renderer)

set(CMAKE_CXX_STANDARD 14)

include_directories(Renderer)

add_executable(Renderer
        Renderer/blob.cpp
        Renderer/blob.h
        Renderer/bvh.cpp
        Renderer/bvh.h
        Renderer/d3d11_renderer_impl.h
        Renderer/debug_renderer.cpp
        Renderer/debug_renderer.h
        Renderer/dev_app.cpp
        Renderer/dev_app.h
        Renderer/frustum_culling.cpp
        Renderer/frustum_culling.h
        Renderer/main.cpp
        Renderer/math_types.h
        Renderer/pools.h
        Renderer/renderer.cpp
        Renderer/renderer.h
        Renderer/view.h
        Renderer.sln
        Renderer.sln.DotSettings.user)
