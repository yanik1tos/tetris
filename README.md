COMPILE
Only works for linux because of the libraries required. All libraries are linux built-in.
g++ main.cpp && ./a.out


SCREEN_SIZE
In order to center the game properly use calc_screen_size.py to calculate your screen size and paste it in main.cpp 14 line as screen_width and screen_height.


KEYS
Use 'a', 'd' to move a shape.
Use 's' to push the shape fully down.
Use 'q', 'e' to rotate the shape in CCW and CW directions, respectively.
Use 'x' to finish the game.


AFTER
Do "stty sane" after using to get ur terminal settings back to default.
