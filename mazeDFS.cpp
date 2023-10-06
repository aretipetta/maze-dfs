#include <iostream>
#include <regex>
#include <stack>
#include <list>

using namespace std;

int getDimension(string dim) {  // validate maze's dimensio
	regex reg("[1-9][0-9]?"); 
	string d;
	cout << dim << " of the maze as an integer (available range = [1 - 99]): \n";
	cin >> d;
	while (!regex_match(d, reg)) {
		cout << "Wrong input. " << dim << " of the maze as an integer (available range = [1 - 99]): \n";
		cin >> d;
	}
	cout << "---------------------------------------------------------------\n\n";
	return stoi(d);
}

int checkChoice() {  // validate input to start (0 == run | 1 == finish/exit)
	regex reg("(0|1)");
	string ch;
	cout << "Press '0' to start or press '1' to exit: \n";
	cin >> ch;
	while (!regex_match(ch, reg)) {
		cout << "Wrong input. Press '0' to start or press '1' to exit: \n";
		cin >> ch;
	}
	cout << "---------------------------------------------------------------\n\n";
	return stoi(ch);
}

int checkInputCell() {  // validate input for each cell that defines whether it is a wall or not (0 == no wall | 1 == wall)
	regex reg("(0|1)");
	string c;
	cout << "Press '0' if the cell is empty or press '1' if the cell is a wall: \n";
	cin >> c;
	while (!regex_match(c, reg)) {
		cout << "Wrong input. Press '0' if the cell is empty or press '1' if the cell is a wall: \n";
		cin >> c;
	}
	return stoi(c);
}

string* initializeMaze(int height, int width) {   //  '#' == wall-cell			'Χ' == no wall-cell
	string* maze = new string[height];
	string r = "";
	for (int i = 0; i < width; i++) r.append("#"); // init maze with '#' (==wall)
	for (int i = 0; i < height; i++) maze[i] = r; 
	// validate input for each cell and if it's needed we change the value to 'X'
	cout << "For each cell of the maze press '0' if that cell is empty so we can \nvisit it or press '1' if the cell is a wall so we can't visit it.\n\n";
	for (int i = 0; i < height; i++) { 
		for (int j = 0; j < width; j++) {  
			cout << "-> Is Cell[" << i+1 << ", " << j+1 << "] empty?\n";
			if (checkInputCell() == 0) maze[i][j] = 'X'; 
		}
	}
	cout << "---------------------------------------------------------------\n\n";
	return maze;
}

void printMaze(string* maze, int height) {
	cout << "This is how the maze looks like:\n";
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < maze[i].length(); j++) cout << maze[i][j] << " "; 
		cout << "\n";
	}
	cout << "\nX: Empty cell.\n#: Wall.\nS: Starting Cell.\nE: Ending Cell.\n";
	cout << "---------------------------------------------------------------\n\n";
}

bool searchStateInClosed(list <string> closed, string state) {  // if the state already exists to closed, there is no need to run the same proccess 
	list <string> ::iterator i;
	for (i = closed.begin(); i != closed.end(); ++i) if (*i == state) return true; 
	return false;
}

bool searchStateInRestSolution(list <string> &solution, list<string>:: iterator &it, string state) {
	list <string> ::iterator i = it;
	++i; // the searching of the solution starts from the next 'step'
	for (i; i != solution.end(); ++i) if (*i == state) return true; 
	return false;
}

void findPath(list <string> solution, string* maze, int mazeHeight) {  // we need to find the distinct path after we have found the solution
	list<string> path;
	list<string> ::iterator i = solution.begin();
	string temp = *i;
	path.push_front(temp); // first thing first: add the end to that path
	int prevRow = stoi(temp.substr(0, temp.find(", ")));
    int prevColumn = stoi(temp.substr(temp.find(", ") + 1, temp.length()));
	int row, column;
	++i;
	while (i != solution.end()) {
		temp = *i;
		if (!searchStateInRestSolution(solution, i, temp)) {  //αν η κατάσταση που πήρε από το solution δεν υπάρχει ξανά σε αυτό το αποθηκεύει στο path
															  // if the state that comes from the solution doesn't exist then we have to add it to path
			                                                  //αν υπάρχει ξανά στο υπόλοιπο του solution τότε σημαίνει πως έχει κάνει backtracking και
															  // but if it exists in the rest of the solution that means that we had a backtracking and
															  //περνά ξανά από το ίδιο κελί και άρα δε χρειάζεται να το αποθηκεύσει ακόμα στο path
															  // we have seen this cell before, so there is no need to add it to path
			row = stoi(temp.substr(0, temp.find(", ")));
			column = stoi(temp.substr(temp.find(", ") + 1, temp.length()));
			if ((prevRow == row && abs(prevColumn - column) == 1) || (prevColumn == column && abs(prevRow - row) == 1)) { //αν είναι γειτονικό (πρόγονος) του προηγούμενου κελιού
				path.push_front(temp);
				if (maze[row - 1][column - 1] == 'X') maze[row - 1][column - 1] = '*'; // -1 bc we save them with +1 but the indexing is -1
				prevRow = row;
				prevColumn = column;
			}
		}
		++i;
	}
	cout << "This is the path which DFS found:\n";
	i = path.begin();
	cout << "Cell[" << *i << "]\n";
	++i;
	while (i != path.end()) {
		cout << "-> Cell[" << *i << "]\n";
		++i;
	}
	cout<<"\n";
	printMaze(maze, mazeHeight);
}

void dfs(string* maze,int mazeHeight, int startRow, int startColumn, int endRow, int endColumn) {
	if (startRow == endRow && startColumn == endColumn) {
		cout << "\nThis is the path which DFS found: \nCell["<<startRow+1<<", "<<startColumn+1<<"]\n";
		cout << "---------------------------------------------------------------\n\n";
		return;
	}
	else {
		string endState = to_string(endRow) + ", " + to_string(endColumn);
		string start = to_string(startRow) + ", " + to_string(startColumn);
		stack <string> searchStates;  // all the states that we might check - metwpo anazitisis
		searchStates.push(start);
		list <string> closed;
		list <string> solution;
		bool found = false;
		while (!searchStates.empty()) {   // while there are states that we haven't checked yet
			string currentState = searchStates.top();
			searchStates.pop();
			int i = stoi(currentState.substr(0, currentState.find(", ")));
			int j = stoi(currentState.substr(currentState.find(", ") + 1, currentState.length()));
			solution.push_front(to_string(i + 1) + ", " + to_string(j + 1));
			// if the currentState has already been added to closed then we don't need to check its children and we can continue with the next state of the stack
			// otherwise, then we have to check those children and each of them must be added to stack
			// finally, the current state should be added to closed when we have found all its childrens
			if (!searchStateInClosed(closed, currentState)) {   
				if (currentState == endState) {  // validate if the current state is the final one
					found = true;
					break;
				}
				else {
					if (j - 1 >= 0) {  // if we can go left
						if (maze[i][j - 1] != '#') searchStates.push(to_string(i) + ", " + to_string(j - 1)); 
					}
					if (i + 1 < mazeHeight) {   // if we can go down
						if (maze[i + 1][j] != '#') searchStates.push(to_string(i + 1) + ", " + to_string(j)); 
					}
					if (j + 1 < maze[0].length()) {   // if we can go right
						if (maze[i][j + 1] != '#') searchStates.push(to_string(i) + ", " + to_string(j + 1)); 
					}
					if (i - 1 >= 0) {   // if we can go up
						if (maze[i - 1][j] != '#') searchStates.push(to_string(i - 1) + ", " + to_string(j));
					}
				}
				closed.push_back(currentState);   // add the checked state to closed ones
			}
		}
		if (found) findPath(solution, maze, mazeHeight); // the solution has been defined and we need to find the distinct path
		else cout << "\nThere is no solution. DFS couldn't find any path.\n\n---------------------------------------------------------------\n\n";
	}
}

int getPoints(int dim, string msg) {   // validate start/end position
	regex reg("[1-9][0-9]?");
	string s;
	cout <<msg<< " (available range = [1 - "<<dim<<"])\n";
	cin >> s;
	while (true) {   // the input must be an integer between 0 and dim
		while (!regex_match(s, reg)) {
			cout << "Wrong input. "<<msg<<"\n";
			cin >> s;
		}
		if (stoi(s) > 0 && stoi(s) <= dim) break; 
		cout << "Wrong input. Available range = [1 - "<<dim<<"]: \n";
		cin >> s;
	}
	cout << "---------------------------------------------------------------\n\n";
	return stoi(s)-1;  // -1 due to array indexing (start == 0)
}

string* setStartEndMaze(string* maze,int mazeHeight, int startRow, int startColumn, int endRow, int endColumn) {
	maze[startRow][startColumn] = 'S';
	maze[endRow][endColumn] = 'E';
	printMaze(maze, mazeHeight);
	return maze;
}

int main()
{
	int height, width;
	while (checkChoice() == 0) { // checkChoice == 1 ==>end of program
		height = getDimension("Height");
		width = getDimension("Width");
		string* maze = initializeMaze(height, width);
		printMaze(maze, height);
		int startRow, startColumn, endRow, endColumn;
		startRow = getPoints(height, "Row of starting cell:");
		startColumn = getPoints(width, "Column of starting cell:");
		endRow = getPoints(height, "Row of ending cell:");
		endColumn = getPoints(width, "Column of ending cell:");
		maze = setStartEndMaze(maze, height, startRow, startColumn, endRow, endColumn);
		dfs(maze, height, startRow, startColumn, endRow, endColumn);
		cout << "#######################################################################\n\n";
	}
}