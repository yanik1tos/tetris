#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

using namespace std;

int shape_index, shape_rotation = 0, new_shape_rotation = 0;
const int height = 20, width = 20, shape_size = 4;
const int start_x = width / 2 - (shape_size / 2), start_y = -1;
int x = start_x, y = start_y, count_fall = 0;
bool game, fall, godown, goright, goleft, cangoright, cangoleft;
time_t randseed = time(0);
vector<vector<int>> field(width, vector<int>(height, 0));
const vector<vector<vector<vector<int>>>> shapes = {{
	{{0, 0, 0, 0},
	 {1, 1, 1, 1},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 1, 0, 0}},

	{{0, 0, 0, 0},
	 {1, 1, 1, 1},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 1, 0, 0}}
},
{
	{{1, 0, 0, 0},
	 {1, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 0, 0, 0},
	 {0, 1, 1, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 0, 0},
	 {0, 1, 1, 0},
	 {0, 0, 1, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 1, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}}
},
{
	{{0, 0, 0, 0},
	 {1, 1, 0, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 0, 0, 0},
	 {1, 1, 0, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 0, 0, 0},
	 {1, 1, 0, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 0, 0, 0},
	 {1, 1, 0, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0}}
},
{
	{{0, 1, 0, 0},
	 {1, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 0, 0, 0},
	 {1, 1, 1, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 0, 0},
	 {0, 1, 1, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 0, 0},
	 {1, 1, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}}
},
{
	{{0, 0, 0, 0},
	 {0, 1, 0, 0},
	 {1, 1, 0, 0},
	 {1, 0, 0, 0}},

	{{0, 0, 0, 0},
	 {0, 0, 0, 0},
	 {1, 1, 0, 0},
	 {0, 1, 1, 0}},

	{{0, 0, 0, 0},
	 {0, 0, 1, 0},
	 {0, 1, 1, 0},
	 {0, 1, 0, 0}},

	{{0, 0, 0, 0},
	 {1, 1, 0, 0},
	 {0, 1, 1, 0},
	 {0, 0, 0, 0}}
}};



void enableBuffering() {
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void disableBuffering() {
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

bool kbhit() {
	timeval tv = {0, 0};
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);

	return select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0;
}


void shape_init() {
	shape_index    = rand() % shapes.size();
	shape_rotation = 0;
	new_shape_rotation = 0;
	x = start_x;
	y = start_y;

	for (int i = 0; i < shape_size; ++i) {
		for (int j = 0; j < shape_size; ++j) {
			field[x + i][y + j] &= shapes[shape_index][shape_rotation][i][j];
		}
	}
	
	fall   = true;
	godown = false;
	cangoleft  = true;
	cangoright = true;
}


void setup() {
    game = true;
    fall    = false;
	godown  = false;
	goright = false;
    goleft  = false;
    srand(randseed);
	shape_init();
}

void input() {
    if (shape_index == -1) {
		return;
    }

	if (kbhit()) {
		char c = getchar();
		if (c) {
			bool brk = false;
			switch (c) {
				case 'a':
					if (cangoleft) {
						goleft = true;
					}
					goright = false;
					break;
				case 'd':
					if (cangoright) {
						goright = true;
					}
					goleft = false;
					break;
				case 's':
					godown = true;
					break;
				case 'q':
					new_shape_rotation = shape_rotation - 1;
					if (new_shape_rotation < 0) {
						new_shape_rotation = 4 + new_shape_rotation;
					}
					break;
				case 'e':
					new_shape_rotation = shape_rotation + 1;
					if (new_shape_rotation > 3) {
						new_shape_rotation = new_shape_rotation - 4;
					}
					break;
				case 'x':
					game = false;
					break;
			}
		}
	}
}


bool checkCollision(int dx, int dy) {
	for (int i = 0; i < shape_size; ++i) {
		for (int j = 0; j < shape_size; ++j) {
			if (shapes[shape_index][shape_rotation][i][j] == 0) {
				continue;
			}

			int nx = x + i + dx;
			int ny = y + j + dy;

			if (nx < 0 || nx >= width  - 1 || 
				ny < 0 || ny >= height - 1 || 
				field[nx][ny] == 1) {

				return true;
			}
		}
	}

	return false;
}



void logic() {
	if (!fall) {
		shape_init();
		return;
	}

	// clear the shape
	for (int i = 0; i < shape_size; ++i) {
		for (int j = 0; j < shape_size; ++j) {
			if (shapes[shape_index][shape_rotation][i][j] == 0) {
				continue;
			}
			
			field[x + i][y + j] = 0;
		}
	}

	shape_rotation = new_shape_rotation;

	++count_fall;
	if (count_fall == 5 && fall) {
		count_fall = 0;

		++y;

		if (checkCollision(0, 1)) {
			fall = false;
		}
	}

	if (godown) {
		while (!checkCollision(0, 1)) {
			++y;
		}

		fall = false;
	}

	if (goleft) {
		--x;
		goleft = false;

		if (checkCollision(-1, 0)) {
			cangoleft = false;
		}
    }

    if (goright) {
		++x;
		goright = false;

		if (checkCollision(1, 0)) {
			cangoright = false;
		}
	}


	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (field[i][j] == 2) {
				field[i][j] = 0;
			}
		}
	}

	for (int i = 0; i < height; ++i) {
		int yes = 0;
		for (int j = 0; j < width; ++j) {
			if (field[i][j] == 1) {
				++yes;
			}
		}

		if (yes == width) {
			for (int j = 0; j < width; ++j) {
				field[i][j] = 2;
			}
		}
		cout << yes << "\n";
		yes = 0;
	}


	// put the shape
	for (int i = 0; i < shape_size; ++i) {
		for (int j = 0; j < shape_size; ++j) {
			if (shapes[shape_index][shape_rotation][i][j] == 0) {
				continue;
			}

			field[x + i][y + j] = 1;
		}
	}
}


void draw() {
    char clear = system("clear");

    for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			if (j == 0 || j == height - 1 || i == width - 1) {
				cout << "##";
			} else if (field[j][i] == 0) {
				cout << "  ";
			} else if (field[j][i] == 1) {
				cout << "[]";
			} else if (field[j][i] == 2) {
				cout << "**";
			} else {
				cout << "//";
			}
		}
		cout << "\n";
    }
}

int main() {
	disableBuffering();
    setup();
	cout << "\033[32m"; // green

    while (game) {
		input();
		logic();
		draw();

		this_thread::sleep_for(chrono::milliseconds(100));
    }

	enableBuffering();
    return 0;
}
