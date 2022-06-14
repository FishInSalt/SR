#include <fstream>
#include <iostream>
#include <memory>
#include<string>
#include <vector>

#include "SRModel.h"

using namespace std;

string g_img_path = "E:/Program Files (x86)/Waifu2x-Extension-GUI-v3.84.01-Win64/data/LR images/0.png";
string g_model_path = "./model_carn_libtorch_cpu_x2.pt";
int scale = 2;


auto read_image_as_python(string img_path) {
	auto img = cv::imread(img_path, -1);
	cout << "img channels " << img.channels() << endl;
	cout << "image size " << img.size() << endl;

	cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
	img.convertTo(img, CV_32F);

	return img;
}
auto RGB_32F_2_BGR_8U(cv::Mat rgb_32f_img) {
	rgb_32f_img.convertTo(rgb_32f_img, CV_8U);
	// cv::Mat bgr_8u_img;
	cv::cvtColor(rgb_32f_img, rgb_32f_img, cv::COLOR_RGB2BGR);

	return rgb_32f_img;

}

auto rgb2ycbcr(cv::Mat rgb_img) {
	cv::Mat ycrcb_img;

	// cout<<" rgb_img: "<<endl;
	// cout<<cv::format(rgb_img,cv::Formatter::FMT_NUMPY)<<endl;
	ycrcb_img = cv::Mat(rgb_img.rows, rgb_img.cols, CV_32FC3);
	// cv::cvtColor(rgb_img, ycrcb_img,cv::COLOR_RGB2YCrCb);
	auto y = cv::Mat(rgb_img.rows, rgb_img.cols, CV_32FC1);
	auto cr = cv::Mat(rgb_img.rows, rgb_img.cols, CV_32FC1);
	auto cb = cv::Mat(rgb_img.rows, rgb_img.cols, CV_32FC1);


	auto r = cv::Mat(rgb_img.rows, rgb_img.cols, CV_32FC1);
	auto g = cv::Mat(rgb_img.rows, rgb_img.cols, CV_32FC1);
	auto b = cv::Mat(rgb_img.rows, rgb_img.cols, CV_32FC1);

	vector<cv::Mat> rgb_channels{ r,g,b };

	// vector<cv::Mat> ycrcb = {y,cr,cb}; 
	cv::split(rgb_img, rgb_channels);
	// cout<<cv::format(r,cv::Formatter::FMT_NUMPY)<<endl;
	//  cout<<cv::format(rgb_channels[1],cv::Formatter::FMT_NUMPY)<<endl;
	//  cout<<cv::format(rgb_channels[2],cv::Formatter::FMT_NUMPY)<<endl;
	y = 16.0f + (r * 64.738f + 129.057f * g + 25.064f * b) / 256.0f;
	cb = 128.0f + (-37.945f * r - 74.494f * g + 112.439f * b) / 256.0f;
	cr = 128.0f + (112.439f * r - 94.154f*g - 18.285f * b) / 256.0f;

	vector<cv::Mat> ycrcb_channels({ y,cb,cr });
	cv::merge(ycrcb_channels, ycrcb_img);
	// cout<<cv::format(y,cv::Formatter::FMT_NUMPY)<<endl;


	return ycrcb_img;
}

auto ycbcr2rgb(cv::Mat ycbcr_img) {
	cv::Mat rgb_img = cv::Mat(ycbcr_img.rows, ycbcr_img.cols, CV_32FC3);
	cv::Mat r = cv::Mat(ycbcr_img.rows, ycbcr_img.cols, CV_32FC1);
	cv::Mat g = cv::Mat(ycbcr_img.rows, ycbcr_img.cols, CV_32FC1);
	cv::Mat b = cv::Mat(ycbcr_img.rows, ycbcr_img.cols, CV_32FC1);

	cv::Mat ycbcr_channels[3];
	cv::split(ycbcr_img, ycbcr_channels);

	r = 298.082f * ycbcr_channels[0] / 256.0f + 408.583f * ycbcr_channels[2] / 256.0f - 222.921f;
	g = 298.082f * ycbcr_channels[0] / 256.0f - 100.291f * ycbcr_channels[1] / 256.0f - 208.120f * ycbcr_channels[2] / 256.0f + 135.576f;
	b = 298.082f * ycbcr_channels[0] / 256.0f + 516.412f * ycbcr_channels[1] / 256.0f - 276.836f;

	cv::merge(vector<cv::Mat>({ r,g,b }), rgb_img);

	return rgb_img;


}

auto get_ycbcr_channels(cv::Mat rbg_img) {

	cv::Mat ycbcr_img;
	ycbcr_img = rgb2ycbcr(rbg_img);

	// vector<cv::Mat> ycbcr_channels(3);
	// cv::split(ycbcr,ycbcr_channels);

	// cout<<"y channels"<<endl<<cv::format(ycbcr_channels[0],cv::Formatter::FMT_NUMPY)<<endl;
	vector<cv::Mat> ycbcr_channels(3);
	cv::split(ycbcr_img, ycbcr_channels);

	// auto rgb_img = ycbcr2rgb(ycbcr_img);
	// rgb_img.convertTo(rgb_img,CV_8U);
	// cout<<"rgb_img" <<endl<<cv::format(rgb_img,cv::Formatter::FMT_NUMPY)<<endl;





	return ycbcr_channels;

}

//获得tensor的结果，并且把ycbcr的结果存到ycbcr_channels中，后续使用
auto img2input(cv::Mat img)
{
	int img_width = img.cols;
	int img_height = img.rows;
	int dst_width = img_width * 2;
	int dst_height = img_height * 2;

	// cv::Mat bicubic_img;
	// cv::resize(img, bicubic_img, cv::Size(dst_width, dst_height), 0, 0, cv::INTER_CUBIC);

	auto ycbcr_channels = get_ycbcr_channels(img);

	auto y = ycbcr_channels[0];
	y /= 255.0f;


	auto tensor_y = torch::from_blob(y.data, { 1,y.rows,y.cols,y.channels() }).clone();
	tensor_y = tensor_y.permute({ 0,3,1,2 });
	// print(tensor_y);

	return tensor_y;



}

void img_test(string img_path) {
	auto img = read_image_as_python(img_path);
	get_ycbcr_channels(img);
}


auto load_model(string model_path) {
	fstream model_file;
	cout << "the path of model is " << model_path << endl;
	model_file.open(model_path, ios::in);
	if (!model_file)
	{
		cout << "the path is not valid!" << endl;
	}
	else
	{
		cout << "the file does exist !" << endl;
	}
	model_file.close();

	torch::jit::script::Module module;
	try
	{
		// Deserialize the ScriptModule from a file using torch::jit::load().
		module = torch::jit::load(model_path);
	}
	catch (const c10::Error &e)
	{
		std::cerr << "error loading the model\n";
		exit(-1);
	}

	std::cout << "ok\n";
	return module;

}


void test_FSRCNN(string img_path, string model_path) {

	auto module = load_model(model_path);

	// 开始测试模型

	auto img = read_image_as_python(img_path);

	cv::Mat bicubic_img;
	cv::resize(img, bicubic_img, cv::Size(img.rows * 3, img.cols * 3), 0, 0, cv::INTER_CUBIC);

	auto bicubic_ycrcb_channels = get_ycbcr_channels(bicubic_img);




	auto input = img2input(img);

	//  cout<<input<<endl;

	vector<torch::jit::IValue> inputs;
	inputs.push_back(input);

	module.eval();

	auto preds = module.forward(inputs).toTensor();

	preds = preds.squeeze(0).detach();
	// preds = preds.permute({1,2,0});
	// cout<<preds.sizes()<<endl;
	preds = preds.mul(255.0f).clamp(0.0, 255.0);
	// cout<<preds<<endl;
	cv::Mat preds_y(cv::Size(img.rows * 3, img.cols * 3), CV_32F, preds.data_ptr());

	// cout<< preds_y.size();
	// cout<<"preds_y" <<endl<<cv::format(preds_y,cv::Formatter::FMT_NUMPY)<<endl;

	cv::Mat ycbcr_img;
	cv::merge(vector<cv::Mat>{preds_y, bicubic_ycrcb_channels[1], bicubic_ycrcb_channels[2]}, ycbcr_img);

	auto sr_img = ycbcr2rgb(ycbcr_img);

	// cout<<"img"<<endl<<cv::format(img,cv::Formatter::FMT_NUMPY)<<endl;

	img = RGB_32F_2_BGR_8U(img);
	bicubic_img = RGB_32F_2_BGR_8U(bicubic_img);
	sr_img = RGB_32F_2_BGR_8U(sr_img);
	// cv::imshow("lr",img);
	// cv::imshow("sr",sr_img);
	// cv::imshow("bicubic", bicubic_img);
	//  cv::waitKey(0);
	string pre_path = g_img_path.substr(0, g_img_path.find_last_of("."));
	string postfix = g_img_path.substr(g_img_path.find_last_of("."));

	string sr_img_path = pre_path + "_sr_x" + to_string(scale) + postfix;
	string bicubic_img_path = pre_path + "_bicubic_x" + to_string(scale) + postfix;

	cv::imwrite(sr_img_path, sr_img);
	cv::imwrite(bicubic_img_path, bicubic_img);

	cout << "pre_path" << pre_path << endl;
}

void test_CARN(string img_path, string model_path, int scale) {

	auto module = load_model(model_path);

	module.eval();
	auto img = read_image_as_python(img_path);

	cv::Mat bicubic_img;
	cv::resize(img, bicubic_img, cv::Size(img.rows * scale, img.cols * scale), 0, 0, cv::INTER_CUBIC);


	auto input_tensor = torch::from_blob(img.data, { 1,img.rows,img.cols,img.channels() }).clone();
	input_tensor = input_tensor.permute({ 0,3,1,2 });
	input_tensor = input_tensor.mul(1.0f / 255.0f);
	// torch::print(input_tensor);

	// torch::print(input_tensor);
	vector<torch::jit::IValue> inputs;
	inputs.push_back(input_tensor);
	// inputs.push_back(2);

	auto preds = module.forward(inputs).toTensor();

	preds = preds.squeeze(0).detach();
	preds = preds.mul(255.0f).clamp(0.0, 255.0);


	preds = preds.permute({ 1,2,0 });
	// torch::print(preds);
	//貌似只能通过单通道来合并合成RGB图片
	cv::Mat preds_y(cv::Size(img.rows *scale, img.cols *scale), CV_32F, preds.index({ "...",0 }).data_ptr());
	cv::Mat preds_g(cv::Size(img.rows *scale, img.cols *scale), CV_32F, preds.index({ "...",1 }).data_ptr());
	cv::Mat preds_b(cv::Size(img.rows *scale, img.cols *scale), CV_32F, preds.index({ "...",2 }).data_ptr());
	cv::Mat rgb32_img;
	cv::merge(vector<cv::Mat>({ preds_y,preds_g,preds_b }), rgb32_img);
	auto sr_img = RGB_32F_2_BGR_8U(rgb32_img);


	// preds = preds.to(torch::kU8);
	// cv::Mat sr_img(img.rows*scale,img.cols* scale,CV_8UC3);
	// cout<<endl<<"sr_img size:"<<sr_img.size()<<"  sr_img channels: " <<sr_img.channels()<<endl;
	// std::memcpy((void *) sr_img.data,preds.data_ptr(),sizeof(torch::kU8) * preds.numel());

	bicubic_img = RGB_32F_2_BGR_8U(bicubic_img);

	string pre_path = img_path.substr(0, img_path.find_last_of("."));
	string postfix = img_path.substr(img_path.find_last_of("."));

	string sr_img_path = pre_path + "_carn_sr_x" + to_string(scale)  + postfix;
	string bicubic_img_path = pre_path + "_bicubic_x" + to_string(scale) + postfix;

	cv::imwrite(sr_img_path, sr_img);
	cv::imwrite(bicubic_img_path, bicubic_img);





}


cv::Mat y_of_ycbcr(const cv::Mat &ycbcr_img) {

	vector<cv::Mat> ycbcr_channels(3);
	cv::split(ycbcr_img, ycbcr_channels);

	auto y = ycbcr_channels[0];
	return y;
}

SRModel::SRModel(string model_path, INPUT_FORMAT format):
model_path_(model_path),input_format_(format)
{
	bool res = load_model(model_path_);
	if (res == false) {
		cout << "load model failed"<<endl;
	}
}

bool SRModel::load_model(const std::string &model_path) {


	try
	{
		// Deserialize the ScriptModule from a file using torch::jit::load().
		module_ = torch::jit::load(model_path);
	}
	catch (const c10::Error &e)
	{
		std::cerr << "error loading the model\n";
		return false;
	}
	return true;
}

bool SRModel::set_model_path(const string &model_path) {
	if (load_model(model_path)) {
		model_path_ = model_path;
		return true;
	}

	return false;
}

at::Tensor SRModel::preprocess(const cv::Mat &src_img) {
	at::Tensor input_tensor;
	switch (input_format_)
	{
	case INPUT_FORMAT::RGB:
	{
		input_tensor = torch::from_blob(src_img.data, { 1,src_img.rows,src_img.cols,src_img.channels() }).clone();
		input_tensor = input_tensor.permute({ 0,3,1,2 });
		input_tensor = input_tensor.mul(1.0f / 255.0f);
		break;
	}
	case INPUT_FORMAT::YCBCR:
	{
		auto ycbcr_img = rgb2ycbcr(src_img);
		auto y = y_of_ycbcr(ycbcr_img);
		y /= 255.0f;
		auto tensor_y = torch::from_blob(y.data, { 1,y.rows,y.cols,y.channels() }).clone();
		input_tensor = tensor_y.permute({ 0,3,1,2 });
		break;
	}

	default:
		break;
	}

	return input_tensor;
}

cv::Mat SRModel::postprocess( at::Tensor& preds) {
	cv::Mat output_img;


	switch (input_format_)
	{
	case INPUT_FORMAT::YCBCR:
	{
		preds = preds.squeeze(0).detach();
		preds = preds.mul(255.0f).clamp(0.0, 255.0);

		auto preds_size = preds.sizes();
		cv::Mat preds_y(cv::Size(preds_size[0], preds_size[1]), CV_32F, preds.data_ptr());

		// cout<< preds_y.size();
		// cout<<"preds_y" <<endl<<cv::format(preds_y,cv::Formatter::FMT_NUMPY)<<endl;
		auto src_img = read_image_as_python(src_img_path_);
		
		cv::Mat bicubic_img;
		cv::resize(src_img, bicubic_img, cv::Size(preds_size[0], preds_size[1]), 0, 0, cv::INTER_CUBIC);

		auto bicubic_ycrcb_channels = get_ycbcr_channels(bicubic_img);

		cv::Mat ycbcr_img;
		cv::merge(vector<cv::Mat>{preds_y, bicubic_ycrcb_channels[1], bicubic_ycrcb_channels[2]}, ycbcr_img);

		output_img = ycbcr2rgb(ycbcr_img);
		break;
	}
	case INPUT_FORMAT::RGB:
	{
		preds = preds.squeeze(0).detach();
		preds = preds.mul(255.0f).clamp(0.0, 255.0);


		preds = preds.permute({ 1,2,0 });
		// torch::print(preds);
		//貌似只能通过单通道来合并合成RGB图片
		auto preds_size = preds.sizes();
		cv::Mat preds_r(cv::Size(preds_size[0], preds_size[1]), CV_32F, preds.index({ "...",0 }).data_ptr());
		cv::Mat preds_g(cv::Size(preds_size[0], preds_size[1]), CV_32F, preds.index({ "...",1 }).data_ptr());
		cv::Mat preds_b(cv::Size(preds_size[0], preds_size[1]), CV_32F, preds.index({ "...",2 }).data_ptr());
		cv::Mat rgb32_img;
		cv::merge(vector<cv::Mat>({ preds_r,preds_g,preds_b }), rgb32_img);
		output_img = RGB_32F_2_BGR_8U(rgb32_img);
		break;
	}

	default:
		break;
	}

	return output_img;
}

at::Tensor SRModel::call_model(at::Tensor &input_tensor) {
	vector<torch::jit::IValue> inputs;
	inputs.push_back(input_tensor);


	auto s = input_tensor.sizes();


	auto preds = module_.forward(inputs).toTensor();
	
	return preds;
}


//  src_img 必须是RGB格式的，opencv默认的channels是BGR，需要在之前转化下channels
cv::Mat SRModel::super_resolution(const cv::Mat &src_img) {


	auto input = preprocess(src_img);

	auto preds = call_model(input);

	auto output_img = postprocess(preds);

	return output_img;

}

