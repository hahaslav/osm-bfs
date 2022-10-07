# A router, based on the BFS algorithm, for OpenStreetMap data
###### English|[Українська](https://github.com/hahaslav/osm-bfs/blob/master/README_UK.md)
### ***Caution! This app is not meant to be used as a serious road router.***
## Building
The source code can be found in *[main.cpp](https://github.com/hahaslav/osm-bfs/blob/master/main.cpp)*. The [released app](https://github.com/hahaslav/osm-bfs/releases/latest) was built with GNU GCC 5.1.0 with parameters `-std=c++11 -Wall -O2`.
## Implementation
The app uses the [Breadth-first search algorithm](https://en.wikipedia.org/wiki/Breadth-first_search) to find the path between two nodes. This path will have the least amount of nodes.
![The algorithm will choose the long way with three nodes but not the shortest one with 20 nodes](https://i.imgur.com/bGbNNhh.png)
## Usage
1. Download any *.osm* file with an area where you want to find a way. Rename this file to *input.osm* and put it in the same folder where the app executable is located.
2. Run the app.
3. Enter the id of the start and the finish nodes of the way.
4. Has the app output "*Program successfully executed!*"? If yes, congratulations!
## Reading the output data
### Method **I**:
Open a generated *map.html* in any web browser. It has a map with the way marked on it.
### Method **II**:
1. Start opening [JOSM](https://josm.openstreetmap.de).
2. At the same time, open a generated *JOSM.txt*.
3. Copy its content into the clipboard.
4. In JOSM, open *File>Download object...* (*Ctrl+Shift+O*):
    * In the list *Object type* choose *node*.
    * Check that the field *Object ID* has the copied text.
    * Unflag *Download referrers (parent relations)*.
    * Flag *Separate Layer*.
5. Done!
## Also
The app was tested in 2020 only on Windows 10. It was compatible with exported data from [OpenStreetMap](https://www.openstreetmap.org) and [overpass turbo](https://overpass-turbo.eu/). If the data has too many nodes, the memory for the algorithm may be overflowed (I guess so).
