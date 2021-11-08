#include <iostream>
#include <queue>
#include <set>
#include <math.h>
#include <chrono>

using namespace std::chrono;
using namespace std;

#define PUZZLE_SIZE 9

static constexpr int solution[PUZZLE_SIZE] = {1,2,3,4,5,6,7,8,0};

enum QueueingFunction {Uniform, Misplaced, Manhattan};
enum Direction {Up, Down, Left, Right};

struct Node {
    Node(int d, int c, int s[PUZZLE_SIZE]) {
        for (int i = 0; i < PUZZLE_SIZE; i++) {
            state[i] = s[i];
        }
        depth = d;
        cost = c;
    }

    friend bool operator< (const Node& n1, const Node& n2) {
        return n1.cost > n2.cost;
    }

    int depth; // g(n)
    int cost; // this is f(n) = g(n) + h(n)
    int state[PUZZLE_SIZE];
};

struct Pos {
    Pos(int problem[PUZZLE_SIZE]) {
        for (int i = 0; i < PUZZLE_SIZE; i++) {
            if (!problem[i]) {
                x = i % 3;
                y = i / 3;
                index = i;
                break;
            }
        }
    }
    int x, y, index;
};  

/* TargetFound()
        Returns whether the solution has been found or not.
*/
bool TargetFound(int problem[PUZZLE_SIZE]) {
    for (int i = 0; i < PUZZLE_SIZE; i++) {
        if (problem[i] != solution[i]) {
            return false;
        }
    }
    return true;
}

void DisplayPuzzle(int problem[PUZZLE_SIZE]) {
    cout << problem[0] << " " << problem[1] << " " << problem[2] << endl;
    cout << problem[3] << " " << problem[4] << " " << problem[5] << endl;
    cout << problem[6] << " " << problem[7] << " " << problem[8] << endl << endl;
}

string ToString(int problem[PUZZLE_SIZE]) {
    string res;
    for (int i = 0; i < PUZZLE_SIZE; i++) {
        res += (problem[i] + '0');
    }
    return res;
}

int MisplacedTile(int problem[PUZZLE_SIZE]) {
    int total = 0;
    for (int i = 0; i < PUZZLE_SIZE; i++) {
        if (problem[i] != solution[i]) {
            total++;
        }
    }
    return total;
}

int ManhattanDist(int problem[PUZZLE_SIZE]) {
    int dist = 0;
    int x1, y1, x2, y2;
    for (int i = 0; i < PUZZLE_SIZE; i++) {
        x1 = i % 3;
        y1 = i / 3;
        for (int j = 0; j < PUZZLE_SIZE; j++) {
            if (problem[i] == solution[j]) {
                x2 = j % 3;
                y2 = j / 3;
                dist += abs(x1 - x2) + abs(y1-y2);
            }
        }
    }
    return dist;
}

Node Move(Node currNode, const QueueingFunction &function, const Direction &dir) {
    int newState[PUZZLE_SIZE];
    Pos p(currNode.state);

    for (int i = 0; i < PUZZLE_SIZE; i++) {
        newState[i] = currNode.state[i];
    }

    // We take sqrt(PUZZLE_SIZE) to easily allow for solving 15 puzzle, 24 puzzle, 35 puzzle, etc.
    int movement;
    switch (dir) {
        case Up:
            movement = -(sqrt(PUZZLE_SIZE));
            break;
        case Down:
            movement = sqrt(PUZZLE_SIZE);
            break;
        case Left:
            movement = -1;
            break;
        case Right:
            movement = 1;
            break;
    }

    // Swap zero and number
    int temp = newState[p.index + movement];
    newState[p.index + movement] = 0;
    newState[p.index] = temp;

    int heuristic = 0;

    switch (function) {
        // Do nothing
        case Uniform: 
            break;
        case Misplaced:
            heuristic = MisplacedTile(newState);
            break;
        case Manhattan:
            heuristic = ManhattanDist(newState);
            break;
    }

    int depth = currNode.depth + 1;
    int cost = depth + heuristic;

    return Node(depth, cost, newState);
}

void GeneralSearch(int problem[PUZZLE_SIZE], const QueueingFunction &function) {
    priority_queue<Node> q;
    set<string> visited;
    
    q.push(Node(0, 0, problem));

    while (!q.empty()) {
        Node temp = q.top();
        q.pop();
        visited.insert(ToString(temp.state));

        if (TargetFound(temp.state)) {
            cout << "Solution found at depth: " << temp.depth << endl;
            return;
        } else {
            Pos p(temp.state);

            if (p.y > 0) {
                Node t = Move(temp, function, Up);
                if (visited.find(ToString(t.state)) == visited.end()) {
                    q.push(t);
                }
            }
            if (p.y < 2) {
                Node t = Move(temp, function, Down);
                if (visited.find(ToString(t.state)) == visited.end()) {
                    q.push(t);
                }
            }
            if (p.x > 0) {
                Node t = Move(temp, function, Left);
                if (visited.find(ToString(t.state)) == visited.end()) {
                    q.push(t);
                }
            }
            if (p.x < 2) {
                Node t = Move(temp, function, Right);
                if (visited.find(ToString(t.state)) == visited.end()) {
                    q.push(t);
                }
            }
        }
    }
}

int main() {
    int trivial[PUZZLE_SIZE] = {1,2,3,4,5,6,7,8,0};
    int problem2[PUZZLE_SIZE] = {1,2,3,4,5,6,0,7,8};
    int problem4[PUZZLE_SIZE] = {1,2,3,5,0,6,4,7,8};
    int problem8[PUZZLE_SIZE] = {1,3,6,5,0,2,4,7,8};
    int problem12[PUZZLE_SIZE] = {1,3,6,5,0,7,4,8,2};
    int problem16[PUZZLE_SIZE] = {1,6,7,5,0,3,4,8,2};
    int problem20[PUZZLE_SIZE] = {7,1,2,4,8,5,6,3,0};
    int problem24[PUZZLE_SIZE] = {0,7,2,4,6,1,3,5,8};

    auto start = high_resolution_clock::now();
    GeneralSearch(problem24, Manhattan);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);

    cout << duration.count() << endl;

    start = high_resolution_clock::now();
    GeneralSearch(problem24, Misplaced);
    stop = high_resolution_clock::now();

    duration = duration_cast<microseconds>(stop - start);
    cout << duration.count() << endl;

    start = high_resolution_clock::now();
    GeneralSearch(problem24, Uniform);
    stop = high_resolution_clock::now();

    duration = duration_cast<microseconds>(stop - start);
    cout << duration.count() << endl;
    return 0;
}
