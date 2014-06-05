#include <iostream>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include "main.h"

#define PI 3.14159265359

using namespace std;

ParamTable *param_table = new ParamTable();
Challenge *challenge = new Challenge();
Error error;

void warning_message(){
	printf("\nOPTIONS:\n\n-p : print tables\n-o : output to xml\n./main.output : Menu with more options\n\nIt should be passed the two files .json or nothing!\n\nExample:\n./main.output\n./main.output -o ../exemplos/json/param-list.json ../exemplos/json/example.json\n./main.output -p ../exemplos/json/param-list.json ../exemplos/json/example.json\n./main.output -p -o ../exemplos/json/param-list.json ../exemplos/json/example.json\n");
}

int main(int argc, char* argv[]){
	if(argc == 1){
		return menu(argc, argv);
	}else if(argc > 1){
		return commandLineTools(argc,argv);
	}

	return 0;
}

void displayMenu(){
	
	printf("\nMENU:\n\n1 - Read parameters list file (.json)\n2 - Read challenge parameters file (.json)\n3 - Print to XML\n4 - Print URDF\n5 - Add Model to Walls\n6 - Stats\n0 - End Program\n\nOption: ");

}

int menu(int argc, char* argv[]){
		
	int b, flag1 = 0, flag2 = 0;
	string filename;
	const char * c;

	do{
		displayMenu();

		while((cin >> b).fail() || cin.peek() != '\n' || b < 0 || b > 6){
			cin.clear();
			cin.ignore(80, '\n');

			cout << "Introduce a valid option: ";
		}

		switch(b){
			case 1:
				printf("\nIntroduce the name of parameters list file: ");

				cin >> filename;
				cin.sync();
				cout << "\n";
				c = filename.c_str();
				if((param_in = fopen(c, "r")) == NULL){
					printf("[ERROR!] %s must exist or be a .json file!\n", c);
					break;
				}

				try{
					param_parse(c);
					flag1 = 1;
				}catch(int e){
					flag1 = 0;
					ErrorHandling(e);
				}
				
				break;

			case 2:
				if(flag1){
					printf("\nIntroduce the name of challenge parameters file: ");
					cin >> filename;
					c = filename.c_str();
					if((maze_in = fopen(c, "r")) == NULL){
						printf("[ERROR!] %s must exist or be a .json file!\n", c);
						break;
					}

					try{
						maze_parse(c);
						flag2 = 1;
					}catch(int e){
						flag2 = 0;
						ErrorHandling(e);
					}
				}else{
					printf("\nYou have to read a parameters list file before read a challenge parameters file!!\n");
				}

				break;

			case 3:
				if(flag1 && flag2){
					system("rm -rf ./xml");
					system("mkdir ./xml");
          
					ofstream labFile("./xml/Lab.xml");
					ofstream gridFile("./xml/Grid.xml");
					ofstream paramFile("./xml/Param.xml");
    	
					gridOutputXML(gridFile);
					labOutputXML(labFile);
					paramOutputXML(paramFile, param_table);

				}else if(!flag1){
					printf("\nYou have to read a parameters list file to print to XML!!\n");
				}else if(flag1 && !flag2){
					printf("\nYou have to read a challenge parameters file to print to XML!!\n");
				}

				break;

			case 4:
				if(flag1 && flag2){
					system("rm -rf ./urdf");
		    		system("mkdir ./urdf");

		    		ofstream URDFFile("./urdf/URDF.xml");
		    		URDFOutput(URDFFile);
				}else if(!flag1){
					printf("\nYou have to read a parameters list file to print to URDF!!\n");
				}else if(flag1 && !flag2){
					printf("\nYou have to read a challenge parameters file to print to URDF!!\n");
				}

				break;

			case 5:
				if(flag1 && flag2){
					string modelname;
					challenge->maze->printTestModels();
					printf("\nIntroduce the name of the model: ");
					cin >> modelname;
					const char * model = modelname.c_str();
					if(challenge->maze->existsModel(model)){
						double x, y, rad;
						printf("\nIntroduce the x translation: ");
						while((cin >> x).fail() || cin.peek() != '\n'){
							cin.clear();
							cin.ignore(80, '\n');

							cout << "\nIntroduce a valid option: ";
						}
						printf("\nIntroduce the y translation: ");
						while((cin >> y).fail() || cin.peek() != '\n'){
							cin.clear();
							cin.ignore(80, '\n');

							cout << "\nIntroduce a valid option: ";
						}
						printf("\nIntroduce the angle(rad) translation: ");
						while((cin >> rad).fail() || cin.peek() != '\n'){
							cin.clear();
							cin.ignore(80, '\n');

							cout << "\nIntroduce a valid option: ";
						}

						challenge->maze->loadModel(model, x, y, rad);

					}else{
						printf("\n[ERROR!] the %s doesn't exists!\n", model);
					}
				}else if(!flag1){
					printf("\nYou have to read a parameters list file to print to URDF!!\n");
				}else if(flag1 && !flag2){
					printf("\nYou have to read a challenge parameters file to print to URDF!!\n");
				}

				break;

			case 6:
				if(flag1 && flag2){
					printf("\nSTATS:\n\nChallenge Name: %s\n", challenge->getChallengeName());
					printf("Challenge Type: %s\n", challenge->getChallengeType());
					printf("Cycle Time: %d\n", challenge->getCycleTime());
					printf("Duration: %d\n", challenge->getDuration());
					printf("Beacons: %d\n", challenge->maze->countBeacons());
					printf("Target Areas: %d\n", challenge->maze->countTargetAreas());
					printf("Walls: %d\n", challenge->maze->countWalls());
				}else if(!flag1){
					printf("\nYou have to read a parameters list file to complete the stats table!!\n");
				}else if(flag1 && !flag2){
					printf("\nYou have to read a challenge parameters file to complete the stats table!!\n");
				}
				break;

		}
	}while(b != 0);

	return 0;
}

int commandLineTools(int argc, char* argv[]){
	/* options via terminal */
	int print = 0;
	int output = 0;
	int output_set = 0;


	if(argc > 2){
		if(!strcmp("-p", argv[1]) || !strcmp("-p", argv[2])){
			print = 1;
		}
		if(!strcmp("-o", argv[1]) || !strcmp("-o", argv[2])){
			output = 1;
		}
		if(output && (!strcmp("-s", argv[2]) || !strcmp("-s", argv[3]))){
			output_set = 1;
		}
	}

	if(!print && !output){
		warning_message();
		return 0;
	}

	/* name files */
	int arg = 1;
	if(print){
		arg++;
	}
	if(output){
		arg++;
	}
	if(output_set){
		arg++;
	}

	if((param_in = fopen(argv[arg], "r")) == NULL){
		printf("[ERROR!] %d must exist or be a .json file!\n", arg-1);
		return 0;
	}

	try{
		param_parse(argv[arg++]);
	}catch(int e){
		ErrorHandlingWithExit(e);
	}

	if(print){
		param_table->print_symboltable();
	}

	if((maze_in = fopen(argv[arg], "r")) == NULL){
		printf("[ERROR!] %d must exist or be a .json file!\n", arg);
		return 0;
	}

	try{
		maze_parse(argv[arg++]);
	}catch(int e){
		ErrorHandlingWithExit(e);
	}

	if(print){
		challenge->printTest();
		challenge->maze->printTestModels();
	}

	if(output){
		if(output_set){
			ofstream labFile(argv[arg++]);
			ofstream gridFile(argv[arg++]);
			ofstream paramFile(argv[arg++]);
	    	ofstream URDFFile(argv[arg++]);

			gridOutputXML(gridFile);
			labOutputXML(labFile);
			paramOutputXML(paramFile, param_table);
	    	URDFOutput(URDFFile);
		}else{
			system("rm -rf ./xml");
			system("mkdir ./xml");

		    system("rm -rf ./urdf");
		    system("mkdir ./urdf");

			ofstream labFile("./xml/Lab.xml");
			ofstream gridFile("./xml/Grid.xml");
			ofstream paramFile("./xml/Param.xml");
	    	ofstream URDFFile("./urdf/URDF.xml");

			gridOutputXML(gridFile);
			labOutputXML(labFile);
			paramOutputXML(paramFile, param_table);
	    	URDFOutput(URDFFile);
		}
	}

	return 1;
}

/********** PRINTS **********/
void gridOutputXML(ofstream& file){
	challenge->gridOutputXML(file);
}

void labOutputXML(ofstream& file){
	challenge->labOutputXML(file);
}

void paramOutputXML(ofstream& file, ParamTable *param_table){
	challenge->paramOutputXML(file, param_table);
}

void URDFOutput(ofstream& file){
    challenge->URDFOutput(file);
}

/********** ERROR **********/
void ErrorHandlingWithExit(int NUM){
	ErrorHandling(NUM);
	exit(0);
	/* SKYP OR EXIT? */
}

void ErrorHandling(int NUM){
	extern char* param_name;
    extern char* class_name;
    extern parameter param;
    extern char* default_value_type;

    const char* semantic = "\n[SEMANTIC ERROR!] Error description below:\n";

	switch (NUM){
	    case PARAMETER_ALREADY_EXISTS    	 	: printf("%sThe parameter \"%s\" already exists in \"%s\" class.\n",semantic, param_name, class_name); break;
	    case PARAMETER_CLASS_NAME_REQUIRED	 	: printf("%sThe class name is required for the parameter \"%s\".\n",semantic, param_name); break;
		case PARAMETER_VALUE_TYPE_REQUIRED    	: printf("%sThe value type is required for the parameter \"%s\" in \"%s\" class.\n",semantic, param_name, class_name); break;
		case PARAMETER_NAME_REQUIRED 			: printf("%sThe class name is required!\n", semantic); break;
		case DEFAULT_VALUE_WRONG_BY_TYPE		: printf("%sThe default value is wrong by value type in the parameter \"%s\" in \"%s\" class, expecting %s but as given %s.\n",semantic, param_name, class_name, param.value_type, default_value_type);   break;
		case PARSING_ERROR						: printf("\n[PARSING ERROR] Error description bellow:\n%s: %d: %s; conteudo no yytext: '%s'\n", error.fname, error.line, error.s, error.yytext); break;
	    default            				 		: printf("unknown error");
	}
}
