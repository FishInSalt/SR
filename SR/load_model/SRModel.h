#pragma once

#include <string>

#undef slots
#include <torch/script.h> // One-stop header.
#define slots Q_SLOTS

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include<opencv2/photo.hpp>

void test_CARN(std::string img_path, std::string model_path, int scale);

enum INPUT_FORMAT {
	RGB,
	YCBCR
};

class SRModel {
public:

	SRModel() = default;
	SRModel(std::string model_path, INPUT_FORMAT input_format = INPUT_FORMAT::RGB);

	cv::Mat super_resolution(const cv::Mat &src_img);

	bool  set_model_path(const std::string &model_path);

	std::string get_model_path() { return model_path_; }
	int get_scale() { return scale_;  }

	INPUT_FORMAT get_input_format_() { return input_format_; }
	std::string get_src_img_path() { return src_img_path_; }


private:

	torch::jit::script::Module module_;
	std::string model_path_;
	std::string src_img_path_;

	
	int scale_;
	INPUT_FORMAT input_format_;

private:
	bool  load_model(const std::string &model_path);
	at::Tensor preprocess(const cv::Mat &src_img);
	cv::Mat postprocess(at::Tensor &res);

	at::Tensor call_model(at::Tensor& input_tensor);
	

	

	


};