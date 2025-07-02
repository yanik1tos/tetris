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

int shape_index;
const int height = 20, width = 20;
bool game, fall, goright, goleft, cangoright, cangoleft;
time_t randseed = time(0);
vector<pair<int, int>> new_shape;
vector<vector<char>> field(height, vector<char>(width, ' '));
const vector<vector<pair<int, int>>> shapes = {
    {{1, width / 2}, {1, width / 2 - 1}, {0, width / 2}, {0, width / 2 - 1}},
    {{1, width / 2}, {1, width / 2 + 1}, {0, width / 2 - 1}, {0, width / 2}},
    {{3, width / 2}, {2, width / 2}, {1, width / 2}, {0, width / 2}},
    {{1, width / 2}, {1, width / 2 - 1}, {0, width / 2 + 1}, {0, width / 2}}};



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


void setup() {
    game = true;
    fall = false;
    goleft = false;
    cangoright = true;
    cangoleft = true;
    srand(randseed);
    shape_index = -1;

    for (int i = 0; i < width; i++) {
		field.back()[i] = '#';
    }
    for (int i = 0; i < height; i++) {
		field[i][0] = '#';
		field[i].back() = '#';
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
				case 'x':
					game = false;
					break;
			}
		}
	}
}

void logic() {
    if (fall) {
		for (int i = 0; i < new_shape.size(); i++) {
			field[new_shape[i].first][new_shape[i].second] = ' ';
			new_shape[i].first++;
			field[new_shape[i].first][new_shape[i].second] = '.';

			if (field[new_shape[i].first + 1][new_shape[i].second] != ' ' && 
				find(new_shape.begin(), new_shape.end(), make_pair(new_shape[i].first + 1, new_shape[i].second)) == new_shape.end()) {
				fall = false;
			}
		}
		} else {
			shape_index = rand() % shapes.size();
			new_shape = shapes[shape_index];
			for (auto p : new_shape) {
				field[p.first][p.second] = '.';
			}
			fall = true;
			cangoleft = true;
			cangoright = true;
	}

	if (goleft) {
		for (int i = 0; i < new_shape.size(); i++) {
			field[new_shape[i].first][new_shape[i].second] = ' ';
			new_shape[i].second--;
			field[new_shape[i].first][new_shape[i].second] = '.';

			if (field[new_shape[i].first][new_shape[i].second - 1] != ' ' &&
				find(new_shape.begin(), new_shape.end(), make_pair(new_shape[i].first, new_shape[i].second - 1)) == new_shape.end()) {
				cangoleft = false;
			}
		}
		goleft = false;
    }

    if (goright) {
		for (int i = 0; i < new_shape.size(); i++) {
			field[new_shape[i].first][new_shape[i].second] = ' ';
			new_shape[i].second++;
			field[new_shape[i].first][new_shape[i].second] = '.';

			if (field[new_shape[i].first][new_shape[i].second + 1] != ' ' &&
				find(new_shape.begin(), new_shape.end(), make_pair(new_shape[i].first, new_shape[i].second + 1)) == new_shape.end()) {
				cangoright = false;
			}
		}
		goright = false;
    }
}

void draw() {
    char clear = system("clear");

    for (auto i : field) {
	for (auto j : i) {
	    cout << j;
	}
	cout << "\n";
    }
}

int main() {
	disableBuffering();
    setup();

    while (game) {
		input();
		logic();
		draw();

		cout << fall << " " << cangoleft << " " << cangoright << "\n";
		this_thread::sleep_for(chrono::milliseconds(500));
    }

	enableBuffering();
    return 0;
}
