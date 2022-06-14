#include "utils.h"

#include <qimage.h>
#include <qcoreapplication.h>
#include <algorithm>
#include <qprocess.h>
#include <qdir.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include<opencv2/photo.hpp>

#include <qdebug.h>
#include <qimage.h>
#include <qlabel.h>



static char test_img_path[] = "E:/learning/SR/SR/demo_image/lena.png";

namespace utils {




	//根据自定义分辨率的大小，裁剪原图为相应的比例
	QImage clip_image(const QImage &img, int target_height, int target_width) {
		QImage target_image;


		float ratio = float(target_height) / float(target_width);

		int img_h = img.height();
		int img_w = img.width();
		int start = 0;

		//剪切原图
		if ((img_w * ratio) < img_h) {
			int clip_h = img_w * ratio;
			start = (img_h - clip_h) / 2;
			target_image = img.copy(0, start, img_w, clip_h);
		}
		else {
			int clip_w = img_h / ratio;
			start = (img_w - clip_w) / 2;

			target_image = img.copy(start, 0, clip_w, img_h);
		}



		return target_image;
	}


	//将height和width不相等的图像转换为相等的图片，用白色填充
	QImage padding_image(const QImage &img) {
		if (img.width() == img.height())
			return QImage(img);

		int mx = std::max(img.width(), img.height());
		QImage res(mx, mx, QImage::Format_RGB888);
		res.fill(Qt::white);
		if (img.width() > img.height()) {
			int start = (mx - img.height()) / 2;

			for (int i = 0; i < img.width(); ++i) {
				for (int j = start; j < img.height() + start; ++j) {
					res.setPixel(i, j, img.pixel(i, j - start));
				}
			}
		}
		else {
			int start = (mx - img.width()) / 2;
			for (int i = start; i < img.width() + start; ++i) {
				for (int j = 0; j < img.height(); ++j) {
					res.setPixel(i, j, img.pixel(i - start, j));
				}
			}
		}

		return res;
	}

	QImage bicubic_interpolation(const QImage &src_img,float scale) {
		cv::Mat src_mat = QImage2cvMat(src_img);
		cv::Mat bicubic_mat;
		cv::resize(src_mat, bicubic_mat, cv::Size(src_mat.cols * scale,src_mat.rows *scale ), 0, 0, cv::INTER_CUBIC);
		
		return cvMat2QImage(bicubic_mat);
	}

	QImage cvMat2QImage(const cv::Mat& mat)
	{
		// 8-bits unsigned, NO. OF CHANNELS = 1
		if (mat.type() == CV_8UC1)
		{
			QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
			// Set the color table (used to translate colour indexes to qRgb values)
			image.setColorCount(256);
			for (int i = 0; i < 256; i++)
			{
				image.setColor(i, qRgb(i, i, i));
			}
			// Copy input Mat
			uchar *pSrc = mat.data;
			for (int row = 0; row < mat.rows; row++)
			{
				uchar *pDest = image.scanLine(row);
				memcpy(pDest, pSrc, mat.cols);
				pSrc += mat.step;
			}
			return image;
		}
		// 8-bits unsigned, NO. OF CHANNELS = 3
		else if (mat.type() == CV_8UC3)
		{
			// Copy input Mat
			const uchar *pSrc = (const uchar*)mat.data;
			// Create QImage with same dimensions as input Mat
			QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
			return image.rgbSwapped();
			//return image;
		}
		else if (mat.type() == CV_8UC4)
		{
			qDebug() << "CV_8UC4";
			// Copy input Mat
			const uchar *pSrc = (const uchar*)mat.data;
			// Create QImage with same dimensions as input Mat
			QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
			return image.copy();
		}
		else
		{
			qDebug() << "ERROR: Mat could not be converted to QImage.";
			return QImage();
		}
	}


	cv::Mat QImage2cvMat(QImage image)
	{
		cv::Mat mat;
		qDebug() << image.format();
		switch (image.format())
		{
		case QImage::Format_ARGB32:
		case QImage::Format_RGB32:
		case QImage::Format_ARGB32_Premultiplied:
			mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
			mat = mat.clone();
			break;
		case QImage::Format_RGB888:
			mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
			mat = mat.clone();
			cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
			break;
		case QImage::Format_Indexed8:
		case QImage::Format_Grayscale8:
			mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
			mat = mat.clone();
			break;
		}
		return mat;
	}



	QImage invoke_model( SRModel &sr_model, const QImage &src_img) {

		//SRModel sr_model(model_path.toStdString(),INPUT_FORMAT::RGB);

		//src_img

		//需要测试Qimage转成cvMat后，是否是RGB方式排列,不是的话需要转化一下
		auto cvMat_img = QImage2cvMat(src_img);
		cv::cvtColor(cvMat_img, cvMat_img, cv::COLOR_BGR2RGB);

		//cv::imshow("cvMat_img",cvMat_img);
		//cv::waitKey(10000);

		auto size = cvMat_img.size;

		//std::cout << "cv_Mat_img" << std::endl << cv::format(cvMat_img, cv::Formatter::FMT_NUMPY);
		auto channels = cvMat_img.channels();
		//auto size1 = size[0];
		//auto size2 = size[1];
		//auto size3 = size[2];

		cvMat_img.convertTo(cvMat_img, CV_32F);
		auto res = sr_model.super_resolution(cvMat_img);



		return cvMat2QImage(res);


	}


	void test1()
	{
		cv::Mat mat = cv::imread(test_img_path, cv::IMREAD_UNCHANGED);
		cv::cvtColor(mat, mat, cv::COLOR_BGR2BGRA);
		QImage image = cvMat2QImage(mat);
		qDebug() << (mat.type() == CV_8UC4);
		cv::namedWindow("cvMat2QImage RGB32", cv::WINDOW_AUTOSIZE);
		imshow("cvMat2QImage RGB32", mat);
		QLabel label;
		label.setPixmap(QPixmap::fromImage(image));
		label.show();
		cv::waitKey(10000);
	}
	void test2()
	{
		cv::Mat mat = cv::imread(test_img_path, cv::IMREAD_UNCHANGED);
		cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
		QImage image = cvMat2QImage(mat);
		cv::namedWindow("cvMat2QImage gray", cv::WINDOW_AUTOSIZE);
		imshow("cvMat2QImage gray", mat);
		QLabel label;
		label.setPixmap(QPixmap::fromImage(image));
		label.show();
		cv::waitKey(10000);
	}
	void test3()
	{
		QImage image(test_img_path);
		image = image.convertToFormat(QImage::Format_RGB32);
		cv::Mat mat = QImage2cvMat(image);
		//cv::cvtColor(mat, mat, CV_BGR2RGB);
		imshow("QImage2cvMat RGB32", mat);
		cv::waitKey(10000);
	}

	void test4()
	{
		QImage image(test_img_path);
		image = image.convertToFormat(QImage::Format_RGB888);
		cv::Mat mat = QImage2cvMat(image);
		imshow("QImage2cvMat RGB24", mat);
		cv::waitKey(10000);
	}
	void test5()
	{
		QImage image(test_img_path);
		image = image.convertToFormat(QImage::Format_Indexed8);
		cv::Mat mat = QImage2cvMat(image);
		imshow("QImage2cvMat Indexed8", mat);
		cv::waitKey(10000);
	}


	bool update_models() {
		QString strInfo;
		QProcess p(nullptr);
		QString curPath = QDir::currentPath();
		QString toolPath;
		toolPath = "/models";
		curPath.append(toolPath);
		p.setWorkingDirectory(curPath);



		QString script_file = "/rsync.bat";

		curPath.append(script_file);

		p.start(curPath);
		if (p.waitForFinished()) {
			strInfo = "完成！";

		}
		else {
			strInfo = "bat运行错误";
			return false;
		}

		qDebug() << "cmd output: " << p.readAllStandardOutput();
		return true;

	}


}