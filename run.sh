#!/bin/bash
# Compile the game
echo "Compiling..."
g++ -O3 -march=native -o shadow_temple Main.cpp camera.cpp player.cpp level.cpp model.cpp -framework OpenGL -framework GLUT -Wno-deprecated-declarations -Wall -I/opt/homebrew/include -L/opt/homebrew/lib -lassimp

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful! Starting game..."
    # Run the game
    ./shadow_temple
else
    echo "Compilation failed."
fi
