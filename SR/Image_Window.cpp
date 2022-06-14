#include "Image_Window.h"
#include <qgraphicsscene.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qdebug.h>
#include <qdrag.h>
#include <qevent.h>
#include <qmimedata.h>
#include <Windows.h>


#include "Scaled_Widget.h"
#include "utils.h"
#include "SRThread.h"
#include "Waiting_Dialog.h"



QStringList g_nameFilters = { "*.png","*.jpg","*.jpeg","*.bmp" };
QString g_imagePostfix = "bmp,jpeg,png,jpg";

QString default_output_path = QDir::currentPath() + "/outputs";



Image_Window::Image_Window(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	
	init();
	

}

Image_Window::~Image_Window()
{
}


void Image_Window::init() {

	//成员赋值
	_output_path = default_output_path;
	_models_dir = QDir::currentPath() + "/models";

	// 设置菜单栏中的基本功能
	QAction* open_action = new QAction(QString::fromLocal8Bit("打开文件"),this);
	QAction* open_dir_action = new QAction(QString::fromLocal8Bit("打开文件夹"),this);
	QAction* save_action = new QAction(QString::fromLocal8Bit("保存"), this);
	QAction* saveTo_action = new QAction(QString::fromLocal8Bit("保存到"), this);
	QAction* exit_action = new QAction(QString::fromLocal8Bit("退出"), this);

	ui.file_menu->addAction(open_action);
	ui.file_menu->addAction(open_dir_action);
	ui.file_menu->addSeparator();
	ui.file_menu->addAction(save_action);
	ui.file_menu->addAction(saveTo_action);
	ui.file_menu->addSeparator();
	ui.file_menu->addAction(exit_action);


	ui.mainToolBar->addAction(open_action);
	ui.mainToolBar->addAction(open_dir_action);
	ui.mainToolBar->addSeparator();
	ui.mainToolBar->addAction(save_action);
	ui.mainToolBar->addAction(saveTo_action);
	ui.mainToolBar->addSeparator();
	ui.mainToolBar->addAction(exit_action);

	
	connect(exit_action, SIGNAL(triggered(bool)), this, SLOT(close()));
	connect(open_action, &QAction::triggered, this, &Image_Window::on_openFile_action_triggered);
	connect(open_dir_action, &QAction::triggered, this, &Image_Window::on_openDir_action_triggered);
	connect(ui.open_inputDir_button, &QPushButton::clicked, this, &Image_Window::on_openDir_action_triggered);
	connect(ui.open_outputDir_button, &QPushButton::clicked, this, &Image_Window::on_outputDir_button_clicked);

	connect(save_action, &QAction::triggered, this, &Image_Window::on_save_action_triggered);
	connect(ui.save_pushButton, &QPushButton::clicked, this, &Image_Window::on_save_action_triggered);
	connect(saveTo_action, &QAction::triggered, this, &Image_Window::on_outputDir_button_clicked);
	
	// 初始化界面的一些组件状态

	ui.inputDir_textLine->setFocusPolicy(Qt::NoFocus);
	
	ui.outputDir_textLine->setText(_output_path);
	ui.outputDir_textLine->setFocusPolicy(Qt::NoFocus);
	
	ui.scale_radioButton->setChecked(1);
	ui.diy_height_lineEdit->setEnabled(0);
	ui.diy_width_lineEdit->setEnabled(0);
	ui.auto_radioButton->setChecked(1);
	ui.modelSelected_comboBox->setEnabled(0);

	ui.auto_radioButton->setChecked(1);
	ui.modelSelected_comboBox->setEnabled(0);

	ui.input_image->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.input_image->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	ui.output_image->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.output_image->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	ui.compare_pushButton->setEnabled(false);



	//installEventFilter(this);
	this->setAcceptDrops(true);
	
	//ui.input_image->installEventFilter(this);
	ui.input_image->setAcceptDrops(false);
	ui.input_image->setAlignment(Qt::AlignCenter);
	
	QStringList scene_list;
	scene_list  << QString::fromLocal8Bit("通用") 
				<< QString::fromLocal8Bit("动漫") 
				<< QString::fromLocal8Bit("人脸");

	ui.scene_comboBox->addItems(scene_list);

	ui.diy_height_lineEdit->setValidator(new QIntValidator(this));
	ui.diy_width_lineEdit->setValidator(new QIntValidator(this));



	// 设置demo图片
	QString image_demo_path = "E:/learning/SR/SR/demo_image/anime_face.png";
	set_image(image_demo_path);
}

void Image_Window::set_image(QString img_path ) {


	
	
	//qDebug() << "set_image: "<<img_path  << endl;

	_curShow_img_path = img_path;

	_curShow_img = QImage(img_path, g_imagePostfix.toStdString().c_str());
	

	QPixmap demo_image = QPixmap::fromImage(_curShow_img);
	//qDebug() << "pixmap size: " << demo_image.width()<<" "<<demo_image.height() << endl;
	auto scaled_image = demo_image.scaled(ui.input_image->width(), ui.input_image->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);

	if (_input_image_item == Q_NULLPTR) {
		QGraphicsScene * input_scene = new QGraphicsScene(this);
		_input_image_item = input_scene->addPixmap(scaled_image);
		ui.input_image->setScene(input_scene);
		_input_image_item->setAcceptDrops(false);
		
	}
	else {
		_input_image_item->setPixmap(scaled_image);
		if (scaled_image.width() == ui.input_image->width()) {
			int offsetY = (ui.input_image->height() - scaled_image.height()) / 2;
			_input_image_item->setOffset(0, offsetY);
		}
		else {
			int offsetX = (ui.input_image->width() - scaled_image.width()) / 2;
			_input_image_item->setOffset(offsetX, 0);
		}
	}
	
	ui.input_height->setText(QString::number(demo_image.height()));
	ui.input_width->setText(QString::number(demo_image.width()));



	auto output_scaled_image = demo_image.scaled(ui.output_image->width(), ui.output_image->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	if (_output_image_item == Q_NULLPTR) {
		QGraphicsScene * output_scene = new QGraphicsScene(this);
		_output_image_item = output_scene->addPixmap(output_scaled_image);
		ui.output_image->setScene(output_scene);
	}
	else {
		_output_image_item->setPixmap(output_scaled_image);
		if (output_scaled_image.width() == ui.output_image->width()) {
			int offsetY = (ui.output_image->height() - output_scaled_image.height()) / 2;
			_output_image_item->setOffset(0, offsetY);
		}
		else {
			int offsetX = (ui.output_image->width() - output_scaled_image.width()) / 2;
			_output_image_item->setOffset(offsetX, 0);
		}
	}



}


void Image_Window::on_openFile_action_triggered() {
	
	QString img_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开文件"), QDir::currentPath(),
		QString::fromLocal8Bit("Image Files(*.jpg *.png *.jpeg *.bmp)"));
	if (img_path.length() == 0) {
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("没有指定任何文件"));
		return;
	}

	reset_input();
	set_image(img_path);




}

void Image_Window::on_openDir_action_triggered() {

	
	QString dir_path = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("打开文件夹"), QDir::currentPath(),
															QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
	if (dir_path.length() == 0) {
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("没有指定任何文件"));
		return;
	}
	
	
	QDir dir(dir_path);
	auto files = dir.entryInfoList(g_nameFilters, QDir::Files | QDir::Readable, QDir::Name);
	if (files.length() == 0) {
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("找不到图像文件"));
		return;
	}


	reset_input();
	//qDebug() << files << endl;
	ui.inputDir_textLine->setText(dir_path);
	//qDebug() << files.front().filePath() << endl;
	set_image( files.front().filePath().replace("\\","/"));


	
}

void Image_Window::on_outputDir_button_clicked() {
	QString dir_path = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("打开文件夹"), QDir::currentPath(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir_path.length() == 0) {
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("没有指定任何文件"));
		return;
	}


	//qDebug() << files << endl;
	ui.outputDir_textLine->setText(dir_path);
	_output_path = dir_path;

}

void Image_Window::on_scale_radioButton_toggled() {
	//点击scale_radioButton时，diy组件不可用，scale组件可用

	if (ui.scale_radioButton->isChecked()) {
		ui.scale_comboBox->setEnabled(1);
	}
	else {
		ui.scale_comboBox->setEnabled(0);
	}

	
	
}

void Image_Window::on_diy_radioButton_toggled() {
	//点击diy_radioButton时，diy组件可用，scale组件可用
	if (ui.diy_radioButton->isChecked()) {
		ui.diy_height_lineEdit->setEnabled(1);
		ui.diy_width_lineEdit->setEnabled(1);

		ui.inputDir_textLine->clear();
		ui.open_inputDir_button->setEnabled(false);
	}
	else {
		ui.diy_height_lineEdit->setEnabled(0);
		ui.diy_width_lineEdit->setEnabled(0);
		ui.open_inputDir_button->setEnabled(true);
	}



}

void Image_Window::on_auto_radioButton_toggled() {
	

}

void Image_Window::on_manual_radioButton_toggled() {
	if (ui.manual_radioButton->isChecked()) {
		ui.modelSelected_comboBox->setEnabled(1);
		QString model_scene = ui.scene_comboBox->currentText();

		QString model_dir = _models_dir;
		auto model_name_list = search_model(model_dir, model_scene);
		ui.modelSelected_comboBox->addItems(model_name_list);
	}
	else {
		ui.modelSelected_comboBox->setEnabled(0);
		ui.modelSelected_comboBox->clear();
	}
	

	
}

void Image_Window::on_compare_pushButton_clicked() {



	auto srcImg_Widget = new Scaled_Widget(_curShow_img);
	srcImg_Widget->setAttribute(Qt::WA_DeleteOnClose);

	
	srcImg_Widget->setWindowTitle(QString::fromLocal8Bit("原图片"));
	srcImg_Widget->setWindowFlags(Qt::CustomizeWindowHint| Qt::WindowTitleHint|Qt::WindowCloseButtonHint|Qt::WindowMinMaxButtonsHint);

	srcImg_Widget->show();
	auto pos = srcImg_Widget->mapToGlobal( QPoint(0,0));
	srcImg_Widget->move(pos.x()-50,pos.y());



	float scale = float(_curShow_sr_img.width()) / _curShow_img.width();
	QImage bicubic_img = utils::bicubic_interpolation(_curShow_img,scale);

	int h = bicubic_img.height(), w = bicubic_img.width();

	auto bicubicImg_Widget = new Scaled_Widget(bicubic_img);
	bicubicImg_Widget->setAttribute(Qt::WA_DeleteOnClose);
	bicubicImg_Widget->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
	bicubicImg_Widget->setWindowTitle(QString::fromLocal8Bit("插值算法修复图片"));

	bicubicImg_Widget->show();
	bicubicImg_Widget->move(pos.x()+50,pos.y());


	auto srImg_Widget = new Scaled_Widget(_curShow_sr_img);
	srImg_Widget->setAttribute(Qt::WA_DeleteOnClose);
	srImg_Widget->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
	srImg_Widget->setWindowTitle(QString::fromLocal8Bit("深度学习超分辨率修复图片"));

	srImg_Widget->show();
	srImg_Widget->move(pos);

	//delete showImg_Widget;
}


void Image_Window::dragEnterEvent(QDragEnterEvent * event) {
	//QMainWindow::dragEnterEvent(event);
	event->acceptProposedAction();

}

void Image_Window::dropEvent(QDropEvent *event) {
	//QMainWindow::dropEvent(event);
	auto posX = event->pos().x();
	auto posY = event->pos().y();
	int minX = ui.input_image->x();
	int minY = ui.input_image->y();
	int maxX = ui.input_image->width() + minX;
	int maxY = ui.input_image->height() + minY;

	//qDebug() << " " << minX << " " << minY << " " << maxX << " " << maxY;

	if (minX <= posX && posX <= maxX && minY <= posY && posY <= maxY) {
		auto urls = event->mimeData()->urls();

		QString path = urls.first().toLocalFile();
		qDebug() << endl << "input path:  " << path;
		QString postfix = path.split(".").back();
		if (g_imagePostfix.contains(postfix.toLower())) {
			reset_input();
			set_image(path);
		}
		else {
			QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("无效文件"));
		}
	}

	QMainWindow::dropEvent(event);

}

void Image_Window::closeEvent(QCloseEvent * event)
{

	Waiting_Dialog* waiting_dialog = new Waiting_Dialog(QString::fromLocal8Bit("等待退出..."), this);
	waiting_dialog->setAttribute(Qt::WA_DeleteOnClose);
	waiting_dialog->setWindowModality(Qt::ApplicationModal);
	waiting_dialog->setWindowFlags(waiting_dialog->windowFlags() &~Qt::WindowCloseButtonHint & ~Qt::WindowContextHelpButtonHint );
	
	waiting_dialog->show();
	QCoreApplication::processEvents();
	
	if (_srThread_ptr != nullptr) {
		while (_srThread_ptr->isRunning()) {
			_srThread_ptr->stopThread();
			_srThread_ptr->wait();
			QCoreApplication::processEvents();
		}
	}

	waiting_dialog->close();

	event->accept();

}

void Image_Window::on_save_action_triggered() {

	save_result(_output_path);

}

bool Image_Window::save_image(QString save_path,const QImage &img) {
	return img.save(save_path);
	
}

void Image_Window::save_result(QString save_path) {
	QDir output_dir(save_path);
	if (!output_dir.exists()) {
		bool mkdir_res = output_dir.mkpath(save_path);
		if (!mkdir_res) {
			QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("输出目录不存在，且无法创建！"));
		}
	}
	
	int scale = 0;
	int diy_height = -1, diy_width = -1;
	QString model_name, selected_scene, batch_path;
	get_params_info(scale, diy_height, diy_width, model_name, selected_scene, batch_path);


	if (!batch_path.isEmpty()) {

		QString batch_dirName = batch_path.split(QRegExp("[\\/]"), QString::SkipEmptyParts).back();
		if (!output_dir.exists(batch_dirName)) {
			bool mkdir_res = output_dir.mkdir(batch_dirName);
			if (!mkdir_res) {
				QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("输出目录不存在，且无法创建！"));
			}
		}

		output_dir.cd(batch_dirName);
	}

	



	for (auto it : _images_to_save) {

		QString image_name = it.first;

		qDebug() << "image name: " << image_name << endl;

		if (diy_height > 0 && diy_width > 0) {
			image_name = model_name+ "_" + QString::number(diy_width) + "x" + QString::number(diy_height) + "_" + image_name;
		
		}
		else {
			image_name = model_name + "_x"+ QString::number(scale)+"_" + image_name;
		
		}


		QString output_img_path = output_dir.absoluteFilePath(image_name);
		bool save_res = save_image(output_img_path, it.second);
		if (!save_res) {
			QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("保存图片: ") + image_name + QString::fromLocal8Bit(" 失败！"));
		}
	}
}





QStringList Image_Window::search_model(QString model_dir,QString scene) {
	QStringList modelName_list;

	if (scene == QString::fromLocal8Bit("通用")) {
		model_dir += "/normal";
	}
	else if (scene == QString::fromLocal8Bit("动漫")) {
		model_dir += "/anime";
	}
	else if (scene == QString::fromLocal8Bit("人脸")) {
		model_dir += "/human face";
	}

	QDir dir(model_dir);

	//QStringList filter;
	//filter << "default";
	if (dir.exists()) {
		modelName_list = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	}

	return modelName_list;
}

void Image_Window::on_scene_comboBox_currentIndexChanged() {
	if (ui.manual_radioButton->isChecked()) {
		QString current_scene = ui.scene_comboBox->currentText();
		QString model_dir = _models_dir;
		auto model_name_list = search_model(model_dir, current_scene);
		ui.modelSelected_comboBox->clear();
		ui.modelSelected_comboBox->addItems(model_name_list);
	}
}



//用于在开始前，收集所有需要的输入信息
void Image_Window::get_params_info(int &scale,int &diy_height,int &diy_width,QString &model_name,QString &selected_scene, QString &batch_path) {

	if (ui.scale_radioButton->isChecked()) {
		QString scale_text = ui.scale_comboBox->currentText();
		scale = scale_text.mid(1).toInt();
	}
	if (ui.diy_radioButton->isChecked())
	{
		QString diy_height_text = ui.diy_height_lineEdit->text();
		QString diy_width_text = ui.diy_width_lineEdit->text();

		diy_height = diy_height_text.toInt();
		diy_width = diy_width_text.toInt();
	}
	if (ui.auto_radioButton->isChecked()) {
		model_name = "default";
	}
	if (ui.manual_radioButton->isChecked()) {
		model_name = ui.modelSelected_comboBox->currentText();
	}
	selected_scene = ui.scene_comboBox->currentText();

	batch_path = ui.inputDir_textLine->text();

}

//开始按钮功能
void Image_Window::on_start_pushButton_clicked() {

	ui.start_pushButton->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	ui.save_pushButton->setAttribute(Qt::WA_TransparentForMouseEvents, true);

	//先收集所有的输入信息
	int scale = 0;
	int diy_height = -1, diy_width = -1;
	QString model_name, selected_scene, batch_path;
	get_params_info(scale, diy_height, diy_width, model_name, selected_scene, batch_path);


	//获取模型文件的路径并加载模型，模型文件的路径为为 models/sence/model_name/xScale/*.pt

	QString model_dir = _models_dir;
	if (selected_scene == QString::fromLocal8Bit("通用")) {
		model_dir += "/normal";
	}
	else if (selected_scene == QString::fromLocal8Bit("动漫")) {
		model_dir += "/anime";
	}
	else if (selected_scene == QString::fromLocal8Bit("人脸")) {
		model_dir += "/human face";
	}


	model_dir += "/" + model_name;

	if (diy_height > 0 && diy_width > 0) {
		//先提示diy分辨率，可能会对原图裁剪
		auto res = QMessageBox::information(this, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("自定义分辨率时，可能会对原图进行对应的裁剪，确定自定义分辨率吗？")
			, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (res == QMessageBox::No)
			return;
	}


	SRThread sr_thread(model_dir, _curShow_img_path, scale,diy_height,diy_width, batch_path);
	connect(&sr_thread, &SRThread::new_sr_img, this, &Image_Window::on_srThread_returnImg);
	connect(&sr_thread, &SRThread::invalid_model_path, this, &Image_Window::on_srThread_return_invalid_modelPath);
	connect(&sr_thread, &SRThread::imgs_length, this, &Image_Window::on_srThread_reutrn_length);
	//connect(this, &Image_Window::force_thread_exit, &sr_thread, &SRThread::quit);
	//connect(&sr_thread, &SRThread::started, this, &Image_Window::on_srThread_started);
	//connect(&sr_thread, &SRThread::finished, this, &Image_Window::on_srThread_finished);

	_srThread_ptr = &sr_thread;
	//_srThread_running = true;
	sr_thread.start();
	while (sr_thread.isRunning()) {

		QCoreApplication::processEvents();
	}

	//_srThread_running = false;
	_srThread_ptr = nullptr;

	ui.start_pushButton->setAttribute(Qt::WA_TransparentForMouseEvents, false);
	ui.save_pushButton->setAttribute(Qt::WA_TransparentForMouseEvents, false);

	
}

void Image_Window::on_srThread_returnImg(QString img_file_name,QImage img )
{
	_images_to_save.append(QPair<QString,QImage>(img_file_name,img));
	QString  cur_img_name = _curShow_img_path.split(QRegExp("[\\/]"), QString::SkipEmptyParts).back();
	if (cur_img_name.compare(img_file_name) == 0) {
		_curShow_sr_img = img;
		set_sr_image(img);
	}
	ui.progressBar->setValue(_images_to_save.length()  * 100 /_imgs_length );
	if (_images_to_save.length() == _imgs_length) {
		ui.dealingStatus_laebl->setText(QString::fromLocal8Bit("完成"));
		ui.compare_pushButton->setEnabled(true);
	}
}

void Image_Window::on_srThread_return_invalid_modelPath()
{
	QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("不是有效的模型路径"));
}

void Image_Window::on_srThread_reutrn_length(int length)
{
	_imgs_length = length;

}

void Image_Window::on_srThread_started()
{
	_srThread_running = true;
}

void Image_Window::on_srThread_finished()
{
	_srThread_running = false;
}

void Image_Window::set_sr_image(const QImage &sr_img) {

	auto output_scaled_image = QPixmap::fromImage( 
					sr_img.scaled(ui.output_image->width(), ui.output_image->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

	if (_output_image_item == Q_NULLPTR) {
		QGraphicsScene * output_scene = new QGraphicsScene(this);
		_output_image_item = output_scene->addPixmap(output_scaled_image);
		ui.output_image->setScene(output_scene);
	}
	else {
		_output_image_item->setPixmap(output_scaled_image);
		if (output_scaled_image.width() == ui.output_image->width()) {
			int offsetY = (ui.output_image->height() - output_scaled_image.height()) / 2;
			_output_image_item->setOffset(0, offsetY);
		}
		else {
			int offsetX = (ui.output_image->width() - output_scaled_image.width()) / 2;
			_output_image_item->setOffset(offsetX, 0);
		}
	}

	ui.output_height->setText(QString::number(sr_img.height()));
	ui.output_width->setText(QString::number(sr_img.width()));

}
//重置输入,
void Image_Window::reset_input() {

	//UI重置
	ui.progressBar->setValue(0);
	ui.dealingStatus_laebl->setText(QString::fromLocal8Bit("就绪"));
	ui.compare_pushButton->setEnabled(false);
	ui.output_height->setText(QString::number(0));
	ui.output_width->setText(QString::number(0));
	ui.inputDir_textLine->setText(QString());


	//成员重置
	_curShow_img = QImage();
	_curShow_img_path = "";
	_curShow_sr_img = QImage();
	_images_to_save.clear();
	_imgs_length = 0;

	


}

void Image_Window::start_super_resolution()
{
	//先收集所有的输入信息
	int scale = 0;
	int diy_height = -1, diy_width = -1;
	QString model_name, selected_scene, batch_path;
	get_params_info(scale, diy_height, diy_width, model_name, selected_scene, batch_path);


	//获取模型文件的路径并加载模型，模型文件的路径为为 models/sence/model_name/xScale/*.pt

	QString model_dir = _models_dir;
	if (selected_scene == QString::fromLocal8Bit("通用")) {
		model_dir += "/normal";
	}
	else if (selected_scene == QString::fromLocal8Bit("动漫")) {
		model_dir += "/anime";
	}
	else if (selected_scene == QString::fromLocal8Bit("人脸")) {
		model_dir += "/human face";
	}


	model_dir += "/" + model_name;
	if (diy_height > 0 && diy_width > 0) {
		//先提示diy分辨率，可能会对原图裁剪
		auto res = QMessageBox::information(this, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("自定义分辨率时，可能会对原图进行对应的裁剪，确定自定义分辨率吗？")
			, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (res == QMessageBox::No)
			return;

		//自定义分辨率时,计算扩大倍数
		float scale1 = float(diy_width) / _curShow_img.width();
		float scale2 = float(diy_height) / _curShow_img.height();

		if (scale1 < scale2) {
			scale = diy_height % _curShow_img.height() == 0 ?
				scale2 : scale2 + 1;
		}
		else {
			scale = diy_width % _curShow_img.width() == 0 ?
				scale1 : scale1 + 1;

		}


		if (scale > 4)
			scale = 4;

	}
	model_dir += "/x" + QString::number(scale);

	QDir dir(model_dir);

	if (!dir.exists()) {
		QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("不是有效的模型路径"));
		return;
	}

	QStringList filters;
	filters << "*.pt";
	auto model_path_list = dir.entryInfoList(filters, QDir::Files | QDir::Readable, QDir::Name);

	if (model_path_list.length() == 0) {
		QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("不是有效的模型路径"));
		return;
	}

	QString model_path = model_path_list.front().filePath();
	SRModel sr_model(model_path.toStdString());


	//获取要处理的图片的路径
	QList<QString> input_imgs_path;

	if (batch_path.isEmpty()) {
		input_imgs_path.push_back(_curShow_img_path);
	}
	else {
		QDir dir(batch_path);
		auto files = dir.entryInfoList(g_nameFilters, QDir::Files | QDir::Readable, QDir::Name);
		for (auto it : files) {
			input_imgs_path.push_back(it.filePath());
		}



	}


	for (int i = 0; i < input_imgs_path.length(); ++i) {
		auto img_path = input_imgs_path[i];

		QImage cur_img(img_path, g_imagePostfix.toStdString().c_str());

		cur_img = cur_img.convertToFormat(QImage::Format_RGB888);

		if (diy_height > 0 && diy_width > 0) {
			cur_img = utils::clip_image(cur_img, diy_height, diy_width);
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


		if (img_path.compare(_curShow_img_path) == 0) {
			_curShow_sr_img = sr_img;
			set_sr_image(sr_img);
		}

		//设置图片的保存信息
		QString img_file_name = img_path.split(QRegExp("[\\/]"), QString::SkipEmptyParts).back();


		_images_to_save.push_back(QPair<QString, QImage>(img_file_name, sr_img));

		//更新进度条
		ui.progressBar->setValue((i + 1) * 100 / input_imgs_path.length());

	}

	ui.dealingStatus_laebl->setText(QString::fromLocal8Bit("完成"));
	ui.compare_pushButton->setEnabled(true);

}
