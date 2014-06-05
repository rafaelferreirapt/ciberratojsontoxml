#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include "Maze.h"
#include <cmath>

#define PI 3.14159265359
using namespace std;

void Maze::setName(char* n){
	name = n;
}

void Maze::setDimensions(double w, double h){
	dimensions.width = w;
	dimensions.height = h;
}

Dimensions Maze::getDimensions(){
	return dimensions;
}

void Maze::addBeacon(Point p, double h){
    Beacon tmp;

    tmp.position = p;
    tmp.height = h;

    beacons->addBeacon(tmp);
}

void Maze::addModel(char* nm, double h, Point fp, Point sp, double t){
	Model tmp;

	tmp.height = h;
	tmp.name = nm;
	tmp.first_point = fp;
	tmp.second_point = sp;
	tmp.thickness = t;

    models->addModel(tmp);
}


vector<Beacon> Maze::getBeacons(){
	return beacons->beaconsList();
}

int Maze::countBeacons(){
	return beacons->countBeacons();
}

void Maze::addTargetArea(Point p, double r){
	TargetArea tmp;

	tmp.radius = r;
	tmp.position = p;

	targetareas->addTargetArea(tmp);
}

vector<TargetArea> Maze::getTargetAreas(){
	return targetareas->targetAreasList();
}

int Maze::countTargetAreas(){
	return targetareas->countTargetAreas();
}

void Maze::addWall(double h, double t, vector<Point>* cl){
  	//if(validate_corner_list(cl)){
  		Wall tmp;

		tmp.height = h;
		if(t==0){
			tmp.thickness = 0.1;
		}
		else if(t>0){
			tmp.thickness = t;
		}
		else{
			tmp.thickness = 0.1;
		}

		tmp.corner_list = cl;

		walls->addWall(tmp);
  	//}
}

bool Maze::validate_corner_list(vector<Point>* cl){

	if(cl->size()<2){
		return false;
	}
	if(cl->size()==2){
		return true;
	}

	vector<Point*> *scalar_vector = new vector<Point*>();

	for(vector<Point>::iterator it = cl->begin() ; it != cl->end(); ++it){
		Point *tmp =  new Point((*(it+1)).getX()-(*it).getX(), (*(it+1)).getY()-(*it).getY());
		scalar_vector->push_back(tmp);
	}
	for(vector<Point*>::iterator it = scalar_vector->begin() ; it != scalar_vector->end()-1; ++it){

		double series1[] = {(*it)->getX(), (*it)->getY()};
	  	double series2[] = {(*(it+1))->getX(), (*(it+1))->getY()};
		if(!inner_product(series1, series1+2, series2, 0)){
			return true;
		}
	}
	return false;
}

void Maze::modify_vector(vector<Point>* cl){

}

vector<Wall> Maze::getWalls(){
	return walls->wallsList();
}

int Maze::countWalls(){
	return walls->countWalls();
}

void Maze::addPose(double xx, double yy, double t){
	Pose tmp;

	tmp.x = xx;
	tmp.y = yy;
	tmp.teta = t;

	grid->addPose(tmp);
}

vector<Pose> Maze::getPoses(){
	return grid->getPosesList();
}

void Maze::printTest(){
	cout << "scenario description : \n\t" << "name : " << name << "\n\t" << "dimensions : " << "[" << dimensions.width << ", " << dimensions.height << "],\n\t" << endl;
	beacons->printTest();
	targetareas->printTest();
	walls->printTest();
	grid->printTest();
}

void Maze::printTestModels(){
	models->printTest();
}

void Maze::loadModel(const char* name, double x, double y, double rot_angle){
	Model md = models->getModel(name);

	Wall tmp;

	tmp.height = md.height;
	if(md.thickness==0){
		tmp.thickness = 0.1;
	}else if(md.thickness>0){
		tmp.thickness = md.thickness;
	}else{
		tmp.thickness = 0.1;
	}

	vector<Point> *new_cl = new vector<Point>();
	Point *fp = new Point(md.first_point.getX()+x, md.first_point.getY()+y);
	Point *sp = new Point(md.second_point.getX()+x, md.second_point.getY()+y);

	Point *middle = walls->middle_point(fp, sp);
	double distance = walls->two_points_distance(fp,sp);
	double d2 = distance/2;

	double a = cos(rot_angle)*d2;
	double o = sin(rot_angle)*d2;

	double fp_x = middle->getX()-a;
	double fp_y = middle->getY()-o;

	double sp_x = middle->getX()+a;
	double sp_y = middle->getY()+o;

	fp = new Point(fp_x, fp_y);
	sp = new Point(sp_x, sp_y);

	new_cl->push_back(*fp);
	new_cl->push_back(*sp);

	tmp.corner_list = new_cl;
	walls->addWall(tmp);
	/*
	struct Model{
	    char* name;
	    Point first_point;
	    Point second_point;
	    double thickness;
	    double height;
	};
	*/
}

int Maze::existsModel(const char* name){
	return models->existsModel(name);
}


void Maze::gridOutputXML(ofstream& file){
	grid->gridOutputXML(file);
}

void Maze::labOutputXML(ofstream& file){

	file << "<Lab Name=\"" << name << "\" Width=\"" << dimensions.width << "\" Height=\"" << dimensions.height << "\">\n";
	beacons->labOutputXML(file);
	targetareas->labOutputXML(file);
	walls->labOutputXML(file);
	file << "</Lab>";
	file.close();
}

void Maze::URDFOutput(ofstream& file){
    int count=1;

    file << "<robot name=\"TOS challenge\">\n";

    file << "\t<link name=\""<< "parede" << count++ <<"\">\n\t\t<visual>\n\t\t\t<origin xyz=\"" << dimensions.width/2 << " 0 1\" rpy=\"0 0 0\"/>\n\t\t\t<geometry>\n\t\t\t\t<box size=\"" << dimensions.width << " 0.05 2\"/>\n\t\t\t</geometry>\n\t\t\t<material name=\"Cyan1\">\n\t\t\t\t<color rgba=\"0 0.9 0.9 1.0\"/>\n\t\t\t</material>\n\t\t</visual>\n\t</link>\n";

    file << "\t<link name=\""<< "parede" << count++ <<"\">\n\t\t<visual>\n\t\t\t<origin xyz=\"" << dimensions.width/2 << " "<< dimensions.height <<" 1\" rpy=\"0 0 0\"/>\n\t\t\t<geometry>\n\t\t\t\t<box size=\"" << dimensions.width << " 0.05 2\"/>\n\t\t\t</geometry>\n\t\t\t<material name=\"Cyan1\">\n\t\t\t\t<color rgba=\"0 0.9 0.9 1.0\"/>\n\t\t\t</material>\n\t\t</visual>\n\t</link>\n";

    file << "\t<link name=\""<< "parede" << count++ <<"\">\n\t\t<visual>\n\t\t\t<origin xyz=\"0 " << dimensions.height/2 <<" 1\" rpy=\"0 0 0\"/>\n\t\t\t<geometry>\n\t\t\t\t<box size=\"0.05 "<< dimensions.height << " 2\"/>\n\t\t\t</geometry>\n\t\t\t<material name=\"Cyan1\">\n\t\t\t\t<color rgba=\"0 0.9 0.9 1.0\"/>\n\t\t\t</material>\n\t\t</visual>\n\t</link>\n";

    file << "\t<link name=\""<< "parede" << count++ <<"\">\n\t\t<visual>\n\t\t\t<origin xyz=\""<< dimensions.width << " " << dimensions.height/2 <<" 1\" rpy=\"0 0 0\"/>\n\t\t\t<geometry>\n\t\t\t\t<box size=\"0.05 "<< dimensions.height << " 2\"/>\n\t\t\t</geometry>\n\t\t\t<material name=\"Cyan1\">\n\t\t\t\t<color rgba=\"0 0.9 0.9 1.0\"/>\n\t\t\t</material>\n\t\t</visual>\n\t</link>\n";

    file << "\t<link name=\"base\">\n\t\t<visual>\n\t\t\t<origin xyz=\""<< dimensions.width/2 << " " << dimensions.height/2 <<" 0\" rpy=\"0 0 0\"/>\n\t\t\t<geometry>\n\t\t\t\t<box size=\""<< dimensions.width << " " << dimensions.height << " 0.2\"/>\n\t\t\t</geometry>\n\t\t\t<material name=\"yellow2\">\n\t\t\t\t<color rgba=\"1 1 0.5 1.0\"/>\n\t\t\t</material>\n\t\t</visual>\n\t</link>\n";

    beacons->URDFOutput(file);
    targetareas->URDFOutput(file);
    walls->URDFOutput(file);
    grid->URDFOutput(file);
    file << "</robot>";
    file.close();
}
