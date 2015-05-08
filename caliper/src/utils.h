#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>

#define EMPTY_SET "#"
#define NOT_DEF_DURATION 0
#define PERIOD_STR "-PERIOD="

#define MAKE_STRING( msg )  ( ((std::ostringstream&)((std::ostringstream() << '\x0') << msg)).str().substr(1) )

typedef struct aging_config_t {
    int duration;
    std::vector<double> alphas;

    aging_config_t() {
        this->duration = NOT_DEF_DURATION;
        this->alphas = std::vector<double>();
    }

    aging_config_t(int duration, std::vector<double> alphas) {
        this->duration = duration;
        this->alphas = alphas;
    }
} aging_config_t;

void generateTree(int max, int min, std::set<std::string>& generated);

void stringTokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);

void loadConfigurations(const std::string& fileName, std::map<std::string, std::vector<aging_config_t> >& configurations, int& init_num_of_cores,
        int& min_num_of_cores);
