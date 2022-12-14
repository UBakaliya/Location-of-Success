#include "prog.h"
int main()
{
  system("clear");
  // maps a Node ID to it's coordinates (lat, lon)
  map<long long, Coordinates> Nodes;
  // info about each footway, in no particular order
  vector<FootwayInfo> Footways;
  // info about each building, in no particular order
  vector<BuildingInfo> Buildings;
  XMLDocument xmldoc;

  cout << "** Welcome to \"Location For Meeting\"**" << endl;
  cout << endl;
  cout << setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "")
    filename = def_filename;

  // Load XML-based map file
  if (!LoadOpenStreetMap(filename, xmldoc))
  {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }

  // Read the nodes, which are the various known positions on the map:
  int nodeCount = ReadMapNodes(xmldoc, Nodes);

  // Read the footways, which are the walking paths:
  int footwayCount = ReadFootways(xmldoc, Footways);

  // Read the university buildings:
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  // Stats
  assert(nodeCount == (int)Nodes.size());
  assert(footwayCount == (int)Footways.size());
  assert(buildingCount == (int)Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;
  graph<long long, double> G;
  // add vertex to our graphs
  for (const auto vertex : Nodes)
    G.addVertex(vertex.first);
  cout << "# of vertices: " << G.NumVertices() << endl;
  addEdges(Footways, G, Nodes);
  cout << "# of edges: " << G.NumEdges() << endl;
  string userInput;
  application(Nodes, Footways, Buildings, G);
  cout << "** Done **" << endl;
  return 0;
}
