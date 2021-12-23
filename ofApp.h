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

	std::vector<int> x_coordi;									// x��ǥ ����
	std::vector<int> y_coordi;									// y��ǥ ����
	std::vector<ofVec2f> vec_line;								// �Ѱ� stroke�� ���� (x, y) ��ǥ ����
	std::vector<std::vector<ofVec2f>> vec_image;				// �Ѱ� image�� ���� stroke ����
	std::vector<std::vector<std::vector<ofVec2f>>> vec_images;	// �� class�� ���� �̹��� ����
};
