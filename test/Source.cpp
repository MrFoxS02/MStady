#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <stack>
#include <map>
#include <string>
#include <format>

using namespace std;


// граф
class GraphTransition
{
public:
	int Cost;

	int X;
	int Y;

	GraphTransition(int x, int y, int cost)
	{
		X = x;
		Y = y;
		Cost = cost;
	}
};

class GraphNode
{
public:
	int X;
	int Y;

	std::vector<GraphTransition> Transitions = {};

	GraphNode(int x, int y, std::vector<GraphTransition> transitions)
	{
		X = x;
		Y = y;

		for (const auto& transition : transitions)
		{
			Transitions.push_back(transition);
		}
	}
};



// робот
class Robot
{
public:
	std::string Id;

	int X;
	int Y;

	int GoalX = 0;
	int GoalY = 0;

	int PathLength = 0;
	std::map<std::pair<int, int>, bool> Path = {};

	Robot(std::string id, int x, int y)
	{
		Id = id;
		X = x;
		Y = y;
	}

	void SetGoalPoint(int goalX, int goalY)
	{
		GoalX = goalX;
		GoalY = goalY;
	}

	void SetPath(std::map<std::pair<int, int>, bool> path, int pathLength)
	{
		Path = path;
		PathLength = pathLength;
	}
};



const int ROWS(9), COLUMNS = 9;

int main()
{
	srand(time(NULL));

	std::vector<Robot> robots = {};
	std::vector<GraphNode> nodes = {};

	int map[10][10];


	// генерим карту
	for (int i = 0; i <= ROWS; i++)
	{
		for (int j = 0; j <= COLUMNS; j++)
		{
			if (rand() % 10 < 7)
			{
				map[i][j] = 1;
			}
			else
			{
				map[i][j] = 0;
			}
		}
	}


	// заполняем граф
	for (int i = 0; i <= ROWS; i++)
	{
		for (int j = 0; j <= COLUMNS; j++)
		{
			std::vector<GraphTransition> transition = { };

			//влево
			if (i != 0 && map[i - 1][j] != 0) {
				transition.push_back(GraphTransition(i - 1, j, 1));
			}

			// вверх
			if (j != 0 && map[i][j - 1] != 0) {
				transition.push_back(GraphTransition(i, j - 1, 1));
			}

			// вправо
			if (i != ROWS && map[i + 1][j] != 0) {
				transition.push_back(GraphTransition(i + 1, j, 1));
			}

			// вниз
			if (j != COLUMNS && map[i][j + 1] != 0) {
				transition.push_back(GraphTransition(i, j + 1, 1));
			}

			if (map[i][j] != 0) nodes.push_back(GraphNode(i, j, transition));
		}
	}


	// генерим робота и цель
	for (int i = 0; i < 3; i++)
	{
		bool robotDone = false;

		while (!robotDone) {
			// стартовая позиция робота
			int x = rand() % ROWS;
			int y = rand() % COLUMNS;

			if (map[x][y] == 0) continue;

			bool canBotBePlaced = true;
			for (const auto& bot : robots)
			{
				if (bot.X == x || bot.Y == y)
				{
					canBotBePlaced = false;
					break;
				}
			}
			if (!canBotBePlaced) continue;

			std::string name = i == 0 ? "X" : i == 1 ? "Y" : "Z";
			Robot newbot = Robot(name, x, y);

			bool goalDone = false;

			// финиш
			while (!goalDone) {
				int goalX = rand() % ROWS;
				int goalY = rand() % COLUMNS;

				if (map[goalX][goalY] == 0 || newbot.X == goalX || newbot.Y == goalY) continue;

				bool canEndBePlaced = true;
				for (const auto& bot : robots)
				{
					if (bot.GoalX == goalX || bot.GoalY == goalY || bot.X == goalX || bot.Y == goalY)
					{
						canEndBePlaced = false;
						break;
					}
				}
				if (!canEndBePlaced) continue;

				newbot.SetGoalPoint(goalX, goalY);
				robots.push_back(newbot);

				robotDone = true;
				goalDone = true;
			}
		}
	}

	cout << "ROBOTS:" << endl;
	for (const auto& bot : robots)
	{
		cout << bot.Id << ": " << endl;
		cout << "    pos: " << bot.X << " " << bot.Y << endl;
		cout << "    end: " << bot.GoalX << " " << bot.GoalY << endl;
	}


	// вывод карты
	cout << "\nMAP:" << endl;
	for (int i = 0; i <= ROWS; i++) {
		for (int j = 0; j <= COLUMNS; j++)
		{
			bool isBotInCell = false;

			for (const auto& bot : robots)
			{
				if (i == bot.X && bot.Y == j)
				{
					cout << bot.Id << "  ";
					isBotInCell = true;
					break;
				}
				else if (i == bot.GoalX && bot.GoalY == j)
				{
					cout << bot.Id + "E" << " ";
					isBotInCell = true;
					break;
				}
			}

			if (!isBotInCell) cout << map[i][j] << "  ";
		}

		std::cout << endl;
	}


	// вывод графа
	/*cout << "\nGRAPH:" << endl;
	for (int i = 0; i < std::size(nodes); i++) {
		cout << "node: " << nodes[i].X << " " << nodes[i].Y << std::endl;
		cout << "transitions: " << std::endl;

		for (int j = 0; j < std::size(nodes[i].Transitions); j++)
		{
			cout << "       node: " << nodes[i].Transitions[j].X << " " << nodes[i].Transitions[j].Y << " | cost: " << nodes[i].Transitions[j].Cost << std::endl;
		}

		cout << "" << std::endl;
	}*/


	// перебор графа
	std::cout << "\nDIJKSTRA" << std::endl; 
	for (auto& bot : robots)
	{
		GraphNode* startNode{ nullptr };

		std::queue<GraphNode*> queue; // очередь обработки вершин

		std::map<std::pair<int, int>, int> costs = {}; // стоимости
		std::map<std::pair<int, int>, std::pair<int, int>> visited = {}; // в какую вершину из какой пришли


		// поиск стартовой вершины
		for (int i = 0; i < std::size(nodes); i++)
		{
			if (nodes[i].X == bot.X && nodes[i].Y == bot.Y) {
				GraphNode* node{ &nodes[i] };
				startNode = node;
				break;
			}
		}

		queue.push(startNode);

		costs[{bot.X, bot.Y}] = 0;

		GraphNode* currentNode = startNode;

		while (queue.size() > 0) {
			GraphNode* targetNode = queue.front();
			queue.pop();

			// если достигли цели - выход
			if (targetNode->X == bot.GoalX && targetNode->Y == bot.GoalY) {
				break;
			}

			// добавляем все переходы в очередь
			for (const auto& transition : targetNode->Transitions) {
				GraphNode* transitionNode { nullptr };

				// находим вершину перехода
				for (int i = 0; i < std::size(nodes); i++)
				{
					if (nodes[i].X == transition.X && nodes[i].Y == transition.Y) {
						transitionNode = &nodes[i];
						break;
					}
				}

				if (transitionNode == nullptr) continue;

				// если стоимости нет в map или если стоимость больше чем новая
				int newCost = costs[{targetNode->X, targetNode->Y}] + transition.Cost;
				if (costs.find({ transitionNode->X, transitionNode->Y }) == costs.end() || costs[{ transitionNode->X, transitionNode->Y }] > newCost) {
					// добавить новую стоимость в словарь
					costs[{ transitionNode->X, transitionNode->Y }] = newCost;

					// добавляем переход в эту точку
					visited[{transitionNode->X, transitionNode->Y}] = { targetNode->X, targetNode->Y };

					// добавляем в очередь проверки новую вершину графа
					queue.push(transitionNode);
				}
			}

			// переместились в новую вершину
			currentNode = targetNode;
		}

		std::map<std::pair<int, int>, bool> path = {};
		int pathLength = 1;

		std::pair<int, int> node = { bot.GoalX, bot.GoalY };

		if (visited.find(node) == visited.end())
		{
			std::cout << bot.Id + " | target is unreachable" << std::endl;
			continue;
		}

		path[{ bot.GoalX, bot.GoalY }] = true;
		std::cout << bot.Id + " | path: " << std::to_string(bot.GoalX) << " " << std::to_string(bot.GoalY);
		while (node.first != bot.X || node.second != bot.Y) {
			node = visited[{ node.first, node.second }];
			path[node] = true;
			pathLength++;

			std::cout << " ---- " << std::to_string(node.first) << " " << std::to_string(node.second);
		}
		std::cout << " | length: " << pathLength << std::endl;

		bot.SetPath(path, pathLength);
	}



	// вывод карты
	std::cout << "\nGRAPH:" << endl;
	for (int i = 0; i <= ROWS; i++) {
		for (int j = 0; j <= COLUMNS; j++)
		{
			if (map[i][j] != 0) 
			{
				std::cout << "o   ";
			}
			else
			{
				std::cout << "    ";
			}
		}

		std::cout << endl;
	}


	// metric
	std::cout << "\nMETRICS" << std::endl;
	std::cout << "MAKESPAN: " << std::max({ robots[0].PathLength, robots[1].PathLength, robots[2].PathLength }) << std::endl;
	std::cout << "FLOWTIME: " << std::to_string(robots[0].PathLength + robots[1].PathLength + robots[2].PathLength) << std::endl;
}

