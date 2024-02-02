# GGConsoleGraphics

Sometimes you don't want or need the complexity of an entire graphics pipeline. Just drawing text and getting shapes to move around the terminal is enough.

## Installation
 
Made to work with Visual Studio, currently have no plans to add a build system. The solution and Main file are mearly a small demo of the usage. GGConsoleGraphics is a single source and header file.<br>
<br>
Add GGConsoleGraphics.h and GGConsoleGraphics.cpp as part of your project and you're good to go!

## Usage

1. Create a class that **extends** GG::ConsoleApp
2. Implement **Init**, **Update(float)**, **Draw(float)**
3. Create an instance of that class and call it's **Run** method
