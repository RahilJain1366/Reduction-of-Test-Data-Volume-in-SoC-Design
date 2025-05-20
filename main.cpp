#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <algorithm>


using namespace std;

// Function to read the test vectors from a file
vector<string> readScanVectors(const string& filename) {
    vector<string> scanVectors;
    ifstream inputfile(filename.c_str());
    //File is not in folder, throw error
    if (!inputfile) {
        cerr << "Wrong file name" << endl;
        exit(1);
    }
    string line;
    while (getline(inputfile, line)) {
        scanVectors.push_back(line);
    }
    //File is empty, throw error
    if (scanVectors.empty()) {
        cerr << "File is empty" << endl;
        exit(1);
    }
    inputfile.close();
    return scanVectors;
}

// Compress unique scan chain values by assigning unique IDs
map<int, string> idToVectorMap;
map<string, int> uniqueIdMap;
vector<int> compressScanChainValues(const vector<string>& scanVectors) {
    
    vector<int> compressedIds;
    int currentId = 1;

    for (size_t i = 0; i < scanVectors.size(); ++i) {
        const string& vector = scanVectors[i];
        
        uniqueIdMap[vector] = currentId;
        idToVectorMap[currentId] = vector;  // Mapping ID to vector string to execute scan chain logic
        currentId++;
      
        compressedIds.push_back(uniqueIdMap[vector]);
    }
    
    return compressedIds;
}

// Function to apply scan chain logic between two vectors
// It will be called while creating the adjacency matrix
bool scanChainLogic(const string& vector1, const string& vector2) {
    for (size_t i = 0; i < vector1.size(); ++i){
        if (vector1[i] != vector2[i]){
            if (!(vector1[i] == 'X' || vector2[i] == 'X')){
                return false;
        }
    }

    }
    return true;
}

// Create an adjacency matrix based on scan chain logic
vector<vector<int> > createAdjMatrix(const vector<int>& compressedIds) {
    int maxId = *max_element(compressedIds.begin(), compressedIds.end()); // Getting the maximum value for column length in adjacency matrix
    vector<vector<int> > adjMatrix(maxId, vector<int>(maxId,0));

    for (size_t i = 0; i < compressedIds.size(); ++i) {
        for (size_t j = i + 1; j < compressedIds.size(); ++j) {
            // Only connect compressed IDs if scan chain logic is satisfied
            if (scanChainLogic(idToVectorMap[compressedIds[i]], idToVectorMap[compressedIds[j]])) {
                adjMatrix[compressedIds[i] - 1][compressedIds[j] - 1] = 1;
                adjMatrix[compressedIds[j] - 1][compressedIds[i] - 1] = 1;
            }
        }
    }
    return adjMatrix;
}

// Function to find cliques using heuristic algorithm
vector<vector<int> > findCliques(const vector<vector<int> >& adjMatrix, int maxCliques) {
    int n = adjMatrix.size();
    vector<vector<int> > cliques;
    vector<bool> visited(n, false);
    vector<vector<int> > tempAdjMatrix = adjMatrix; // Copy of the graph since wasn't able to modify the adjMatrix to remove

    while (cliques.size() < static_cast<size_t>(maxCliques)) {
        vector<int> clique;
        vector<int> tempClique;
        int maxVertex = -1, maxDeg = -1;

        // Find the vertex with the highest degree
        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                int deg = count(adjMatrix[i].begin(), adjMatrix[i].end(), 1); // Check the number of 1's in the vector 
                // Getting the maxVertex based on the number of 1's
                if (deg > maxDeg) {
                    maxDeg = deg; 
                    maxVertex = i;
                }
            }
        }

        // If there is no vertex itself, then break from the loop
        if (maxVertex == -1) break;

        // Form a new clique starting with maxVertex
        clique.push_back(maxVertex);
        tempClique.push_back(maxVertex);
        visited[maxVertex] = true; // marking that the maxVertex has been visited
        vector<int> connectedVertices;
        set<int> compatibleVertices;
          
        for (int i = 0; i < n; ++i) {
            // If not visited and there is a connection with maxVertex
            if (!visited[i] && tempAdjMatrix[maxVertex][i]) {
                connectedVertices.push_back(i); // Getting all the vertices connected to the maxVertex
            }
        }
        
        // Getting all the vertices possible to form a clique from the conencted vertices and the maxVertex 
        for (size_t i = 0; i < connectedVertices.size(); ++i) {
            int v = connectedVertices[i];
            bool isCompatible = true;
            for (std::set<int>::iterator j = compatibleVertices.begin(); j != compatibleVertices.end(); ++j) {
                int s = *j;
                if (tempAdjMatrix[v][s] == 0) {
                    isCompatible = false;
                    break;
                }
            }
            if (isCompatible){
                compatibleVertices.insert(v); 
            }
        }

        if (!compatibleVertices.empty()) {
            tempClique.insert(tempClique.end(), compatibleVertices.begin(), compatibleVertices.end());
            for (std::set<int>::iterator it = compatibleVertices.begin(); it != compatibleVertices.end(); ++it) {
                visited[*it] = true;
            }
        }

        // Compare size of tempClique and clique, keep the larger one
        if (tempClique.size() > clique.size()) {
            clique = tempClique;
        }

        cliques.push_back(clique); // Add the formed clique to the result

        // Remove the vertices in the clique from G′
        for (size_t i = 0; i < clique.size(); ++i) {
            int v = clique[i];
            for (int j = 0; j < n; ++j) {
                tempAdjMatrix[v][j] = 0;
                tempAdjMatrix[j][v] = 0;
            }
        }
    }
    tempAdjMatrix.clear();
    vector<vector<int> >().swap(tempAdjMatrix); // Swapping with empty array for deallocation of memory
    return cliques;
}


// Compare function to sort in descending order based on size
bool compareSize(const std::vector<int>& vector1, const std::vector<int>& vector2) {
    return vector1.size() > vector2.size();
}

vector<string> compressCliques(const vector<vector<int> >& cliques) {
    vector<string> compressedCliqueVectors;

    // Iterate over each clique
    for (size_t i = 0; i < cliques.size(); ++i) {
        if (cliques[i].empty()) continue;

        // Initialize a compressed vector with 'X', setting it as default
        string compressedVector(idToVectorMap[cliques[i][0] + 1].size(), 'X');

        // Iterate column-wise over all vectors in the clique
        for (size_t j = 0; j < compressedVector.size(); j++) {
            bool hasOne = false, hasZero = false;

            for (size_t k = 0; k < cliques[i].size(); ++k) {
                char value = idToVectorMap[cliques[i][k] + 1][j];
                if (value == '1'){ 
                    hasOne = true;
                    break;
                }
                else if (value == '0'){ 
                    hasZero = true;
                    break;
                }
                // Early termination if either '1' and '0' are found
            }
            if (hasOne && !hasZero)
                compressedVector[j] = '1'; // Setting the compressed vector column position to '1'
            else if (hasZero && !hasOne)
                compressedVector[j] = '0'; // Setting the compressed vector column position to '0'
            // Otherwise it will remain as 'X'
        }
        // Add compressed vector for the current clique
        compressedCliqueVectors.push_back(compressedVector);
    }
    return compressedCliqueVectors;
}

// Main function to read from terminal and call all the function
// Writing the ouptut in the output file as well
int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Incorrect number of arguments" << endl;
        return 1;
    }

    string inputFile = argv[1]; // Getting the input file name from the terminal
    int numCliques;
    int vectorLength;
    string outputFile = argv[4]; // Getting the output file name from the terminal

    stringstream terminal;
    terminal << argv[2];
    terminal >> numCliques; 
    terminal.clear();
    terminal << argv[3];
    terminal >> vectorLength;
    
    if (vectorLength == 4 || vectorLength == 8|| vectorLength == 16 || vectorLength == 32 || vectorLength == 64){
        // Read scan the vectors from file
        vector<string> scanVectors = readScanVectors(inputFile);
        for (size_t i = 0; i < scanVectors.size(); i++) {
            if (scanVectors[i].size() != static_cast<std::size_t>(vectorLength)){
                cerr << "Mismatch between vector length in terminal " << vectorLength << " and input file bits length " << scanVectors[i].size() << endl;
                return 1;
            }
        }
        // Compress scan chain values using mapping
        vector<int> compressedIds = compressScanChainValues(scanVectors);
        
        // Create adjacency matrix based on scan chain logic
        vector<vector<int> > adjMatrix = createAdjMatrix(compressedIds);

        // Find all the cliques using the heuristic approach
        vector<vector<int> > cliques = findCliques(adjMatrix, numCliques);

        // Sorting the cliques in descending order of the size
        std::sort(cliques.begin(), cliques.end(), compareSize);

        // Compress the cliques into only '1' or '0', if not then 'X'
        vector<string> compressedCliqueVectors = compressCliques(cliques);

        // Output the result
        ofstream outFile(outputFile.c_str());
        if (!outFile) {
            cerr << "Error opening output file" << endl;
            return 1;
        }

        /*
        // If you want to see the clique values from the dictionary, please uncomment these lines of code
        // The size of Clique will also be displayed

        // Write cliques and size of cliques to the output file
        int cliqueCount = cliques.size();
        int count = 0;
        for (int i = 0; i < cliqueCount; ++i) {
            outFile << "Clique " << i + 1 << ": ";
            for (size_t j = 0; j < cliques[i].size(); ++j) {
                outFile << cliques[i][j] + 1 << " ";
                count += 1;
            }
            outFile << endl;
            outFile << "The size of Clique " << i + 1 << " is: " << count;
            outFile << endl;
            count = 0;
        }  
        */

        // Write compressed cliques in the output file 
        int compcliqueCount = compressedCliqueVectors.size();
        for (int i = 0; i < compcliqueCount; ++i) {
            for (size_t j = 0; j < compressedCliqueVectors[i].size(); ++j) {
                outFile << compressedCliqueVectors[i][j];
            }
            outFile << endl;
        }

        if (compcliqueCount < numCliques) {
            cout << "Only " << compcliqueCount << " dictionary entries are possible" << endl;
        }

        // Clearing all values to save memory allocation
        uniqueIdMap.clear();
        idToVectorMap.clear();
        scanVectors.clear();
        compressedIds.clear();
        adjMatrix.clear(); //.clear() might not remove memory
        vector<vector<int> >().swap(adjMatrix); // Swapping with empty array for deallocation of memory
        cliques.clear();
        compressedCliqueVectors.clear();

    outFile.close();
    }
    else{
        cerr << "Enter a valid length for the bits" << endl; // Throwing an error if it is not 8,16,32 or 64 bit length.
    }
    return 0;
}