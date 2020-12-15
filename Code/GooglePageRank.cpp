#include<bits/stdc++.h>
#define NODE 1005 // The dataset taken has 1005 nodes which is initialised in the NODE macro
#define P 0.15 // Teleportation Probability, In case of sink node with this much value times the probability of selecting each node we'll go to n nodes
#define itr 5// Number of iterations for which the page rank will be updated.

using namespace std;

void read_record(vector<int> *adj, vector<int> *inadj, int *indegree, int *outdegree)// Function to read the graph as edge table from the csv file
{ // File pointer
	fstream fin;
	// Open an existing file
	fin.open("email-Eu-core.csv", ios::in);
	// Read the Data from the file
	// as String Vector
  vector<string> row;
	string line, word;

	while (getline(fin, line)) {
		row.clear();
		// // read an entire row and
		// // store it in a string variable 'line'
		// // used for breaking words
		stringstream s(line);
		// // read every column data of a row and
		// // store it in a string variable, 'word'
		while (getline(s, word, ' ')) {

			// add all the column data
			// of a row to a vector
			row.push_back(word);
		}

		// convert string to integer for storing the nodes index
		adj[stoi(row[0])].push_back(stoi(row[1]));// adding the outgoing edges as the nodes in the adjacency list.
    inadj[stoi(row[1])].push_back(stoi(row[0]));// adding the incoming edges as the nodes in the adjacency list.
    outdegree[stoi(row[0])] += 1;// computing the outdegree
    indegree[stoi(row[1])] += 1;// computing the indegree
	}
}

void write_csv(string filename, vector<pair<double,int>> &finalrank){// Store the results in a csv file.
    // Make a CSV file with one column of integer values
    // filename - the name of the file

    // Create an output filestream object
    ofstream myFile(filename);

    // Send the column name to the stream
    myFile << "Node , PageRank" << "\n";

    // Send data to the stream
    for(auto i=finalrank.begin(); i!=finalrank.end(); i++)
    {
        myFile << i->second << "," << i->first << "\n";
    }

    // Close the file
    myFile.close();
}

void makematrix(vector<vector<double>> &A, vector<int> *adj, int *outdegree){// Function to build the matrix A in the google page rank matrix
  for(int j=0; j<NODE; j++){
		bool mark = false;
    for(auto i=adj[j].begin(); i!=adj[j].end(); i++){
      A[*i][j] = 1/(double)outdegree[j];// filling the matrix with each column treating as the nodes with the probability of the number of nodes it points to
			mark = true;
    }
		if(mark==false){// if there is sink then fill the teleport probability to keep the matrix M column stochastic
			for(int i=0; i<NODE; i++)
				A[i][j] = 1/(double)NODE;
		}
  }
}

vector<vector<double>> matmul(vector<vector<double>> &A, vector<vector<double>> &B){// Basic program for multiplying 2 matrices
  int m = A.size();
  int p = A[0].size();
  int n = B[0].size();
  vector<vector<double>> C(m, vector<double>(n,0));
  for(int i=0; i<m; i++){
    for(int j=0; j<n; j++){
      for(int k=0; k<p; k++){
        C[i][j] += A[i][k]*B[k][j];
      }
    }
  }

  return C;
}

vector<vector<double>> scalmul(double num, vector<vector<double>> &B){// Basic Program for multiplying each elements of a metrix with a constant scalar
  vector<vector<double>> C(NODE, vector<double>(NODE,0));

  for(int i=0; i<NODE; i++){
    for(int j=0; j<NODE; j++){
      C[i][j] = num*B[i][j];
    }
  }

  return C;
}

vector<vector<double>> matsum(vector<vector<double>> &A, vector<vector<double>> &B){// Basic program to sum two matrices with dimension of both the matrices to be equal
  vector<vector<double>> C(NODE, vector<double>(NODE,0));

  for(int i=0; i<NODE; i++){
    for(int j=0; j<NODE; j++){
      C[i][j] = A[i][j] + B[i][j];
    }
  }

  return C;
}

int main(){

  vector<int> adj[1005];// Adjacency list representation of the graph
  vector<int> inadj[1005];// Adjacency list of the inward edges being pointed to a particular nodes by other nodes.
  int indegree[1005] = {0};// indegree value array for each node
  int outdegree[1005] = {0};// outdegree value array for each node
  vector<vector<double>> A(NODE, vector<double>(NODE,0));// A is the matrix which is formed in the basic page rank algorithm
  vector<vector<double>> B(NODE, vector<double>(NODE,1/(double)NODE));// B is the matrix for the teleport part and to overcome the problems in the basic page rank algorithm

  read_record(adj,inadj,indegree,outdegree);// import the graph as adjacency list

  makematrix(A,adj,outdegree);// turn the graph into the ranking matrix A

  vector<vector<double>> M(1005, vector<double>(1005,0));// Final Google Page Rank MAtrix intialisation to be used for the page rank iteration
  double D = (1-P);// The weights with which the outedges will be picked up


  vector<vector<double>> X(1005, vector<double>(1005,0));// For storing the intermediate computations
  X = scalmul(D,A);

  vector<vector<double>> Y(1005, vector<double>(1005,0));
  Y = scalmul(P,B);

  M = matsum(X,Y);// M = (1-P)*A + P*B

  vector<vector<double>> v(NODE, vector<double>(1,1/(double)NODE));// Intiialising the page rank vector for the first iteration
  vector<vector<double>> temp(NODE, vector<double>(1,0));// Temp variable for storing the intermediate results
  for(int i=0; i<itr; i++){// Performing V = M*V
    temp = matmul(M,v);
    v = temp;
  }

	vector<pair<double,int>> finalrank;
	for(int i=0; i<NODE; i++){// preparing the final rank list
		finalrank.push_back(make_pair(v[i][0],i));
	}

	sort(finalrank.begin(),finalrank.end(),greater<pair<double,int>>()); // Sorting the final rank list form highest to lowest

	cout<<"   NODE   "<<"PAGERANK "<<endl;
	for(auto i=finalrank.begin(); i!=finalrank.end(); i++)// Printing the sorted result
		cout<<"Node "<<i->second<<" "<<i->first<<endl;

	write_csv("CodeOutput.csv", finalrank);	// Calling the function to write in the file


  return 0;
}
