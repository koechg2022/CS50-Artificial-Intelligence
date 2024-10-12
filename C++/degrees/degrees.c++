

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
const std::string ID = "id", NAME = "name", BIRTH = "birth", MOVIES = "movies", TITLE = "title", YEAR = "year", STARS = "stars", MOVIE_ID = "movie_id", PERSON_ID = "person_id";

csv_dict_reader dict_reader(std::FILE* open_csv_file);

void load_data(const std::string directory = "large");

int main(int len, char** args) {

    if (len > 2) {
        std::fprintf(stderr, "Usage: %s [directory]\n", args[0]);
        return 1;
    }
    
    const std::string directory = (len == 2) ? args[1] : "large";
    unsigned long index;
    
    std::printf("------------------------------------------------------Names------------------------------------------------------\n");
    for (std::map<std::string, std::set<std::string> >::const_iterator name = names.begin(); name != names.end(); name++, index++) {
        index = 0;
        std::printf("%s:\n", name->first.c_str());
        for (std::set<std::string>::const_iterator star = name->second.begin(); star != name->second.end(); star++, index++) {
            std::printf("%s%s", star->c_str(), (index + 1 == name->second.size()) ? ", " : "\n");
        }
    }
    
    std::printf("-----------------------------------------------------People------------------------------------------------------\n");
    for (std::map<std::string, people_val>::const_iterator person = people.begin(); person != people.end(); person++) {
        index = 0;
        std::printf("%s:\n", person->first.c_str());
        std::printf("\t%s\n\t%s\n\t\t", person->second.name.c_str(), person->second.birth.c_str());
        for (std::set<std::string>::const_iterator movie = person->second.movies.begin(); movie != person->second.movies.end(); movie++, index++) {
            std::printf("%s%s", movie->c_str(), (index + 1 == person->second.movies.size()) ? ", " : "\n");
        }
    }
    
    std::printf("-----------------------------------------------------Movies------------------------------------------------------\n");
    for (std::map<std::string, movie_val>::const_iterator movie = movies.begin(); movie != movies.end(); movie++) {
        index = 0;
        std::printf("%s:\n", movie->first.c_str());
        std::printf("\t%s\n\t%s\n\t\t", movie->second.title.c_str(), movie->second.year.c_str());
        for (std::set<std::string>::const_iterator star = movie->second.stars.begin(); star != movie->second.stars.end(); star++, index++) {
            std::printf("%s%s", (index + 1 == movie->second.stars.size()) ? "\n" : ", ", star->c_str());
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

// TODO : Implement me
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
    csv_dict_reader file_data;

    // Load people
    open_file = std::fopen((directory + sys_slash + "people.csv").c_str(), "r");
    file_data = dict_reader(open_file);
    for (std::vector<std::map<std::string, std::string> >::const_iterator row = file_data.data.begin(); row != file_data.data.end(); row++) {
        people.insert(std::make_pair(row->at(ID), (people_val) {row->at(NAME), row->at(BIRTH), std::set<std::string>()}));

        if (names.find(row->at(NAME)) == names.end()) {
            std::set<std::string>_;
            names.insert(std::make_pair(row->at(NAME), _));
        }
        names[row->at(NAME)].insert(row->at(ID));
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
    file_data = dict_reader(open_file);
    for (std::vector<std::map<std::string, std::string> >::const_iterator row = file_data.data.begin(); row != file_data.data.end(); row++) {
        movies.insert(std::make_pair(row->at(ID), (movie_val) {row->at(TITLE), row->at(YEAR), std::set<std::string>()}));
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
    file_data = dict_reader(open_file);
    for (std::vector<std::map<std::string, std::string> >::const_iterator row = file_data.data.begin(); row != file_data.data.end(); row++) {
        try {
            people[row->at(PERSON_ID)].movies.insert(row->at(MOVIE_ID));
            movies[row->at(MOVIE_ID)].stars.insert(row->at(PERSON_ID));
        }

        catch (std::exception e) {
            std::fprintf(stderr, "Exception caught. Error message: %s\n", e.what());
        }
    }
    std::fclose(open_file);
}

