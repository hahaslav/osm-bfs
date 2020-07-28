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

    string line, lastNode, startNode, finishNode, toFind; //line - стрічка файлу яка перетворюється на id точки або на її координати, lastNode - попередня точка, startNode - стартова точка, finishNode - фінішна точка, toFind - стрічка із id точки для пошуку її координат
    int start = -1, tmp = 0, i, j, lastNodeIndex, nowNodeIndex, result, allNodes = 0; //start - індекс стартової точки, tmp - для обмеження частоти сповіщень, i - індекс для циклу, j - додатковий індекс для циклу, lastNodeIndex - індекс попередньої точки, nowNodeIndex - індекс теперішньої точки, result - кількість точок отриманого маршрута, allNodes - кількість усіх точок, включно із збігами, під час експорту - кількість точок у маршруті
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
            cout << "Indexing nodes: found " << allNodes << " nodes, " << nodesIndexes.size() << " nodes in index.\n";
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

    allNodes = route.size();

    //експорт маршруту для JOSM
    cout << "Exporting route for JOSM.\n";
    ofstream fout ("JOSM.txt");
    for (i = 0; i < allNodes; i++) fout << nodesIndexes[route[i]] << ' ';
    fout.close();
    cout << "Route for JOSM exported successfully.\n";

    tmp = 0;
    j = 0;

    //експорт маршруту для показу на карті
    cout << "Exporting route to show it on map.\n";
    fout.open("map.html");
    fout << "<!--This file was generated by BFS for OSM:-->\n";
    fout << "<!--https://github.com/Yarjk32/osm-bfs-->\n";
    fout << "<!DOCTYPE HTML>\n";
    fout << "<html>\n";
    fout << "\t<head>\n";
    fout << "\t\t<meta charset=\"utf-8\" />\n";
    fout << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no\">\n";
    fout << "\t\t<script src=\"http://cdn.leafletjs.com/leaflet/v0.7.7/leaflet.js\"></script>\n";
    fout << "\t\t<link rel=\"stylesheet\" href=\"http://cdn.leafletjs.com/leaflet/v0.7.7/leaflet.css\" />\n";
    fout << "\t\t<style>\n";
    fout << "\t\t\thtml, body {\n";
    fout << "\t\t\t\theight: 100%;\n";
    fout << "\t\t\t\tpadding: 0;\n";
    fout << "\t\t\t\tmargin: 0;\n\t\t\t}\n";
    fout << "\t\t\t#map {\n";
    fout << "\t\t\t\twidth: 100%;\n\t\t\t\theight: 100%;\n\t\t\t}\n";
    fout << "\t\t</style>\n";
    fout << "\t</head>\n";
    fout << "\t<body>\n";
    fout << "\t\t<div id=\"map\"></div>\n";
    fout << "\t\t<script>\n";
    fout << "\t\t\tvar map = L.map('map');\n";
    fout << "\t\t\tL.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {\n";
    fout << "\t\t\t\tmaxZoom: 19,\n";
    fout << "\t\t\t\tattribution: '&copy; <a href=\"https://openstreetmap.org/copyright\">OpenStreetMap contributors</a>'\n";
    fout << "\t\t\t}).addTo(map);\n";
    fout << "\t\t\tvar latlngs = [\n";
    for (i = 0; i < allNodes; i++) {
        if (j % 25 == 0 && j != tmp) {
            tmp = j;
            cout << "Exporting route to show it on map: " << tmp << '/' << allNodes + 2 << " nodes processed.\n";
        }
        j++;
        fin.open("input.osm");
        toFind = "<node id=\"" + nodesIndexes[route[i]];
        while(!fin.eof()) {
            getline(fin, line);
            if (line.find(toFind) < 10000) {
                line.erase(0, line.find("lat") + 5);
                line.replace(line.find('"'), 7, ", ");
                line.erase(line.find('"'), line.find('>') - line.find('"') + 1);
                fout << "\t\t\t\t[" << line << "],\n";
                break;
            }
        }
        fin.close();
    }
    fout << "\t\t\t];\n\t\t\tvar polyline = L.polyline(latlngs, {color: '#002FFF', weight: 10, opacity: 0.3}).addTo(map);\n";
    fout << "\t\t\tmap.fitBounds(polyline.getBounds());\n";
    fout << "\t\t\tvar startIcon = L.icon({\n";
    fout << "\t\t\t\ticonUrl: 'https://www.openstreetmap.org/assets/marker-green-04ef1c75c0153b30420deef0668abdcab321651ddccb00a5a0b053a0521ae102.png',\n";
    fout << "\t\t\t\ticonSize: [25, 41],\n";
    fout << "\t\t\t\ticonAnchor: [13, 41],\n";
    fout << "\t\t\t\tshadowUrl: 'https://www.openstreetmap.org/assets/images/marker-shadow-a2d94406ba198f61f68a71ed8f9f9c701122c0c33b775d990edceae4aece567f.png',\n";
    fout << "\t\t\t\tshadowSize: [41, 41],\n";
    fout << "\t\t\t\tshadowAnchor: [13, 41]\n\t\t\t});\n";
    fout << "\t\t\tL.marker([";
    if (j % 25 == 0 && j != tmp) {
        tmp = j;
        cout << "Exporting route to show it on map: " << tmp << '/' << allNodes + 2 << " nodes processed.\n";
    }
    j++;
    fin.open("input.osm");
    toFind = "<node id=\"" + nodesIndexes[route[0]];
    while(!fin.eof()) {
        getline(fin, line);
        if (line.find(toFind) < 10000) {
            line.erase(0, line.find("lat") + 5);
            line.replace(line.find('"'), 7, ", ");
            line.erase(line.find('"'), line.find('>') - line.find('"') + 1);
            fout << line << "], {icon: startIcon}).addTo(map);\n";
            break;
        }
    }
    fin.close();
    fout << "\t\t\tvar finishIcon = L.icon({\n";
    fout << "\t\t\t\ticonUrl: 'https://www.openstreetmap.org/assets/marker-red-b4fe6bcae1b02da7478f3db266afb8027ed59ffba2fc43c03b85f3a6cf0d3d85.png',\n";
    fout << "\t\t\t\ticonSize: [25, 41],\n";
    fout << "\t\t\t\ticonAnchor: [13, 41],\n";
    fout << "\t\t\t\tshadowUrl: 'https://www.openstreetmap.org/assets/images/marker-shadow-a2d94406ba198f61f68a71ed8f9f9c701122c0c33b775d990edceae4aece567f.png',\n";
    fout << "\t\t\t\tshadowSize: [41, 41],\n";
    fout << "\t\t\t\tshadowAnchor: [13, 41]\n\t\t\t});\n";
    fout << "\t\t\tL.marker([";
    if (j % 25 == 0 && j != tmp) {
        tmp = j;
        cout << "Exporting route to show it on map: " << tmp << '/' << allNodes + 2 << " nodes processed.\n";
    }
    j++;
    fin.open("input.osm");
    toFind = "<node id=\"" + nodesIndexes[route[allNodes - 1]];
    while(!fin.eof()) {
        getline(fin, line);
        if (line.find(toFind) < 10000) {
            line.erase(0, line.find("lat") + 5);
            line.replace(line.find('"'), 7, ", ");
            line.erase(line.find('"'), line.find('>') - line.find('"') + 1);
            fout << line << "], {icon: finishIcon}).addTo(map);\n";
            break;
        }
    }
    fin.close();
    fout << "\t\t</script>\n";
    fout << "\t</body>\n";
    fout << "</html>";
    cout << "Exporting route to show it on map finished: " << j << '/' << allNodes + 2 << " nodes processed.\n";

    cout << "Program successfully executed!\n";
    system("pause");
    return 0;
}
