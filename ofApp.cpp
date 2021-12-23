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

	//��� Ŭ������ json������ �ִ� ����
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

	// ����
	// 1. �� Ŭ������ ����
	// 2. �� Ŭ������ json ���� ����Ʈ�� �ҷ��� -> �� json������ ��ǥ�� ������ ������
	// 3. �� ��ǥ���� �̿��Ͽ� �̹����� ����� ����
	
	for (auto& draw_class : class_name)	//�� Ŭ������ ����
	{
		std::cout << "\n\n" << folder_index << ". " << draw_class << "\n\n  1) ��ǥ �ҷ�����\n";

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

		for (auto& json : json_files) //�� json ������ ��ǥ�� ������ ����
		{
			ofFile file(json);
			if (!file.exists())	continue;

			file >> js;
			for (auto& stroke : js) //stroke : �׸� ��ġ
			{
				if (stroke.empty())	continue;

				bool is_recog = stroke["recognized"];	//�νĿ��� Ȯ��(true ���� �̹����� ����)
				if (!is_recog)	continue;

				for (auto& xyn : stroke["drawing"])		//xyn : ����ġ
				{
					int xyn_index = 0;
					x_coordi.clear();
					y_coordi.clear();

					for (auto& points : xyn)	//points : x, y, t �� ��ǥ ��ġ��(x ����, y ����, t ������)
					{
						for (auto& p : points)	//p : ��ǥ �ϳ��� ����
						{
							std::string p_str = p.dump(); //json to std::string
							float num = std::stof(p_str); //std::string to float

							if (xyn_index == 0)			x_coordi.push_back(num);
							else if (xyn_index == 1)	y_coordi.push_back(num);
						}
						xyn_index++;
					}

					for (int i = 0; i < x_coordi.size(); i++) //x,y ��ǥ�� �ѽ־� ���� vector�� ����
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

			if (jsonfile_index % 10000 == 1)	std::cout << "     " << jsonfile_index << " ��° �ҷ����� �Ϸ�\n";
			jsonfile_index++;
		}
		std::cout << "     ��ǥ �ҷ����� �Ϸ�!\n\n  2) �̹��� ����\n";


		//vector �ϳ��� ���� ��ǥ�޾Ƽ� �׸� �׸���
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
					//���� ���� ��, ū �� �˾Ƴ��� �̹��� ũ�� ����
					if (max_x < coord.x)	max_x = coord.x;
					if (max_y < coord.y)	max_y = coord.y;
					if (min_x > coord.x)	min_x = coord.x;
					if (min_y > coord.y)	min_y = coord.y;

					//line�� ù��° ���� moveTo�� �̵��� �ϰ� ���� ������ lineTo�� �׸���
					if (line_index == 0)	path.moveTo(coord.x, coord.y);
					else					path.lineTo(coord.x, coord.y);

					line_index++;
				}
				path.draw();
			}

			//�̹��� �ʺ�, ���� ���
			width = abs(max_x - min_x);
			height = abs(max_y - min_y);

			//�̹��� �̼��� ���� �ֱ�
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
			if (final_miny + final_height > 1070)	final_height = 1060 - final_miny;	//�̹��� �ʺ���� �˰� ����

			if (final_width < 0 || final_height < 0 || final_width > 3000000)			
			{
				std::cout << "       ** " << image_index << " ��° drawing�� ��������\n";		//�̷��� �˰��� ���� �� ���� ���
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

			if (image_index % 10000 == 1)	std::cout << "     " << image_index << "��° ���� �Ϸ�\n";
			image_index++;

			path.clear();
			ofBackground(255);
			//ofClear(0, 0, 0, 1.f); // ��� ����
		}
		std::cout << "     �̹��� ���� �Ϸ�!\n";
		vec_images.clear();
		folder_index++;
	}
	OF_EXIT_APP(0); // openFrameworks Ư���� �����Ӻ� �ݺ����� �ʰ� �ٷ� ����
}