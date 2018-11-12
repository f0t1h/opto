#include <getopt.h>
#include "options.h"
#include <iostream>
#include <memory>
#include <cstring>
Opto::Opto(){
	optionstring = "";
}

Opto::~Opto(){
	for(auto iter = options.begin();iter != options.end();iter++){
		delete[] iter->name;
	}
	options.clear();
	arg_map.clear();
	non_option_args.clear();
}

char *save_str(std::string str){
	char *pt =  new char[str.size()+1];
	strcpy(pt,str.c_str());

	return pt;
}

void Opto::add_option(std::string lngopt, OptionType type, std::string descript){
	arg_descriptions.insert(std::pair<std::string,std::string>(lngopt,descript));
	options.push_back((option){save_str(lngopt),static_cast<int>(type),0,0});
}

void Opto::add_option(std::string lngopt, OptionType type, char shrt, std::string descript){

	arg_descriptions.insert(std::pair<std::string,std::string>(lngopt,descript));
	optionstring+=shrt;

	short2long.insert(std::make_pair(shrt,lngopt));
	if(type==OptionType::RequiredArgument){
		optionstring+=":";
	}
	else if(type==OptionType::OptionalArgument){
		optionstring+="::";
	}
	options.push_back((option){save_str(lngopt),static_cast<int>(type),0,shrt});
}

void Opto::add_flag_option(std::string lngopt, OptionType type, int &flag, char shrt, std::string descript){

	arg_descriptions.insert(std::pair<std::string,std::string>(lngopt,descript));
	optionstring+=shrt;
	short2long.insert(std::make_pair(shrt,lngopt));
	if(type==OptionType::RequiredArgument){
		optionstring+=":";
	}
	else if(type==OptionType::OptionalArgument){
		optionstring+="::";
	}
	options.push_back((option){save_str(lngopt),static_cast<int>(type),&flag,shrt});
}

void Opto::add_flag(std::string lngopt, int *flag){

	for(auto iter = options.begin(); iter != options.end(); iter++){
		if( iter->name == lngopt){
			iter->flag = flag;
			if(!iter->val){
				iter->val = 1;
			}
		}
	}
}

void Opto::set_mandatory(std::string lngopt){
	mandatory_args.push_back(lngopt);
	arg_descriptions[lngopt] = "[Mandatory]\t" + arg_descriptions[lngopt];
}

void Opto::set_default(std::string lngopt, std::string value){
	arg_map[lngopt] = value;
	arg_descriptions[lngopt] = "[Default: " + value + "] " + arg_descriptions[lngopt];
}

void Opto::parse(int argc, char **argv){
	options.push_back((option){0,0,0,0});
	option *long_opts = &options[0];
	unsigned index;
	int opt;

	opt = getopt_long( argc, argv, optionstring.c_str(),long_opts,(int *)&index);	
	while( opt != -1){
		std::string val = optarg==0?"True":std::string(optarg);

		if(index < options.size() - 1){
			//			std::pair<std::string,std::string> pr(long_opts[index].name,val);	
			//			arg_map.insert(pr);
			arg_map[long_opts[index].name] = val;
		}
		else if(opt=='?'){
			std::cerr << "Unknown Argument!"  << std::endl;
			return;
		}
		else if(opt!=0){
			std::string lng = short2long.find(opt)->second;
			for(auto iter = options.begin();std::next(iter)!=options.end();iter++){
				if(iter->name == lng){ *(iter->flag) = opt; break;}
			}
			//			std::pair<std::string,std::string> pr(lng,val);	
			//			arg_map.insert(pr);
			arg_map[lng] = val;
		}

		opt = getopt_long( argc, argv, optionstring.c_str(),long_opts,(int *)&index);	
	}
	for(unsigned i = optind;i <(unsigned) argc;i++){
		non_option_args.push_back(argv[i]);
	}


	options.pop_back();
}

void Opto::validate(){
	bool flag = false;
	for(auto iter = mandatory_args.begin(); iter != mandatory_args.end();iter++){
		if( !query(*iter)){
			std::cerr << "Mandatory argument --" << *iter << " is missing!\n\t" 
				<< "--" << *iter << "\t" << arg_descriptions[*iter] << std::endl;
			flag = true;
		}
	}
	if( flag){
		exit(-1);
	}
}

std::ostream &operator<<(std::ostream &str, Opto const &opto){
	for(auto iter = opto.options.begin(); iter != opto.options.end();iter++){
		std::string name =  (*iter).name;
		char shrt = ' ';
		for(auto i2 = opto.short2long.begin();i2!=opto.short2long.end();i2++){
			if(i2->second == name){
				shrt = i2->first;
			}
		}
		str << "\t--" << name << "\t" << ((shrt==' ')?"":"-") << shrt << "\t" << opto.arg_descriptions.find(name)->second << std::endl;
	}
	return str;
}
bool Opto::query(char str){
	return short2long.find(str) != short2long.end();
}

std::string Opto::fetch(char str){
	std::string lng = short2long.find(str)->second;
	return arg_map.find(lng)->second;
}


bool Opto::query(std::string str){
	return arg_map.find(str) != arg_map.end();
}

std::string Opto::fetch(std::string str){
	return arg_map.find(str)->second;
}


const std::string &Opto::operator[](std::string str)  const {
	return arg_map.find(str)->second;
}
std::string &Opto::operator[](std::string str){
	return arg_map.find(str)->second;
}
