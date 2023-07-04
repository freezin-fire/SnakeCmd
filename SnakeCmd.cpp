#include <iostream>
#include <list>
#include <chrono>

#include <Windows.h>

int screenW = 120;
int screenH = 30;

// Struct to hold Snake body parts
struct SnakeSegment {
	int x;
	int y;
};

int main() {
	// Create Screen Buffer
	wchar_t* screen = new wchar_t[screenW * screenH];
	for (int i = 0; i < screenW * screenH; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	// Game loop
	while (true)
	{
		// Initialize Snake
		std::list<SnakeSegment> mahesh = { {60, 20}, {61, 20}, {62, 20}, {63, 20}, {64, 20}, {65, 20}, {66, 20}, {67, 20}, {68, 20}, {69, 20} };
		int foodX = 30;
		int foodY = 15;
		int score = 0;
		int snakeDirection = 3;
		bool dead = false;
		bool keyLeft = false, keyRight = false, keyLeftOld = false, keyRightOld = false;
		
		while (!dead) {

			// ==================== Timing and Input ====================
			auto t1 = std::chrono::system_clock::now();
			while ((std::chrono::system_clock::now() - t1) < ((snakeDirection % 2 == 1) ? std::chrono::milliseconds(140) : std::chrono::milliseconds(180))) {
				// Input setup
				keyRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
				keyLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;

				if (keyRight && !keyRightOld)
				{
					snakeDirection++;
					if (snakeDirection == 4) snakeDirection = 0;
				}
				if (keyLeft && !keyLeftOld)
				{
					snakeDirection--;
					if (snakeDirection == -1) snakeDirection = 3;
				}

				keyRightOld = keyRight;
				keyLeftOld = keyLeft;
			}


			// ==================== Game Logic ====================
			// Snake position Update
			switch (snakeDirection)
			{
			case 0: // UP
				mahesh.push_front({ mahesh.front().x, mahesh.front().y - 1});
				break;
			case 1: // RIGHT
				mahesh.push_front({ mahesh.front().x + 1, mahesh.front().y });
				break;
			case 2: // DOWN
				mahesh.push_front({ mahesh.front().x, mahesh.front().y + 1});
				break;
			case 3: // LEFT
				mahesh.push_front({ mahesh.front().x - 1, mahesh.front().y });
				break;
			}
			mahesh.pop_back(); // erasing the tail

			// ------------ Collision detection
			// Collision: Snake and World
			if (mahesh.front().x < 2 || mahesh.front().x >= screenW)
				dead = true;
			if (mahesh.front().y < 3 || mahesh.front().y >= screenH)
				dead = true;
			// Collision: Snake and Food
			if (mahesh.front().x == foodX && mahesh.front().y == foodY) {
				score++;
				while (screen[foodY * screenW + foodX] != L' ')
				{
					foodX = std::rand() % screenW;
					foodY = (std::rand() % (screenH - 3) + 3);
				}
				for (int i = 0; i < 5; i++)
				{
					mahesh.push_back({ mahesh.back().x, mahesh.back().y });
				}
			}
			// Collision: Snake and Body
			for (std::list<SnakeSegment>::iterator i = mahesh.begin(); i != mahesh.end(); i++)
				if (i != mahesh.begin() && i->x == mahesh.front().x && i->y == mahesh.front().y)
					dead = true;


			// ==================== Add to Frame Buffer ====================
			// Clear screen
			for (int i = 0; i < screenW*screenH; i++)
			{
				screen[i] = L' ';
			}

			// Draw Stats
			for (int i = 0; i <= screenW; i++)
			{
				screen[i] = L'+';
				screen[2 * screenW + i] = L'+';
			}
			wsprintf(&screen[screenW + 5], L"   Mahesh : The Snake!!!        Score = %d ", score);

			// Draw Snake Body
			for (auto s : mahesh)
				screen[s.y * screenW + s.x] = dead ? L'*' : L'o';

			// Draw Snake Head
			screen[mahesh.front().y * screenW + mahesh.front().x] = dead ? L'X' : L'@';
			
			// Draw Food
			screen[foodY * screenW + foodX] = L'#';

			// Play again prompt
			if (dead)
				wsprintf(&screen[15 * screenW + 40], L"<<< PRESS 'SPACE' TO PLAY AGAIN >>>");

			// ==================== Render Buffer ====================
			WriteConsoleOutputCharacter(hConsole, screen, screenW * screenH, { 0, 0 }, &dwBytesWritten);
		}

		// Wait for pressing space key
		while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
	}

	return 0;
}