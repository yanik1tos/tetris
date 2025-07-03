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
const int height = 20, width = 15, shape_size = 4, screen_width = 230, screen_height = 64;
const int start_x = width / 2 - (shape_size / 2), start_y = 0;
const int delete_ticks = 4;
int x = start_x, y = start_y, count_fall = 0;
bool game, fall, godown, goright, goleft, cangoright, cangoleft;
time_t randseed = time(0);
vector<vector<int>> field(height, vector<int>(width, 0));
const vector<vector<vector<vector<int>>>> shapes = {
{
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
	 {0, 1, 0, 0}},

	{{0, 0, 0, 0},
	 {1, 1, 1, 1},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},
},
{
	{{0, 1, 1, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 0, 0},
	 {0, 1, 1, 0},
	 {0, 0, 1, 0},
	 {0, 0, 0, 0}},

	{{0, 0, 0, 0},
	 {0, 1, 1, 0},
	 {1, 1, 0, 0},
	 {0, 0, 0, 0}},

	{{1, 0, 0, 0},
	 {1, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}}
},
{
	{{0, 1, 1, 0},
	 {0, 1, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 1, 0},
	 {0, 1, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 1, 0},
	 {0, 1, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 1, 0},
	 {0, 1, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}}
},
{
	{{0, 1, 0, 0},
	 {1, 1, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 0, 0},
	 {0, 1, 1, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 0, 0, 0},
	 {1, 1, 1, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 0, 0},
	 {1, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}}
},
{
	{{1, 1, 0, 0},
	 {0, 1, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 0, 1, 0},
	 {0, 1, 1, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 0, 0, 0},
	 {1, 1, 0, 0},
	 {0, 1, 1, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 0, 0},
	 {1, 1, 0, 0},
	 {1, 0, 0, 0},
	 {0, 0, 0, 0}}
},
{
	{{0, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 1, 1, 0},
	 {0, 0, 0, 0}},

	{{0, 0, 0, 0},
	 {1, 1, 1, 0},
	 {1, 0, 0, 0},
	 {0, 0, 0, 0}},

	{{1, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 1, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 0, 1, 0},
	 {1, 1, 1, 0},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}}
},
{
	{{0, 0, 1, 0},
	 {0, 0, 1, 0},
	 {0, 1, 1, 0},
	 {0, 0, 0, 0}},

	{{0, 1, 0, 0},
	 {0, 1, 1, 1},
	 {0, 0, 0, 0},
	 {0, 0, 0, 0}},

	{{0, 0, 1, 1},
	 {0, 0, 1, 0},
	 {0, 0, 1, 0},
	 {0, 0, 0, 0}},

	{{0, 0, 0, 0},
	 {0, 1, 1, 1},
	 {0, 0, 0, 1},
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
			field[y + i][x + j] &= shapes[shape_index][shape_rotation][i][j];
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


	for (int j = 0; j < width; ++j) {
		field[height - 1][j] = 1;
	}

	for (int i = 0; i < height; ++i) {
		field[i][0] = 1;
		field[i].back() = 1;
	}
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

			int ny = y + i + dy;
			int nx = x + j + dx;

			if (nx <= 0 || nx >= width  - 1 || 
				ny <= 0 || ny >= height - 1 || 
				field[ny][nx] == 1) {

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
			
			field[y + i][x + j] = 0;
		}
	}


	// rotate
	bool can = true;
	for (int i = 0; i < shape_size; ++i) {
		for (int j = 0; j < shape_size; ++j) {
			if (shapes[shape_index][new_shape_rotation][i][j] == 1 && field[y + i][x + j] == 1) {
				can = false;
			}
		}
	}
	if (can) {
		shape_rotation = new_shape_rotation;
	}
	new_shape_rotation = shape_rotation;


	// fall
	++count_fall;
	if (count_fall == 5 && fall) {
		count_fall = 0;

		++y;

		if (checkCollision(0, 1)) {
			fall = false;
		}
	}


	// down
	if (godown) {
		while (!checkCollision(0, 1)) {
			++y;
		}

		fall = false;
	}

	// left
	if (goleft) {
		--x;
		goleft = false;

		if (checkCollision(-1, 0)) {
			cangoleft = false;
		}
    }

	// right
    if (goright) {
		++x;
		goright = false;

		if (checkCollision(1, 0)) {
			cangoright = false;
		}
	}


	// delete rows
	for (int j = 1; j < width - 1; ++j) {
		for (int i = 0; i < height - 1; ++i) {
			if (field[i][j] == delete_ticks) {
				for (int k = i - 1; k > -1; k--) {
					field[k + 1][j] = field[k][j];
				}
			}
		}
	}


	for (int j = 1; j < width - 1; ++j) {
		for (int i = 0; i < height - 1; ++i) {
			if (field[i][j] >= 2) {
				++field[i][j];
			}
		}
	}
	

	for (int i = 0; i < height - 1; ++i) {
		int yes = 0;
		for (int j = 1; j < width - 1; ++j) {
			if (field[i][j] == 1) {
				++yes;
			}
		}

		if (yes == width - 2) {
			for (int j = 1; j < width - 1; ++j) {
				field[i][j] = 2;
			}
		}
		yes = 0;
	}


	// put the shape
	for (int i = 0; i < shape_size; ++i) {
		for (int j = 0; j < shape_size; ++j) {
			if (shapes[shape_index][shape_rotation][i][j] == 0) {
				continue;
			}

			field[y + i][x + j] = 1;
		}
	}
}


void draw() {
    char clear = system("clear");

	// center vertically
	for (int _ = 0; _ < (screen_height - height) / 2; ++_) {
		cout << "\n";
	}

	string spaces((screen_width - width) / 2, ' ');

    for (int i = 0; i < height; ++i) {
		cout << spaces;
		for (int j = 0; j < width; ++j) {
			if (j == 0 || j == width - 1 || i == height - 1) {
				cout << "##";
			} else if (field[i][j] == 0) {
				cout << "  ";
			} else if (field[i][j] == 1) {
				cout << "[]";
			} else if (field[i][j] == 2 || field[i][j] == 3) {
				cout << "**";
			} else if (field[i][j] == 4) {
				cout << "//";
			} else {
				cout << "00";
			}
		}
		cout << "\n";
    }


	// for (int i = 0; i < height; ++i) {
	// 	for (int j = 0; j < width; ++j) {
	// 		cout << field[i][j];
	// 	}
	// 	cout << "\n";
    // }
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
