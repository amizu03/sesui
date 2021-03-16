# SESUI
Sesame's immediate mode GUI framework

# Information
1. This GUI framework creates a list of polygons, which can then be drawn on the screen with the help of a custom rendering backend.
2. This is a header-only library, meaning that you only need to include the packed header file when including it in your project.
3. An additional, implementation-specific header must be provided by the user (the rendering backend). An example for DX9 is provided in the source files.

# Requirements
1. MSVC VS 2019 C++ Build Tools or newer.
2. VS2019 or newer (if you want to compile the DX9 test environment).
3. Python 3 or newer.
4. Microsoft DirectX SDK (June 2010).
5. Project settings that support the c++17 language standard or newer.

# Usage (Implementing in Your Project)
1. Clone the repository or download the source files to your computer.
2. Run the Python script to pack all source and header files into one header.
3. Copy the header and source file into your project directory. Include the header file in your project (sesui.hpp) and implement the custom rendering backend. Read src/main.cpp to see an example window.

# Usage (DX9 Test Environment)
1. Clone the repository or download the source files to your computer.
2. Open the VS2019 solution.
3. Compile the project and run the application.
