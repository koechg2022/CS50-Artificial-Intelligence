


#include <fstream>
#include <string>
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

csv_dict_reader dict_reader(const std::string file_name, const std::string directory);

void load_data(const std::string directory = "large");

int main(int len, char** args) {

    if (len > 2) {
        std::fprintf(stderr, "Usage: %s [directory]\n", args[0]);
        return 1;
    }
    
    // const std::string directory = (len == 2) ? args[1] : "large";
    load_data((len == 2) ? args[1] : "large");
    unsigned long index;
    
    std::printf("\n------------------------------------------------------Names------------------------------------------------------\n");
    for (std::map<std::string, std::set<std::string> >::const_iterator name = names.begin(); name != names.end(); name++, index++) {
        index = 0;
        std::printf("%s:\t", name->first.c_str());
        for (std::set<std::string>::const_iterator star = name->second.begin(); star != name->second.end(); star++, index++) {
            std::printf("%s%s", star->c_str(), (index + 1 == name->second.size()) ? "\n" : ", ");
        }
    }
    
    std::printf("\n-----------------------------------------------------People------------------------------------------------------\n");
    for (std::map<std::string, people_val>::const_iterator person = people.begin(); person != people.end(); person++) {
        index = 0;
        std::printf("%s:\n", person->first.c_str());
        std::printf("\tName: %s\n\tBirth: %s\n\tMovies: ", person->second.name.c_str(), person->second.birth.c_str());
        for (std::set<std::string>::const_iterator movie = person->second.movies.begin(); movie != person->second.movies.end(); movie++, index++) {
            std::printf("%s%s", movie->c_str(), (index + 1 == person->second.movies.size()) ? "\n" : ", ");
        }
        if (person->second.movies.empty()) {
            std::printf("No movies for this actor.\n");
        }
    }
    
    std::printf("\n-----------------------------------------------------Movies------------------------------------------------------\n");
    for (std::map<std::string, movie_val>::const_iterator movie = movies.begin(); movie != movies.end(); movie++) {
        index = 0;
        std::printf("%s:\n", movie->first.c_str());
        std::printf("\t%s\n\t%s\n\tActors: ", movie->second.title.c_str(), movie->second.year.c_str());
        for (std::set<std::string>::const_iterator star = movie->second.stars.begin(); star != movie->second.stars.end(); star++, index++) {
            std::printf("%s%s", star->c_str(), (index + 1 == movie->second.stars.size()) ? "\n" : ", ");
        }
        if (movie->second.stars.empty()) {
            std::printf("No movies listed.\n");
        }
    }

    return 0;
}

csv_dict_reader dict_reader(const std::string file_name, const std::string directory) {
    
    csv_dict_reader the_answer;
    std::fstream open_file(directory + sys_slash + file_name, std::ios_base::in);
    
    if (not open_file.is_open()) {
        std::fprintf(stderr, "Failed to open file.\n");
        return the_answer;
    }

    // Get the headers
    std::string line, value;
    unsigned long start, current;
    std::getline(open_file, line);

    if (not line.empty()) {
        
    }

    
    return the_answer;
}

// TODO : Implement me
void load_data(const std::string directory) {
    
    csv_dict_reader reader;

    
}

