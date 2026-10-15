#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cctype>

struct Position {
    int r, c;
    bool operator==(const Position& other) const {
        return r == other.r && c == other.c;
    }
};

class DungeonGame {
private:
    const int rows = 10;
    const int cols = 20;
    Position player{1, 1};
    std::vector<Position> coins;
    std::vector<Position> guards;
    std::vector<std::string> baseGrid;
    int score = 0;
    int moves = 0;
    bool gameOver = false;
    bool won = false;
    std::mt19937 rng{std::random_device{}()};

    void clearScreen() const {
        // ANSI escape code to clear screen and move cursor to top-left
        std::cout << "\033[2J\033[1;1H";
    }

    Position getRandomEmptyTile() {
        std::uniform_int_distribution<int> rDist(1, rows - 2);
        std::uniform_int_distribution<int> cDist(1, cols - 2);
        while (true) {
            Position p{rDist(rng), cDist(rng)};
            if (baseGrid[p.r][p.c] == '.' && !(p == player)) {
                return p;
            }
        }
    }

public:
    DungeonGame() {
        baseGrid.resize(rows, std::string(cols, '.'));
        for (int r = 0; r < rows; ++r) {
            baseGrid[r][0] = '#';
            baseGrid[r][cols - 1] = '#';
        }
        for (int c = 0; c < cols; ++c) {
            baseGrid[0][c] = '#';
            baseGrid[rows - 1][c] = '#';
        }

        // Add interior walls
        for (int i = 0; i < 15; ++i) {
            Position wall = getRandomEmptyTile();
            baseGrid[wall.r][wall.c] = '#';
        }

        // Spawn 5 coins and 3 guards
        for (int i = 0; i < 5; ++i) coins.push_back(getRandomEmptyTile());
        for (int i = 0; i < 3; ++i) guards.push_back(getRandomEmptyTile());
    }

    void render() const {
        clearScreen();
        std::vector<std::string> display = baseGrid;

        for (const auto& c : coins) display[c.r][c.c] = '$';
        for (const auto& g : guards) display[g.r][g.c] = 'G';
        display[player.r][player.c] = '@';

        std::cout << "=== DUNGEON TREASURE HUNT ===\n";
        std::cout << "Coins left: " << coins.size() 
                  << " | Score: " << score 
                  << " | Moves: " << moves << "\n\n";

        for (const auto& row : display) {
            std::cout << "  " << row << "\n";
        }
        std::cout << "\nLegend: [@] You  [$] Coin  [G] Guard  [#] Wall\n";
        std::cout << "Controls: W (Up), A (Left), S (Down), D (Right), Q (Quit)\n";
    }

    void moveGuards() {
        std::uniform_int_distribution<int> dirDist(0, 3);
        const int dr[] = {-1, 1, 0, 0};
        const int dc[] = {0, 0, -1, 1};

        for (auto& g : guards) {
            int dir = dirDist(rng);
            int nr = g.r + dr[dir];
            int nc = g.c + dc[dir];
            if (baseGrid[nr][nc] != '#') {
                g.r = nr;
                g.c = nc;
            }
        }
    }

    void update(char input) {
        input = std::tolower(input);
        if (input == 'q') {
            gameOver = true;
            return;
        }

        int nr = player.r;
        int nc = player.c;

        if (input == 'w') nr--;
        else if (input == 's') nr++;
        else if (input == 'a') nc--;
        else if (input == 'd') nc++;
        else return; // Ignore invalid keys

        // Move player if not hitting a wall
        if (baseGrid[nr][nc] != '#') {
            player.r = nr;
            player.c = nc;
            moves++;
        }

        // Check if player collected a coin
        auto it = std::remove(coins.begin(), coins.end(), player);
        if (it != coins.end()) {
            coins.erase(it, coins.end());
            score += 100;
            if (coins.empty()) {
                won = true;
                gameOver = true;
                return;
            }
        }

        // Check collision before and after guard movement
        for (const auto& g : guards) {
            if (g == player) { gameOver = true; return; }
        }

        moveGuards();

        for (const auto& g : guards) {
            if (g == player) { gameOver = true; return; }
        }
    }

    void run() {
        while (!gameOver) {
            render();
            std::cout << "Move > ";
            char input;
            if (!(std::cin >> input)) break;
            update(input);
        }

        render();
        if (won) {
            std::cout << "\nVictory! You collected all treasures in " << moves << " moves!\n";
        } else {
            std::cout << "\nGame Over! Final Score: " << score << "\n";
        }
    }
};

int main() {
    DungeonGame game;
    game.run();
    return 0;
}
