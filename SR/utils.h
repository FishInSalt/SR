#pragma once

#include <opencv2/opencv.hpp>
#include <qimage.h>

#include "load_model/SRModel.h"

namespace utils
{

	//�����Զ���ֱ��ʵĴ�С���ü�ԭͼΪ��Ӧ�ı���
	QImage clip_image(const QImage &img, int target_height, int target_width);


	//��height��width����ȵ�ͼ��ת��Ϊ��ȵ�ͼƬ���ð�ɫ���
	QImage padding_image(const QImage &img);
	//˫���β�ֵ
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


