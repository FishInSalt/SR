#pragma once

#include <opencv2/opencv.hpp>
#include <qimage.h>

#include "load_model/SRModel.h"

namespace utils
{

	//根据自定义分辨率的大小，裁剪原图为相应的比例
	QImage clip_image(const QImage &img, int target_height, int target_width);


	//将height和width不相等的图像转换为相等的图片，用白色填充
	QImage padding_image(const QImage &img);
	//双三次插值
	QImage bicubic_interpolation(const QImage &src_img, float scale);
	QImage cvMat2QImage(const cv::Mat& mat);
	cv::Mat QImage2cvMat(QImage image);

	void test1();
	void test2();
	void test3();
	void test4();
	void test5();

	QImage invoke_model( SRModel &sr_model, const QImage &src_img);
	bool update_models();

}


