#include <cctype>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <thread>
#include <vector>

using namespace std;

// size of the grid, it can be adjusted
int N = 5, M = 5;


// util namesapce contians some utility functions
namespace util {

int dx[] = {-1, 0, 1, 0};
int dy[] = {0, 1, 0, -1};

vector<vector<char>> parse_input(string file_name) {
    ifstream ifs(file_name);
    vector<vector<char>> input;
    string line;
    while (getline(ifs, line)) {
        vector<char> temp;
        for (char ch : line) {
            if (isalpha(ch) || ch == '-') {
                temp.push_back(ch);
            }
        }
        input.push_back(temp);
    }
    ifs.close();
    return input;
};

// here i check if the (x, y) is on the border of the grid
bool is_valid_position(int x, int y) {
    return (x >= 0 && x < N) && (y >= 0 && y < M);
}

// this functions returns list of neighbour cells to given (x, y) like up, down, left , right 
vector<pair<int, int>> get_neighbour_postions(int x, int y) {
    vector<pair<int, int>> neighbours;
    for (int i = 0; i < 4; i++) {
        int new_x = x + dx[i];
        int new_y = y + dy[i];
        if (is_valid_position(new_x, new_y)) {
            neighbours.push_back(make_pair(new_x, new_y));
        }
    }
    return neighbours;
}

char get_last_letter() {
    return  'A' + (N * M - 1);
}

vector<char> get_consecutive_letters(char letter) {
    if (letter == 'A') {
        return {'B'};
    }
    char last_letter = get_last_letter();
    if (letter == last_letter) {
        return {(char) (last_letter - 1)};
    }
    return {(char)(letter - 1), (char)(letter + 1)};
}

// it is used to populate domain of each variabale before running csp
vector<char> get_possible_domain(char curr) {
    if (curr == '-') {
        vector<char> letters;
        char last_letter = get_last_letter();
        for (char ch = 'A'; ch <= last_letter; ch++) {
            letters.push_back(ch);
        }
        return letters;
    }
    return {curr};
}

int map_position_to_index(int x, int y) {
    return x * M + y;
}

// print functions are used for debug purposes
void print_vector(vector<char> v) {
    for (char ch : v) {
        cout << ch << " ";
    }
    cout << "\n";
}

void print_answer(vector<vector<char>> domains) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            cout << domains[map_position_to_index(i, j)][0] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}


}  // namespace util

// for some reason, this function got pretty complicated and unreadable, but what it does is simple. basiacally 
// for each value in domain of some cell, i check if it is a possible value, meaning its neigbors has consecutive
// letters to that cell in their domains. note that i check the cases if the only one neighbour contains both of the 
// consecutive letters in their domain, then it is not considered valid.
// if the domain of the checked cell changes, then all its neigbors are added to worklist to check again
// it returns false if some domain becomes empty, otherwise updates the domain and returns true
bool run_ac3(const vector<vector<char>> &input, 
            vector<vector<char>> &domains, const vector<vector<bool>> &is_assigned) {

    queue<pair<int, int>> work_list;
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (!is_assigned[i][j]) work_list.push(make_pair(i, j));
        }
    }

    int cnt = 0;
    while (!work_list.empty()) {
        pair<int, int> top = work_list.front();
        work_list.pop();
        vector<pair<int, int>> neighbours =
            util::get_neighbour_postions(top.first, top.second);

        char current_char = input[top.first][top.second];

        int current_char_domain_ind = top.first * M + top.second;

        vector<char> new_domain;

        for (char curr_domain : domains[current_char_domain_ind]) {
            map<char, set<int>> occurance;
            set<int> neighbours_with;
            for (pair<int, int> neighbour : neighbours) {
                int neighbour_domain_ind =
                    neighbour.first * M + neighbour.second;
                for (char neighbour_domain : domains[neighbour_domain_ind]) {
                    if (abs(curr_domain - neighbour_domain) == 1) {
                        neighbours_with.insert(neighbour_domain_ind);
                        occurance[neighbour_domain].insert(neighbour_domain_ind);
                    }
                }
            }

            auto ne_letters = util::get_consecutive_letters(curr_domain);

            if (ne_letters.size() == 1 && occurance[ne_letters[0]].size() > 0) {
                new_domain.push_back(curr_domain);
            } else {
                if (occurance[ne_letters[0]].size() > 0 && occurance[ne_letters[1]].size() > 0) {
                    if (occurance[ne_letters[0]].size() == 1 && occurance[ne_letters[1]].size() == 1) {
                        if (*occurance[ne_letters[0]].begin() != *occurance[ne_letters[1]].begin()) {
                            new_domain.push_back(curr_domain);
                        }
                    } else {
                        new_domain.push_back(curr_domain);
                    }
                }
            }
        }

        if (new_domain.empty()) {
            return false;
        }

        if (new_domain.size() != domains[current_char_domain_ind].size()) {
            for (pair<int, int> neighbour : neighbours) {
                work_list.push(neighbour);
            }
        }
        domains[current_char_domain_ind] = new_domain;
    }
    return true;
}


// This function is kind of precheck to partially assigned domain to reduce the search space further.
// First it check if the already assinged values contains duplicate values. then it check there is a already assinged
// values that does not have consecutive letters in its all assigned neigbors thus viloating the snake condition
// the second condition is not tested if there is a negibors that are not assigned yet
bool not_satisfied(const vector<vector<char>> &domains, const vector<vector<bool>> &is_assigned) {
    map<char, int> cnt;
    for (int i = 0; i < domains.size(); i++) {
        if (domains[i].size() == 1) {
            cnt[domains[i][0]]++;
        }
        if (cnt[domains[i][0]] > 1) {
            return true;
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            char cell = domains[i * M + j][0];
            vector<pair<int, int>> neighbours = util::get_neighbour_postions(i, j);
            map<char, int> neigbors_cells;

            int assgined_neigbor_cnt = 0;

            for (auto neighbour : neighbours) {
                int neighbour_domain_ind = util::map_position_to_index(neighbour.first, neighbour.second);

                neigbors_cells[domains[neighbour_domain_ind][0]]++;

                if (is_assigned[neighbour.first][neighbour.second]) {
                    assgined_neigbor_cnt++;
                }
            }

            if (assgined_neigbor_cnt != neighbours.size()) { // if all of the neigbors not assigned yet, then skip checking
                continue;                                    // since they can have consecutive letters later
            }

            vector<char> consecutive_letters = util::get_consecutive_letters(cell);

            for (char ch : consecutive_letters) {
                if (neigbors_cells[ch] == 0) {
                    return true;
                }
            }
        }
    }

    return false;
}

// chose the unassinged variable with minimum domain size (MRV)
pair<int, int> get_unassigned_cell_using_mrv(const vector<vector<char>> &domains, const vector<vector<bool>> &is_assigned) {
    pair<int, int> selected_cell = {-1, -1};

    int sz = INT_MAX;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (!is_assigned[i][j]) {
                int ind = util::map_position_to_index(i, j);
                if (sz > domains[ind].size()) {
                    selected_cell = {i, j};
                    sz = domains[ind].size();
                }
            }
        }
    }
    return selected_cell;
}

bool backtrack(const vector<vector<char>>& input, vector<vector<char>> domains,
               vector<vector<bool>> &is_assigned) {

    pair<int, int> selected_cell = get_unassigned_cell_using_mrv(domains, is_assigned);

    if (selected_cell.first == -1) {
        cout << "Solution found :" << "\n";
        util::print_answer(domains);
        return true;
    }

    int ind = util::map_position_to_index(selected_cell.first, selected_cell.second);

    for (char value : domains[ind]) {
        auto temp_domain = domains;

        temp_domain[ind] = {value};

        if (not_satisfied(temp_domain, is_assigned)) { // precheck satisfaction,
            continue;
        }

        is_assigned[selected_cell.first][selected_cell.second] = true;

        if (run_ac3(input, temp_domain, is_assigned) && backtrack(input, temp_domain, is_assigned)) {
            return true;
        }

        is_assigned[selected_cell.first][selected_cell.second] = false;
    }
    return false;
}

int main(int argc, char* argv[]) {
    string file_name = argv[1];
    vector<vector<char>> input = util::parse_input(file_name);

    N = input.size();
    M = input[0].size();
    vector<vector<char>> domains; // domain keeps possible domain values for each position, 
                                  // position (i, j) is mapped to (i * M + j) in domains varaible (i did it for simpler data structure)
                                  // if the is_assigned[i][j] is true, then corresponding domain size is 1 and it is the assigned value.
    
    vector<vector<bool>> is_assigned(N, vector<bool>(M, false));

    // domain of each cell is populated and fixed varibles are marked
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            auto temp = util::get_possible_domain(input[i][j]);
            domains.push_back(temp);
            if (input[i][j] != '-') {
                is_assigned[i][j] = true;
            }
        }
    }
    // since there is already some fixed variables, I run ac3 before starting solution to reduce 
    // domain size. but it is optional
    run_ac3(input, domains, is_assigned);


    // since letters with one consecutive neigbor are the least constraining, i presort them before recursion
    // and since it does not change during execution i dont sort them each time in recursing in 
    // order to avoid exra overhead. 
    //However for some input cases it took longer if i did this sorting. I think maybe my assumption about 
    //least constraining value is not correct
    for (int i = 0; i < domains.size(); i++) {
        sort(domains[i].begin(), domains[i].end(), [](char a, char b){
            return util::get_consecutive_letters(a).size() < util::get_consecutive_letters(b).size();
        });
    }

    if (!backtrack(input, domains, is_assigned) ) {
        cout << "No solution found!" << "\n";
    }
    return 0;
}