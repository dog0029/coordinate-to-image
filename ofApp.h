#pragma once

#include "ofMain.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <filesystem>
#include <iomanip>

class ofApp : public ofBaseApp
{
public:
	void setup();
	//void update();
	void draw();

	ofJson js;
	ofJson stroke;
	ofTrueTypeFont ttf;
	ofPath path;
	std::string text;

	ofImage img;

private:
	std::string json_path;
	std::vector<std::string> class_name;
	std::vector<std::string> json_files;

	std::vector<int> x_coordi;									// x좌표 모음
	std::vector<int> y_coordi;									// y좌표 모음
	std::vector<ofVec2f> vec_line;								// 한개 stroke에 대한 (x, y) 좌표 모음
	std::vector<std::vector<ofVec2f>> vec_image;				// 한개 image에 대한 stroke 모음
	std::vector<std::vector<std::vector<ofVec2f>>> vec_images;	// 한 class에 대한 이미지 모음
};
