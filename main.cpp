#include "simple_log.h"
#include <iostream>
int main(){
	auto lg = getLogger("consumer");
	lg(INFO)<<"consumer started";
	lg(DEBUG)<<"Degub message from consumer";
	lg(WARNING)<<"Waring from consumer";
	lg(ERROR)<<"Error from consumer";
	std::cout<<"consumer finished\n";
	return 0;
}
