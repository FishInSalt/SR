#include "SRThread.h"
#include <QDir>
#include <QImage>
#include <qcoreapplication.h>

#include "utils.h"
#include "load_model/SRModel.h"

SRThread::SRThread(QObject *parent)
	: QThread(parent)
{
}

SRThread::SRThread(QString model_path, QString curShow_img_path, int scale , 
		int diy_height , int diy_width, QString batch_dir , QObject *parent) 
	:QThread(parent), model_path_(model_path),curShow_img_path_(curShow_img_path),scale_(scale),
	diy_height_(diy_height),diy_width_(diy_width),batch_path_(batch_dir)
{

 }

SRThread::~SRThread()
{
}


void SRThread::stopThread()
{
	stop_ = 1;
}

void SRThread::run()
{
	work();
	quit();
}

void SRThread::work()
{
	////先收集所有的输入信息
	//int scale = 0;
	//int diy_height = -1, diy_width = -1;
	//QString model_name, selected_scene, batch_path;
	//get_params_info(scale, diy_height, diy_width, model_name, selected_scene, batch_path);


	////获取模型文件的路径并加载模型，模型文件的路径为为 models/sence/model_name/xScale/*.pt

	//QString model_dir = _models_dir;
	//if (selected_scene == QString::fromLocal8Bit("通用")) {
	//	model_dir += "/normal";
	//}
	//else if (selected_scene == QString::fromLocal8Bit("动漫")) {
	//	model_dir += "/anime";
	//}
	//else if (selected_scene == QString::fromLocal8Bit("人脸")) {
	//	model_dir += "/human face";
	//}


	//model_dir += "/" + model_name;

	QString imagePostfix = "bmp,jpeg,png,jpg";

	QImage _curShow_img(curShow_img_path_, imagePostfix.toStdString().c_str());
	int scale = scale_;
	QString model_dir = model_path_;
	if (diy_height_ > 0 && diy_width_ > 0) {
		//先提示diy分辨率，可能会对原图裁剪
		//auto res = QMessageBox::information(this, QString::fromLocal8Bit("提示"),
		//	QString::fromLocal8Bit("自定义分辨率时，可能会对原图进行对应的裁剪，确定自定义分辨率吗？")
		//	, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		//if (res == QMessageBox::No)
		//	return;

		//自定义分辨率时,计算扩大倍数
		float scale1 = float(diy_width_) / _curShow_img.width();
		float scale2 = float(diy_height_) / _curShow_img.height();

		if (scale1 < scale2) {
			scale = diy_height_ % _curShow_img.height() == 0 ?
				scale2 : scale2 + 1;
		}
		else {
			scale = diy_width_ % _curShow_img.width() == 0 ?
				scale1 : scale1 + 1;

		}

		if (scale > 4)
			scale = 4;

	}
	model_dir += "/x" + QString::number(scale);

	QDir dir(model_dir);

	if (!dir.exists()) {
		emit invalid_model_path();
		return;
	}

	QStringList filters;
	filters << "*.pt";
	auto model_path_list = dir.entryInfoList(filters, QDir::Files | QDir::Readable, QDir::Name);

	if (model_path_list.length() == 0) {
		emit invalid_model_path();
		return;
	}

	QString model_path = model_path_list.front().filePath();
	SRModel sr_model(model_path.toStdString());


	//获取要处理的图片的路径
	QList<QString> input_imgs_path;

	if (batch_path_.isEmpty()) {
		input_imgs_path.push_back(curShow_img_path_);
	}
	else {
		QDir dir(batch_path_);
		QStringList nameFilters = { "*.png","*.jpg","*.jpeg","*.bmp" };
		auto files = dir.entryInfoList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
		for (auto it : files) {
			input_imgs_path.push_back(it.filePath());
		}

	}
	emit imgs_length(input_imgs_path.length());


	

	for (int i = 0; i < input_imgs_path.length() ; ++i) {
		if (stop_.load() == 1)
			break;

		auto img_path = input_imgs_path[i];
		
		QImage cur_img(img_path, imagePostfix.toStdString().c_str());

		cur_img = cur_img.convertToFormat(QImage::Format_RGB888);

		if (diy_height_ > 0 && diy_width_ > 0) {
			cur_img = utils::clip_image(cur_img, diy_height_, diy_width_);
		}

		int cur_img_height = cur_img.height();
		int cur_img_width = cur_img.width();

		QImage padding_img = cur_img;
		if (cur_img.height() != cur_img.width()) {
			padding_img = utils::padding_image(cur_img);
		}


		//调用模型
		auto sr_img = utils::invoke_model(sr_model, padding_img);


		//可能需要对原图进行修剪
		sr_img = utils::clip_image(sr_img, cur_img_height, cur_img_width);


		//if (img_path.compare(_curShow_img_path) == 0) {
		//	_curShow_sr_img = sr_img;
		//	set_sr_image(sr_img);
		//}

		//设置图片的保存信息
		QString img_file_name = img_path.split(QRegExp("[\\/]"), QString::SkipEmptyParts).back();

		emit new_sr_img(img_file_name, sr_img);


		//_images_to_save.push_back(QPair<QString, QImage>(img_file_name, sr_img));

		//更新进度条
		//ui.progressBar->setValue((i + 1) * 100 / input_imgs_path.length());
		QCoreApplication::processEvents();
	}

	//ui.dealingStatus_laebl->setText(QString::fromLocal8Bit("完成"));
	//ui.compare_pushButton->setEnabled(true);
}


