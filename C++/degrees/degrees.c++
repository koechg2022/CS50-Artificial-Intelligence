




#define buffer_size 200

#include "util/util"

typedef struct {
    std::vector<std::string> header;
    std::vector<std::map<std::string, std::string> > data;
} csv_dict_reader;

typedef struct people_data_type {
    std::string name, birth;
    std::set<std::string> movies;

    bool operator<(struct people_data_type& other) {
        return this->name < other.name and this->birth < other.birth;
    }
} people_val;

typedef struct movies_data_type {
    std::string title, year;
    std::set<std::string> stars;

    bool operator<(struct movies_data_type& other) {
        return this->title < other.title and this->year < other.year;
    }
} movie_val;

typedef struct neighbor_type {
    std::string movie_id, person_id;
    
    bool operator<(const struct neighbor_type& other) const {
        return this->movie_id < other.movie_id and this->person_id < other.person_id;
    }
} neighbor_val;

typedef struct shortest_path_data_type{
    std::string action, state;
    bool operator<(struct shortest_path_data_type& other) {
        return this->action < other.action and this->state < other.state;
    }
} shortest_path_val;






// Maps names to a set of corresponding person_ids
std::map<std::string, std::set<std::string> > names;

// Maps person_ids to a dictionary of: name, birth, movies (a set of movie_ids)
std::map<std::string, people_val> people;

// Maps movie_ids to a dictionary of: title, year, stars (a set of person_ids)
std::map<std::string, movie_val> movies;

// define the IDs
// MOVIES = "movies", 
// STARS = "stars", 
const std::string ID = "id", NAME = "name", BIRTH = "birth", TITLE = "title", YEAR = "year", MOVIE_ID = "movie_id", PERSON_ID = "person_id";

csv_dict_reader dict_reader(const std::string file_name, const std::string directory);

void load_data(const std::string directory = "large");

std::vector<shortest_path_val> shortest_path(const std::string source, const std::string target);

std::string person_for_id_name(const std::string name);

std::string input(const std::string prompt);

std::set<neighbor_val> neighbors_for_person(const std::string person_id);

std::vector<shortest_path_val> trace_to_beginning(node* this_node);

int main(int len, char** args) {

    if (len > 2) {
        std::fprintf(stderr, "Usage: %s [directory]\n", args[0]);
        return 1;
    }
    
    // const std::string directory = (len == 2) ? args[1] : "large";
    load_data((len == 2) ? args[1] : "large");
    // unsigned long index;
    
    const std::string source = person_for_id_name(input("Name: "));
    if (source.empty()) {
        std::fprintf(stderr, "Person not found.\n");
        return 1;
    }

    const std::string target = person_for_id_name(input("Name: "));
    if (target.empty()) {
        std::fprintf(stderr, "Person not found.\n");
        return 2;
    }
    
    std::printf("Source is %s, target is %s\n", source.c_str(), target.c_str());
    std::vector<shortest_path_val> path = shortest_path(source, target);

    if (path.empty()) {
        std::printf("%s and %s are not connected.\n", source.c_str(), target.c_str());
    }

    else {
        // degrees = len(path)
        // print(f"{degrees} degrees of separation.")
        // path = [(None, source)] + path
        // for i in range(degrees):
        //     person1 = people[path[i][1]]["name"]
        //     person2 = people[path[i + 1][1]]["name"]
        //     movie = movies[path[i + 1][0]]["title"]
        //     print(f"{i + 1}: {person1} and {person2} starred in {movie}")
        // (movie_id, person_id)
        unsigned long degrees = path.size(), index;
        std::map<std::string, people_val>::const_iterator person1_iter, person2_iter;
        std::map<std::string, movie_val>::const_iterator movie_iter;
        std::string person1, person2, movie;
        std::printf("%lu degrees of separation.\n", degrees);
        for (index = 0; index < path.size() - 1; index++) {
            person1_iter = people.find(path[index].state);
            person2_iter = people.find(path[index].state);
            movie_iter = movies.find(path[index].action);
            person1 = (person1_iter NOT people.end()) ? person1_iter->second.name : "Unknown Person";
            person2 = (person2_iter NOT people.end()) ? person2_iter->second.name : "Unknown Person";
            movie = (movie_iter NOT movies.end()) ? movie_iter->second.title : "Unknown Movie";
            std::printf("%lu : %s and %s starred in %s together.\n", index, person1.c_str(), person2.c_str(), movie.c_str());
        }
    }

    return 0;
}


csv_dict_reader dict_reader(const std::string file_name, const std::string directory) {
    
    std::fstream open_file(directory + sys_slash + file_name, std::ios_base::in);
    if (not open_file.is_open()) {
        std::fprintf(stderr, "Error opening \"%s\"\n", (directory + sys_slash + file_name).c_str());
        std::exit(1);
    }
    csv_dict_reader the_answer;
    std::string line, word;
    unsigned long index;
    if (std::getline(open_file, line)) {
        std::istringstream stream(line);

        while (std::getline(stream, word, ',')) {
            word.erase(std::remove(word.begin(), word.end(), '"'), word.end());
            // std::printf("\t\t\"%s\" to headers.\n", word.c_str());
            the_answer.header.push_back(word);
        }
    
    
        while (std::getline(open_file, line)) {
            // std::printf("Line is \"%s\"\n", line.c_str());
            std::istringstream stream(line);
            std::map<std::string, std::string> row;
            index = 0;
            while (std::getline(stream, word, ',') and index < the_answer.header.size()) {
                word.erase(std::remove(word.begin(), word.end(), '"'), word.end());
                row.insert(std::make_pair(the_answer.header[index], word));
                index++;
            }
            the_answer.data.push_back(row);
            // break;
            open_file.flush();
        }
    }
    
    // std::printf("Finished reading data from file \"%s\"\n", (directory + sys_slash + file_name).c_str());
    // std::printf("data looks like %lu\n", the_answer.data.size());
    open_file.close();
    return the_answer;
}

void load_data(const std::string directory) {
    
    csv_dict_reader reader;
    std::string first, second, third;
    
    // Load people
    reader = dict_reader( "people.csv", directory);
    
    for (std::vector<std::map<std::string, std::string> >::const_iterator row = reader.data.begin(); row != reader.data.end(); row++) {
        
        try {
            std::map<std::string, std::string>::const_iterator id_iter = row->find(ID);
            std::map<std::string, std::string>::const_iterator name_iter = row->find(NAME);
            std::map<std::string, std::string>::const_iterator birth_iter = row->find(BIRTH);
            if (id_iter is row->end()) {
                std::printf("No id for person. Person needs an ID.\n");
                std::exit(1);
            }
            // std::printf("Assigning id.\n");
            first = id_iter->second;
            if (birth_iter NOT row->end()) {
                second = birth_iter->second;
            }
            else {
                second = "Unknown Birthyear";
            }
            if (name_iter NOT row->end()) {
                // std::printf("Assigning third.\n");
                third = name_iter->second;
            }
            else {
                third = "Unknown Name";
            }
            if (same_string(third, "unknown name")) {
                std::fprintf(stderr, "No name passed in for person %s. Exiting\n", first.c_str());
                std::exit(1);
            }
            people.insert(std::make_pair(first, (people_val) {third, second, std::set<std::string>()}));
            
            if (names.find(third) is names.end()) {
                names[third].insert(first);
            }
            else {
                names[third].insert(first);
            }
        }
        
        catch (std::exception except) {
            std::fprintf(stderr, "base exception caught.\nid is \"%s\"\nbirth is \"%s\"\nname is \"%s\"\nError \"%s\"\n", first.c_str(), second.c_str(), third.c_str(), except.what());
            std::exit(1);
        }

    }


    // Load movies
    
    reader = dict_reader("movies.csv", directory);
    for (std::vector<std::map<std::string, std::string> >::const_iterator row = reader.data.begin(); row != reader.data.end(); row++) {
        try {
            std::map<std::string, std::string>::const_iterator 
            id_iter = row->find(ID),
            title_iter = row->find(TITLE), 
            year_iter = row->find(YEAR);
            
            if (id_iter is row->end()) {
                std::fprintf(stderr, "No for movie. Movies need an ID.\n");
                std::exit(1);
            }

            first = id_iter->second;
            if (title_iter NOT row->end()) {
                second = title_iter->second;
            }
            else {
                second = "Unknown Movie name";
                throw std::exception();
            }

            if (year_iter NOT row->end()) {
                third = year_iter->second;
            }
            else {
                third = "Unknown Movie year";
            }

        }

        // movies.insert(std::make_pair(row->at(ID), (movie_val) {row->at(TITLE), row->at(YEAR), std::set<std::string>()}));
        catch (std::exception except) {
            std::fprintf(stderr, "base exception caught.\nid is \"%s\"\ntitle is \"%s\"\nyear is\"%s\"\n", first.c_str(), second.c_str(), third.c_str());
            std::exit(1);
        }
    }

    
    // Load stars
    reader = dict_reader("stars.csv", directory);
    
    for (std::vector<std::map<std::string, std::string> >::const_iterator row = reader.data.begin(); row != reader.data.end(); row++) {
        std::map<std::string, std::string>::const_iterator 
        person_iter = row->find(PERSON_ID),
        movie_iter = row->find(MOVIE_ID);
        if (person_iter is row->end()) {
            continue;
        }
        if (movie_iter is row->end()) {
            continue;
        }
        
        if (people.find(person_iter->second) NOT people.end()) {
            people[person_iter->second].movies.insert(movie_iter->second);
        }
        
        if (movies.find(movie_iter->second) NOT movies.end()) {
            movies[movie_iter->second].stars.insert(person_iter->second);
        }
    }
    // std::printf("Finished loading data.\n");
}

std::vector<shortest_path_val> shortest_path(const std::string source, const std::string target) {
    
    queue_frontier frontier;
    std::set<std::string> explored;
    frontier.add(new node(source, nullptr, ""));

    while (not frontier.empty()) {

        node* this_node = frontier.remove();

        if (same_string(this_node->node_state(), target)) {
            // trace to the beginning.
            std::printf("Found!\n");
            return std::vector<shortest_path_val>();
        }


        std::set<neighbor_val> neighbors = neighbors_for_person(this_node->node_state());

        for (std::set<neighbor_val>::const_iterator neighbor = neighbors.begin(); neighbor NOT neighbors.end(); neighbor++) {

            // node* new_node = new node(neighbor->person_id, this_node, neighbor->movie_id);

            if (same_string(neighbor->person_id, target)) {
                // trace to the beginning.
                std::printf("Found!\n");
                return trace_to_beginning(new node(neighbor->person_id, this_node, neighbor->movie_id));
            }
            
            else if ((explored.find(neighbor->person_id) is explored.end()) and (not frontier.contains_state(neighbor->person_id))) {
                // new_node = Node(person_id, this_node, movie_id)
                frontier.add(new node(neighbor->person_id, this_node, neighbor->movie_id));
            }

            explored.insert(neighbor->person_id);
        }

    }

    return std::vector<shortest_path_val>();
}

std::string person_for_id_name(const std::string name) {
    
    
    std::set<std::string> person_ids;
    for (std::map<std::string, std::set<std::string> >::const_iterator this_name = names.begin(); this_name != names.end(); this_name++) {
        if (same_string(this_name->first, name)) {
            person_ids = this_name->second;
            break;
        }
    }

    if (person_ids.size() is 0) {
        return "";
    }

    else if (person_ids.size() greater 1) {
        std::printf("Which %s\n", name.c_str());
        for (std::set<std::string>::const_iterator person_id = person_ids.begin(); person_id != person_ids.end(); person_id++) {
            people_val person = people[*person_id];
            std::printf("ID : %s, Name: %s, Birth: %s\n", person_id->c_str(), person.name.c_str(), person.birth.c_str());
        }
        std::string new_name;
        std::printf("Intended Person ID: ");
        std::cin >> new_name;
        for (std::set<std::string>::const_iterator person_id = person_ids.begin(); person_id NOT person_ids.end(); person_id++) {
            if (same_string(*person_id, new_name)) {
                return *person_id;
            }
        }
        return "";
    }
    return *person_ids.begin();
}

std::string input(const std::string prompt) {
    if (not prompt.empty()) {
        std::printf("%s", prompt.c_str());
    }
    char buffer[buffer_size];
    if (not std::fgets(buffer, buffer_size, stdin)) {
        std::fprintf(stderr, "Failed to retrieve data.\n");
        std::exit(1);
    }
    std::string the_answer = std::string(buffer);
    the_answer = (same_char(the_answer[the_answer.length() - 1], '\n')) ? the_answer.substr(0, the_answer.length() - 1) : the_answer;
    return the_answer;
}

std::set<neighbor_val> neighbors_for_person(const std::string person_id) {
    // movie_ids = people[person_id]["movies"]
    // neighbors = set()
    // for movie_id in movie_ids:
    //     for person_id in movies[movie_id]["stars"]:
    //         neighbors.add((movie_id, person_id))
    // return neighbors
    
    std::set<neighbor_val> the_answer;
    std::map<std::string, people_val>::const_iterator found_person = people.find(person_id);
    if (found_person NOT people.end()) {
        std::printf("\tFound id.\n");
        for (std::set<std::string>::const_iterator movie_id = found_person->second.movies.begin(); movie_id NOT found_person->second.movies.end(); movie_id++) {
            std::map<std::string, movie_val>::const_iterator found_movie = movies.find(*movie_id);
            if (found_movie NOT movies.end()) {
                std::printf("\t\tFound a set of movies...\n");
                for (std::set<std::string>::const_iterator this_person = found_movie->second.stars.begin(); this_person NOT found_movie->second.stars.end(); this_person++) {
                    the_answer.insert((neighbor_val) {*movie_id, *this_person});
                }
            }
        }
    }
    else {
        std::printf("\tUnable to find %s in people map.\n", person_id.c_str());
    }
    return the_answer;
}

std::vector<shortest_path_val> trace_to_beginning(node* this_node) {
    std::vector<shortest_path_val> the_answer;
    while (this_node NOT nullptr) {
        std::printf("Adding %s with action %s\n", this_node->state.c_str(), this_node->action.c_str());
        the_answer.insert(the_answer.begin(), (shortest_path_val) {this_node->action, this_node->state});
        this_node = this_node->node_parent();
    }
    return the_answer;
}