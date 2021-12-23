#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetBackgroundColor(255);
	ttf.load("mono.ttf", 8);
	ofColor color(0, 0, 0);
	path.setStrokeColor(color);
	path.setFilled(false);
	path.setStrokeWidth(5);

	//모든 클래스의 json파일이 있는 폴더
	json_path = "D:\\yujin\\image_draw\\json";
	std::vector<std::string> folders_full_path;

	//class folder full path
	std::filesystem::directory_iterator itr(json_path);
	while (itr != std::filesystem::end(itr))
	{
		const std::filesystem::directory_entry& entry = *itr;
		std::string path_list = entry.path().string();
		folders_full_path.push_back(path_list);
		itr++;
	}

	//class names only
	for (auto& i : folders_full_path)
	{
		int pos_filename = 0;
		if ((1 + i.find_last_of("\\")) < i.length()) pos_filename = 1 + i.find_last_of("\\");
		if ((1 + i.find_last_of("/")) < i.length()) pos_filename = std::max(pos_filename, 1 + (int)i.find_last_of("/"));
		std::string const filename = i.substr(pos_filename);
		std::string const filename_without_ext = filename.substr(0, filename.find_last_of("."));
		class_name.push_back(filename_without_ext);
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	int folder_index = 1;

	// 순서
	// 1. 한 클래스씩 꺼냄
	// 2. 한 클래스의 json 파일 리스트를 불러옴 -> 각 json파일의 좌표를 꺼내어 저장함
	// 3. 각 좌표들을 이용하여 이미지를 만들고 저장
	
	for (auto& draw_class : class_name)	//한 클래스씩 꺼냄
	{
		std::cout << "\n\n" << folder_index << ". " << draw_class << "\n\n  1) 좌표 불러오기\n";

		json_files.clear();

		std::stringstream paths;
		paths << json_path << "\\" << draw_class;
		std::string folder_path = paths.str();

		//file full path
		std::filesystem::directory_iterator itr(folder_path);
		while (itr != std::filesystem::end(itr))
		{
			const std::filesystem::directory_entry& entry = *itr;
			std::string path_list = entry.path().string();
			json_files.push_back(path_list);
			itr++;
		}

		int jsonfile_index = 1;

		for (auto& json : json_files) //각 json 파일의 좌표를 꺼내어 저장
		{
			ofFile file(json);
			if (!file.exists())	continue;

			file >> js;
			for (auto& stroke : js) //stroke : 그림 뭉치
			{
				if (stroke.empty())	continue;

				bool is_recog = stroke["recognized"];	//인식여부 확인(true 값만 이미지로 생성)
				if (!is_recog)	continue;

				for (auto& xyn : stroke["drawing"])		//xyn : 선뭉치
				{
					int xyn_index = 0;
					x_coordi.clear();
					y_coordi.clear();

					for (auto& points : xyn)	//points : x, y, t 각 좌표 뭉치들(x 따로, y 따로, t 따로임)
					{
						for (auto& p : points)	//p : 좌표 하나씩 추출
						{
							std::string p_str = p.dump(); //json to std::string
							float num = std::stof(p_str); //std::string to float

							if (xyn_index == 0)			x_coordi.push_back(num);
							else if (xyn_index == 1)	y_coordi.push_back(num);
						}
						xyn_index++;
					}

					for (int i = 0; i < x_coordi.size(); i++) //x,y 좌표를 한쌍씩 묶어 vector에 저장
					{
						ofVec2f xy;
						xy.x = x_coordi.at(i);
						xy.y = y_coordi.at(i);
						vec_line.push_back(xy);

						if (i == x_coordi.size() - 1 && !vec_line.empty())
						{
							vec_image.push_back(vec_line);
							vec_line.clear();
						}
					}
				}

				if (!vec_image.empty())
				{
					vec_images.push_back(vec_image);
					vec_image.clear();
				}
			}

			if (jsonfile_index % 10000 == 1)	std::cout << "     " << jsonfile_index << " 번째 불러오기 완료\n";
			jsonfile_index++;
		}
		std::cout << "     좌표 불러오기 완료!\n\n  2) 이미지 생성\n";


		//vector 하나씩 열고 좌표받아서 그림 그리기
		//vec_images -> vec_image -> vec_line -> ofVec2f

		int image_index = 1;

		for (auto& image : vec_images)
		{
			float width = 0;
			float height = 0;
			float max_x = 0;
			float max_y = 0;
			float min_x = 99999;
			float min_y = 99999;
			float final_minx = 0;
			float final_miny = 0;
			float final_width = 0;
			float final_height = 0;

			for (auto& line : image)
			{
				int line_index = 0;
				for (auto& coord : line)
				{
					//가장 작은 값, 큰 값 알아내서 이미지 크기 추출
					if (max_x < coord.x)	max_x = coord.x;
					if (max_y < coord.y)	max_y = coord.y;
					if (min_x > coord.x)	min_x = coord.x;
					if (min_y > coord.y)	min_y = coord.y;

					//line의 첫번째 값은 moveTo로 이동을 하고 이후 값들은 lineTo로 그리기
					if (line_index == 0)	path.moveTo(coord.x, coord.y);
					else					path.lineTo(coord.x, coord.y);

					line_index++;
				}
				path.draw();
			}

			//이미지 너비, 높이 계산
			width = abs(max_x - min_x);
			height = abs(max_y - min_y);

			//이미지 미세한 여백 넣기
			if (min_x - 10 > 0)
			{
				final_minx = min_x - 10;
				final_width = width + 20;
			}
			else
			{
				final_minx = 0;
				final_width = width;
			}
			if (min_y - 10 > 0)
			{
				final_miny = min_y - 10;
				final_height = height + 20;
			}
			else
			{
				final_miny = 0;
				final_height = height;
			}
			if (final_miny + final_height > 1070)	final_height = 1060 - final_miny;	//이미지 너비높이 똥값 예방

			if (final_width < 0 || final_height < 0 || final_width > 3000000)			
			{
				std::cout << "       ** " << image_index << " 번째 drawing에 문제있음\n";		//이래도 똥값이 있을 시 생성 취소
			}
			else
			{
				std::string category = draw_class;
				std::stringstream ss;
				ss << "image/" << category << "/" << category << "_"
					<< std::setw(6) << std::setfill('0') << image_index << ".png";
				std::string out = ss.str();

				img.grabScreen(final_minx, final_miny, final_width, final_height);
				img.saveImage(out.c_str());
			}

			if (image_index % 10000 == 1)	std::cout << "     " << image_index << "번째 생성 완료\n";
			image_index++;

			path.clear();
			ofBackground(255);
			//ofClear(0, 0, 0, 1.f); // 배경 제거
		}
		std::cout << "     이미지 생성 완료!\n";
		vec_images.clear();
		folder_index++;
	}
	OF_EXIT_APP(0); // openFrameworks 특성상 프레임별 반복하지 않고 바로 종료
}