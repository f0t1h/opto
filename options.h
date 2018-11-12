#ifndef OPTIONS_HEADER
#define OPTIONS_HEADER

#include <vector>
#include <unordered_map>
#include <unordered_set>



struct option;
class Opto{
private:
	std::string optionstring;
	std::vector<option> options;
	std::unordered_map<char,std::string> short2long;
	std::unordered_map<std::string,std::string> arg_map;
	std::unordered_map<std::string,std::string> arg_descriptions;
	std::vector<std::string> non_option_args;
	std::vector<std::string> mandatory_args;
	friend std::ostream &operator<<(std::ostream &str, Opto const&);
public:
	enum class OptionType{
		NoArgument = 0,
		RequiredArgument = 1,	
		OptionalArgument = 2,
	};
	Opto();
	~Opto();

	void add_option(std::string lngopt, OptionType type, std::string descript);
	void add_option(std::string lngopt, OptionType type, char shrt, std::string descript);

	void add_flag_option(std::string lngopt, OptionType type, int &flag, char shrt, std::string descript);
	void parse(int argc, char **argv);

	void add_flag(std::string lngopt, int *flag);

	void set_default(std::string lngopt, std::string value);
	void set_mandatory(std::string lngopt);
	
	std::string fetch(char opt);
	bool query(char opt);
	std::string fetch(std::string opt);
	bool query(std::string opt);
	std::vector<std::string> get_nonopt();

	void validate();
	std::string &operator[](std::string);
	const std::string &operator[] (std::string) const;

};

std::ostream &operator<<(std::ostream &str, Opto const&);
#endif
