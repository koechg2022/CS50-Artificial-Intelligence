

#define buffer_size 100

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
} people_data_type;

typedef struct movies_data_type {
    std::string title, year;
    std::set<std::string> stars;

    bool operator<(struct movies_data_type& other) {
        return this->title < other.title and this->year < other.year;
    }
} movies_data_type;

typedef struct neighbor_type {
    std::string movie_id, person_id;
    
    bool operator<(const struct neighbor_type& other) const {
        return this->movie_id < other.movie_id and this->person_id < other.person_id;
    }
} neighbor_type;

typedef struct shortest_path_data_type{
    std::string action, state;
    bool operator<(struct shortest_path_data_type& other) {
        return this->action < other.action and this->state < other.state;
    }
} shortest_path_data_type;






// Maps names to a set of corresponding person_ids
std::map<std::string, std::set<std::string> > names;

// Maps person_ids to a dictionary of: name, birth, movies (a set of movie_ids)
std::map<std::string, people_data_type> people;

// Maps movie_ids to a dictionary of: title, year, stars (a set of person_ids)
std::map<std::string, movies_data_type> movies;

// define the IDs
const std::string ID = "id", NAME = "name", BIRTH = "birth", MOVIES = "movies", TITLE = "title", YEAR = "year", STARS = "stars", MOVIE_ID = "movie_id", PERSON_ID = "person_id";

csv_dict_reader dict_reader(std::FILE* open_csv_file);

void load_data(const std::string directory = "large");

std::string person_id_for_name(const std::string name);

std::vector<shortest_path_data_type> shortest_path(const std::string source, const std::string target);

std::set<neighbor_type> neighbors_for_person(const std::string person_id);

std::vector<shortest_path_data_type> trace_to_beginning(node* this_node);

int main(int len, char** args) {

    if (len > 2) {
        std::fprintf(stderr, "Usage: %s [directory]\n", args[0]);
        return 1;
    }
    
    const std::string directory = (len == 2) ? args[1] : "large";

    std::printf("Loading data...\n");
    load_data(directory);
    std::printf("Data loaded.\n");

    char buffer[buffer_size];
    std::printf("Source name: ");
    if (not fgets(buffer, buffer_size, stdin)) {
        std::fprintf(stderr, "Error reading in source.\n");
        return 1;
    }
    
    const std::string source = person_id_for_name(std::string(buffer, std::strlen(buffer) - 1));
    if (source.empty()) {
        std::fprintf(stderr, "No person \"%s\" found.\n", source.c_str());
        return 1;
    }
    std::printf("Target name: ");
    if (not fgets(buffer, buffer_size, stdin)) {
        std::fprintf(stderr, "Error reading the target.\n");
        return 1;
    }
    const std::string target = person_id_for_name(std::string(buffer, std::strlen(buffer) - 1));
    if (target.empty()) {
        std::fprintf(stderr, "No person \"%s\" found.\n", target.c_str());
        return 1;
    }

    std::printf("Now to find the path from \"%s\" to \"%s\"\n", people[source].name.c_str(), people[target].name.c_str());

    std::vector<shortest_path_data_type> path = shortest_path(source, target);

    if (path.empty()) {
        std::printf("%s and %s are not connected.\n", source.c_str(), target.c_str());
    }

    else {

        unsigned long degrees = path.size(), index;
        std::string person1, person2, movie;
        std::printf("%lu degrees of separation.\n", degrees - 1);
        
        for (index = 0; index < path.size() - 1; index++) {
            person1 = people[path[index].state].name;
            person2 = people[path[index + 1].state].name;
            movie = movies[path[index].action].title;
            std::printf("\t%lu: %s and %s starred in %s\n", index + 1, person1.c_str(), person2.c_str(), movie.c_str());
        }

    }

    return 0;
}


csv_dict_reader dict_reader(std::FILE* open_csv_file) {
    csv_dict_reader the_answer;
    std::string line;
    unsigned long start, current, key;
    int c;
    while (std::fread(&c, sizeof(char), 1, open_csv_file) == 1) {
        
        if (same_char( c, '\n')) {
            
            if (the_answer.header.empty()) {
                
                for (start = current = 0; current < line.length() and start < line.length(); current++) {
                    
                    if (same_char((const char) line[current], ',')) {
                        the_answer.header.push_back(line.substr(start, current - start));
                        start = current + 1;
                    }
                    else if (current == line.length() - 1) {
                        the_answer.header.push_back(line.substr(start, current - start + 1));
                        start = current + 1;
                    }
                }
            }
            else {
                std::map<std::string, std::string> new_map;
                key = 0;
                for (start = current = 0; current < line.length(); current++) {
                    if (same_char((const char) line[current], ',')) {
                        
                        same_char(line[start], '\n') ? start++ : start;
                        
                        // trim the edges as necessary
                        if (same_char(line[start], '"')) {
                            new_map.insert(std::make_pair(the_answer.header[key], line.substr(start + 1, current - start - 2)));
                        }
                        else {
                            new_map.insert(std::make_pair(the_answer.header[key], line.substr(start, current - start)));
                        }
                        new_map.insert(std::make_pair(the_answer.header[key], line.substr(start, current - start)));
                        start = current + 1;
                        key++;
                    }
                    else if (current == line.length() - 1) {
                        new_map.insert(std::make_pair(the_answer.header[key], line.substr(start)));
                        start = current + 1;
                        key++;
                    }
                }

                the_answer.data.push_back(new_map);
                // std::printf("final line added was \"%s\"\n", line.c_str());
            }
            line = "";
            continue;
        }
        line = line + ((char) c);
    }
    return the_answer;
}

void load_data(const std::string directory) {
    
    // # Load people
    // with open(f"{directory}/people.csv", encoding="utf-8") as f:
    //     reader = csv.DictReader(f)
    //     for row in reader:
    //         people[row["id"]] = {
    //             "name": row["name"],
    //             "birth": row["birth"],
    //             "movies": set()
    //         }
    //         if row["name"].lower() not in names:
    //             names[row["name"].lower()] = {row["id"]}
    //         else:
    //             names[row["name"].lower()].add(row["id"])


    std::FILE* open_file;
    csv_dict_reader reader;
    
    // Load people
    open_file = std::fopen((directory + sys_slash + "people.csv").c_str(), "r");
    if (not open_file) {
        std::fprintf(stderr, "Failed to open \"%s\"\n", (directory + sys_slash + "people.csv").c_str());
        return;
    }
    
    reader = dict_reader(open_file);
    
    for (std::vector<std::map<std::string, std::string> >::const_iterator row = reader.data.begin(); row != reader.data.end(); row++) {
        std::printf("row->ID is %s\n", row->at(ID).c_str());
        people_data_type new_person = (people_data_type) {row->at(NAME), row->at(BIRTH), std::set<std::string>()};
        people.insert(std::make_pair(row->at(ID), new_person));
        
        if (names.find(new_person.name) == names.end()) {
            // std::printf("names now has %s\n", new_person.name.c_str());
            names.insert(std::make_pair(new_person.name, std::set<std::string>()));
        }
        names[new_person.name].insert(row->at(ID));
    }
    std::fclose(open_file);

    // # Load movies
    // with open(f"{directory}/movies.csv", encoding="utf-8") as f:
    //     reader = csv.DictReader(f)
    //     for row in reader:
    //         movies[row["id"]] = {
    //             "title": row["title"],
    //             "year": row["year"],
    //             "stars": set()
    //         }
    

    // Load movies
    open_file = std::fopen((directory + sys_slash + "movies.csv").c_str(), "r");
    if (not open_file) {
        std::fprintf(stderr, "Failed to open \"%s\"\n", (directory + sys_slash + "movies.csv").c_str());
        return;
    }
    reader = dict_reader(open_file);
    
    for (std::vector<std::map<std::string, std::string> >::const_iterator row = reader.data.begin(); row != reader.data.end(); row++) {
        movies.insert(std::make_pair(row->at(ID), (movies_data_type) {row->at(TITLE), row->at(YEAR), std::set<std::string>()}));
    }
    std::fclose(open_file);


    // # Load stars
    // with open(f"{directory}/stars.csv", encoding="utf-8") as f:
    //     reader = csv.DictReader(f)
    //     for row in reader:
    //         try:
    //             people[row["person_id"]]["movies"].add(row["movie_id"])
    //             movies[row["movie_id"]]["stars"].add(row["person_id"])
    //         except KeyError:
    //             pass


    // Load stars
    open_file = std::fopen((directory + sys_slash + "stars.csv").c_str(), "r");
    if (not open_file) {
        std::fprintf(stderr, "Failed to open \"%s\"\n", (directory + sys_slash + "stars.csv").c_str());
        return;
    }
    reader = dict_reader(open_file);

    for (std::vector<std::map<std::string, std::string> >::const_iterator row = reader.data.begin(); row != reader.data.end(); row++) {
        try {
            people[row->at(PERSON_ID)].movies.insert(row->at(MOVIE_ID));
            movies[row->at(MOVIE_ID)].stars.insert(row->at(PERSON_ID));
        }
        catch (std::exception except) {
            std::fprintf(stderr, "Exception caught: %s\n", except.what());
            continue;
        }
    }
    std::fclose(open_file);
}

std::string person_id_for_name(const std::string name) {

    // Does the set contain name?
    const std::string null = "";
    std::string in_names;
    for (std::map<std::string, std::set<std::string> >::const_iterator this_name = names.begin(); this_name != names.end(); this_name++) {
        // std::printf("comparing \"%s\" and \"%s\"\n", this_name->first.c_str(), name.c_str());
        if (same_string(this_name->first, name)) {
            in_names = this_name->first;
            // std::printf("Found the name in the names map.\n");
            break;
        }
    }

    // The name as it exists in the names map
    if (in_names.empty()) {
        return null;
    }

    if (names[in_names].empty()) {
        return null;
    }

    if (names[in_names].size() > 1) {
        char buffer[buffer_size];
        std::printf("Which \"%s\":\n", name.c_str());
        for (std::set<std::string>::const_iterator this_id = names[in_names].begin(); this_id != names[in_names].end(); this_id++) {
            std::printf("ID : %s, Name : %s, Birth : %s\n", this_id->c_str(), people[*this_id].name.c_str(), people[*this_id].birth.c_str());
        }
        std::printf(": ");
        if (not fgets(buffer, buffer_size, stdin)) {
            return null;
        }
        for (std::set<std::string>::const_iterator this_id = names[in_names].begin(); this_id != names[in_names].end(); this_id++) {
            if (same_string(std::string(buffer), *this_id)) {
                return *this_id;
            }
        }
    }
    else {
        return *names[in_names].begin();
    }
    
    return null;
}

std::vector<shortest_path_data_type> shortest_path(const std::string source, const std::string target) {
    
    queue_frontier frontier;
    std::set<std::string> explored;

    node* this_node, *new_node;
    std::set<neighbor_type> neighbors;

    frontier.add(new node(source, nullptr, ""));

    while (not frontier.empty()) {

        this_node = frontier.remove();

        if (same_string(this_node->state, target)) {
            return trace_to_beginning(this_node);
        }

        neighbors = neighbors_for_person(this_node->state);

        for (std::set<neighbor_type>::const_iterator neighbor = neighbors.begin(); neighbor != neighbors.end(); neighbor++) {

            new_node = new node(neighbor->person_id, this_node, neighbor->movie_id);

            if (same_string(new_node->state, target)) {
                return trace_to_beginning(new_node);
            }

            else if (explored.find(neighbor->person_id) == explored.end() and not frontier.contains_state(neighbor->person_id)) {
                frontier.add(new_node);
            }
            explored.insert(neighbor->person_id);
        }

    }
    
    return std::vector<shortest_path_data_type>();
}

std::set<neighbor_type> neighbors_for_person(const std::string person_id) {
    
    std::set<std::string> movie_ids = people[person_id].movies;
    std::set<neighbor_type> the_answer;

    for (std::set<std::string>::const_iterator movie_id = movie_ids.begin(); movie_id != movie_ids.end(); movie_id++) {
        for (std::set<std::string>::const_iterator this_id = movies[*movie_id].stars.begin(); this_id != movies[*movie_id].stars.end(); this_id++) {
            the_answer.insert((neighbor_type) {*movie_id, *this_id});
        }
    }
    
    return the_answer;
}

std::vector<shortest_path_data_type> trace_to_beginning(node* this_node) {
    std::vector<shortest_path_data_type> the_answer;

    while (this_node->parent != nullptr and not this_node->action.empty()) {
        the_answer.push_back((shortest_path_data_type) {this_node->action, this_node->state});
        this_node = this_node->parent;
    }
    // std::printf("trace_to_beginning vector size is %lu\n", the_answer.size());
    return the_answer;
}