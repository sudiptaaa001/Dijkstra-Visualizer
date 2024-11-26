#include <SFML/Graphics.hpp>
#include <queue>
#include <vector>
#include <map>
#include <cmath>

const int WIDTH = 600;
const int HEIGHT = 600;
const int ROWS = 30;
const int CELL_SIZE = WIDTH / ROWS;

sf::Color WHITE(255, 255, 255);
sf::Color BLACK(0, 0, 0);
sf::Color RED(255, 0, 0);
sf::Color GREEN(0, 255, 0);
sf::Color BLUE(0, 0, 255);
sf::Color YELLOW(255, 255, 0);

class Node {
public:
    int row, col;
    sf::RectangleShape cell;
    bool isObstacle = false;
    bool isStart = false;
    bool isEnd = false;
    float distance = INFINITY;
    Node* previous = nullptr;
    std::vector<Node*> neighbors;

    Node(int r, int c) : row(r), col(c) {
        cell.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        cell.setPosition(row * CELL_SIZE, col * CELL_SIZE);
        cell.setFillColor(WHITE);
    }

    void reset() {
        isObstacle = isStart = isEnd = false;
        distance = INFINITY;
        previous = nullptr;
        cell.setFillColor(WHITE);
    }

    void setStart() {
        isStart = true;
        distance = 0;
        cell.setFillColor(BLUE);
    }

    void setEnd() {
        isEnd = true;
        cell.setFillColor(YELLOW);
    }

    void setObstacle() {
        isObstacle = true;
        cell.setFillColor(BLACK);
    }

    void addNeighbors(std::vector<std::vector<Node>>& grid) {
        neighbors.clear();
        if (row < ROWS - 1 && !grid[row + 1][col].isObstacle) neighbors.push_back(&grid[row + 1][col]); // Down
        if (row > 0 && !grid[row - 1][col].isObstacle) neighbors.push_back(&grid[row - 1][col]); // Up
        if (col < ROWS - 1 && !grid[row][col + 1].isObstacle) neighbors.push_back(&grid[row][col + 1]); // Right
        if (col > 0 && !grid[row][col - 1].isObstacle) neighbors.push_back(&grid[row][col - 1]); // Left
    }
};

void drawGrid(sf::RenderWindow& window, std::vector<std::vector<Node>>& grid) {
    for (auto& row : grid) {
        for (auto& node : row) {
            window.draw(node.cell);
        }
    }
}

void reconstructPath(Node* endNode, sf::RenderWindow& window) {
    Node* current = endNode;
    while (current->previous != nullptr) {
        current->cell.setFillColor(BLUE);
        current = current->previous;
        window.display();
        sf::sleep(sf::milliseconds(50));
    }
}

void dijkstra(sf::RenderWindow& window, std::vector<std::vector<Node>>& grid, Node* start, Node* end) {
    std::priority_queue<std::pair<float, Node*>, std::vector<std::pair<float, Node*>>, std::greater<>> openSet;
    start->distance = 0;
    openSet.push({0, start});

    while (!openSet.empty()) {
        Node* current = openSet.top().second;
        openSet.pop();

        if (current == end) {
            reconstructPath(end, window);
            return;
        }

        for (Node* neighbor : current->neighbors) {
            float tempDist = current->distance + 1;

            if (tempDist < neighbor->distance) {
                neighbor->distance = tempDist;
                neighbor->previous = current;
                openSet.push({neighbor->distance, neighbor});
                neighbor->cell.setFillColor(GREEN);
            }
        }

        if (current != start && current != end) current->cell.setFillColor(RED);
        window.clear();
        drawGrid(window, grid);
        window.display();
        sf::sleep(sf::milliseconds(20));
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Dijkstra Path Finding Algorithm Visualizer");

    std::vector<std::vector<Node>> grid(ROWS, std::vector<Node>(ROWS, Node(0, 0)));
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < ROWS; ++col) {
            grid[row][col] = Node(row, col);
        }
    }

    Node* start = nullptr;
    Node* end = nullptr;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                int row = sf::Mouse::getPosition(window).x / CELL_SIZE;
                int col = sf::Mouse::getPosition(window).y / CELL_SIZE;

                if (row >= 0 && col >= 0 && row < ROWS && col < ROWS) {
                    Node* node = &grid[row][col];

                    if (!start && node != end) {
                        start = node;
                        start->setStart();
                    } else if (!end && node != start) {
                        end = node;
                        end->setEnd();
                    } else if (node != start && node != end) {
                        node->setObstacle();
                    }
                }
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                int row = sf::Mouse::getPosition(window).x / CELL_SIZE;
                int col = sf::Mouse::getPosition(window).y / CELL_SIZE;

                if (row >= 0 && col >= 0 && row < ROWS && col < ROWS) {
                    Node* node = &grid[row][col];
                    node->reset();
                    if (node == start) start = nullptr;
                    if (node == end) end = nullptr;
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space && start && end) {
                    for (int row = 0; row < ROWS; ++row) {
                        for (int col = 0; col < ROWS; ++col) {
                            grid[row][col].addNeighbors(grid);
                        }
                    }
                    dijkstra(window, grid, start, end);
                }

                if (event.key.code == sf::Keyboard::C) {
                    start = end = nullptr;
                    for (auto& row : grid) {
                        for (auto& node : row) {
                            node.reset();
                        }
                    }
                }
            }
        }

        window.clear();
        drawGrid(window, grid);
        window.display();
    }

    return 0;
}
