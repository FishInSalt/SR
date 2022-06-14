#pragma once

#include <QMainWindow>
#include "ui_Image_Window.h"
#include <qgraphicsitem.h>


class SRThread;
class Image_Window : public QMainWindow
{
	Q_OBJECT

public:
	explicit Image_Window(QWidget *parent = Q_NULLPTR);
	~Image_Window();
	/*bool eventFilter(QObject* watched, QEvent *event) override;*/

protected:
	
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dropEvent(QDropEvent *event) override;
	void closeEvent(QCloseEvent* event) override;

private:
	Ui::Image_Window ui;
	
	//当前在界面显示的image以及其路径
	QImage _curShow_img;
	QString _curShow_img_path;

	QImage _curShow_sr_img;

	//需要保存的图片列表,包含了图片文件的名字（不是路径）QString以及图片的数据 QImage
	QList<QPair<QString, QImage>> _images_to_save;
	
	//当前使用的模型的名字
	QString _model_name;
	//模型文件的路径
	QString _models_dir;  //存放模型的路径，到.../models/

	int _imgs_length; // 当前要处理的图片的数量

	bool _srThread_running = false;

	//界面上指向显示的图片的item
	QGraphicsPixmapItem *_input_image_item = Q_NULLPTR;
	QGraphicsPixmapItem * _output_image_item = Q_NULLPTR;

	SRThread * _srThread_ptr = nullptr;

	// 输出路径
	QString _output_path;

private:
	void set_image(QString img_path);
	void init();
	bool save_image(QString save_path, const QImage &img);
	void save_result(QString save_path);

	void get_params_info(int &scale, int &diy_height, int &diy_width, QString &model_name, QString &selected_scene, QString &batch_path);

	QStringList search_model(QString model_dir,QString scene);

	void set_sr_image(const QImage &sr_img);

	void reset_input();

	void start_super_resolution();


private slots:
	void on_openFile_action_triggered();
	void on_openDir_action_triggered();
	void on_outputDir_button_clicked();
	void on_scale_radioButton_toggled();
	void on_diy_radioButton_toggled();
	void on_auto_radioButton_toggled();
	void on_manual_radioButton_toggled();
	void on_compare_pushButton_clicked();
	void on_save_action_triggered();
	//void on_saveTo_action_triggered();

	void on_scene_comboBox_currentIndexChanged();
	void on_start_pushButton_clicked();

	void on_srThread_returnImg(QString img_file_name, QImage img);
	void on_srThread_return_invalid_modelPath();

	void on_srThread_reutrn_length(int length);
	void on_srThread_started();
	void on_srThread_finished();
	//void on_manual_radioButton_toggled();
//	void on_save_action_triggered();
//	void on_saveTo_action_triggered();

signals:
	void force_thread_exit();
};
