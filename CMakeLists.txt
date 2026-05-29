cmake_minimum_required(VERSION 3.23)#so see cmake min version idk why
project(nebula C)#just name it anything i never saW IT anywhere
find_package(Vulkan REQUIRED)#vulkan required cause its in file na 
find_package(glfw3 REQUIRED)#its also in file 
add_executable(nebula)#executable name
target_sources(nebula
  PRIVATE
  instance.c
  device.c
  present.c
  buffer.c
  cmdbuffer.c
  renderpass.c
  pipeline.c
  main.c
    )#like the source files here my all .c vulkan will come here now wooh  
target_link_libraries(nebula
Vulkan::Vulkan
glfw
)#linking lib