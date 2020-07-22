#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>

using namespace std;

vector<vector<bool> > graph, used; //graph - матриця суміжності графа, used - використані ребра графа
queue<vector<int> > q; //q - черга для bfs
vector<int> route; //route - маршрут
int finish = -1, n; //finish - індекс фінішної точки, n - кількість індексованих точок

int bfs() {
    //перевірка на зв'язність графа
    if (q.empty()) return -1;

    route = q.front();
    q.pop();
    int i, last = route.back(); //i - індекс для циклу, last - остання точка маршруту

    //пошук ще не використаних суміжних точок
    for (i = 0; i < n; i++) if (graph[last][i] && !used[last][i]) {
            route.push_back(i);
            if (i == finish) return route.size();
            q.push(route);
            route.pop_back();
            used[i][last] = 1;
            used[last][i] = 1;
    }
    return bfs();
}

int main()
{
    //перевірка наявності вхідного файла
    ifstream fin ("input.osm");
    if (!fin.is_open()) {
        cout << "File \"input.osm\" is not finded.\n";
        system("pause");
        return 0;
    }

    string line, lastNode, startNode, finishNode; //line - стрічка файлу яка перетворюється на id точки, lastNode - попередня точка, startNode - стартова точка, finishNode - фінішна точка
    int start = -1, tmp = 0, i, j, lastNodeIndex, nowNodeIndex, result, allNodes = 0; //start - індекс стартової точки, tmp - для обмеження частоти сповіщень, i - індекс для циклу, j - додатковий індекс для циклу, lastNodeIndex - індекс попередньої точки, nowNodeIndex - індекс теперішньої точки, result - кількість точок отриманого маршрута, allNodes - кільсість усіх точок, включно із збігами
    vector<string> nodesIndexes; //nodesIndexes - індекс точок
    bool f; //f - прапорець
    vector<bool> g0; //g0 - масив для отримання матриць

    //отримання кінцевих точок
    cout << "Start node: ";
    cin >> startNode;
    cout <<  "Finish node: ";
    cin >> finishNode;

    //індексація точок
    cout << "Indexing nodes.\n";
    while(!fin.eof()) {
        f = 1;
        if (allNodes % 5000 == 0 && allNodes != tmp) {
            tmp = allNodes;
            cout << "Indexing nodes: found " << allNodes << " nodes, " << tmp << " nodes in index.\n";
        }
        getline(fin, line);
        if (line.find("<nd ref=") < 10000) {
            allNodes++;
            line.erase(0, line.find('=') + 2);
            line.erase(line.find_first_of("'\""), line.find('>') - line.find_first_of("'\"") + 1);
            for (i = 0; i < nodesIndexes.size(); i++) if (line == nodesIndexes[i]) {
                    f = 0;
                    break;
            }
            if (f) {
                nodesIndexes.push_back(line);
                if (line == startNode) start = nodesIndexes.size() - 1;
                if (line == finishNode) finish = nodesIndexes.size() - 1;
            }
        }
    }
    fin.close();
    cout << "Indexing nodes finished: found " << allNodes << " nodes, " << nodesIndexes.size() << " nodes in index.\n";

    //перевірка на наявність кінцевих точок у вхідному файлі
    if (start < 0 || finish < 0) {
        if (start < 0 && finish < 0) cout << "Start and finish nodes are not found at \"input.osm\".";
        else {
            if (start < 0) cout << "Start node is not found at \"input.osm\".";
            else cout << "Finish node is not found at \"input.osm\".";
        }
        return 0;
    }

    n = nodesIndexes.size();
    route.push_back(start);
    q.push(route);
    tmp = 0;

    //виділення пам'яті для матриці суміжності та матриці використаних ребер
    cout << "Getting memory for graph.\n";
    for (i = 0; i < n; i++) {
        if (i % 10000 == 0 && i != tmp) {
            tmp = i;
            cout << "Getting memory for graph: " << tmp << '/' << n <<".\n";
        }
        g0.clear();
        for (j = 0; j < n; j++) g0.push_back(0);
        graph.push_back(g0);
        used.push_back(g0);
    }
    cout << "Getting memory for graph finished: " << n << '/' << n <<".\n";

    j = 0;
    tmp = 0;
    f = 0;

    //заповнення матриці суміжності
    cout << "Extracting graph.\n";
    fin.open("input.osm");
    while(!fin.eof()) {
        if (j % 5000 == 0 && j != tmp) {
            tmp = j;
            cout << "Extracting graph: " << tmp << '/' << allNodes << " nodes processed.\n";
        }
        getline(fin, line);
        if (line.find("<nd ref=\"") < 10000) {
            j++;
            line.erase(0, line.find('=') + 2);
            line.erase(line.find_first_of("'\""), line.find('>') - line.find_first_of("'\"") + 1);
            if (f) {
                for (i = 0; i < n; i++) if (nodesIndexes[i] == line) {
                    nowNodeIndex = i;
                    break;
                }
                graph[nowNodeIndex][lastNodeIndex] = 1;
                graph[lastNodeIndex][nowNodeIndex] = 1;
                lastNodeIndex = nowNodeIndex;
            }
            else {
                lastNode = line;
                for (i = 0; i < n; i++) if (nodesIndexes[i] == lastNode) {
                    lastNodeIndex = i;
                    break;
                }
                f = 1;
            }
        }
        else f = 0;
    }
    cout << "Extracting graph finished: " << j << '/' << allNodes << " nodes processed.\n";
    fin.close();

    //виконуємо bfs та знаходимо кількість точок маршрута
    cout << "BFS.\n";
    result = bfs();
    cout << "BFS finished.\n";

    //якщо маршруту немає
    if (result < 0) {
        cout << "Route doesn't exist.\n";
        system("pause");
        return 0;
    }

    //експорт маршруту
    cout << "Exporting route.\n";
    ofstream fout ("out.txt");
    for (i = 0; i < route.size(); i++) fout << nodesIndexes[route[i]] << ' ';
    fout.close();
    cout << "Route exported successfully.\n";

    system("pause");
    return 0;
}
