#pragma once

#include <QThread>
#include <QString>
#include <QImage>
#include <qmutex.h>
#include <qatomic.h>

class SRThread : public QThread
{
	Q_OBJECT

public:
	SRThread(QObject *parent =nullptr);
	SRThread(QString model_path, QString curShow_img_path, int scale =2,int diy_height = -1 ,int diy_width = -1, QString batch_dir = QString(), QObject *parent = nullptr);
	~SRThread();

	void stopThread();


protected:
	void run() Q_DECL_OVERRIDE	; 

private:
	//超分所需的信息
	QString model_path_;  // path to ../model_name
	QString batch_path_;  // 批量操作的路径
	int scale_;
	QString curShow_img_path_;
	int diy_height_;
	int diy_width_;

	QAtomicInt stop_ = 0;


private:
	void work();

signals:
	void new_sr_img(QString img_file_name, QImage sr_img);
signals:
	void invalid_model_path();
signals:
	void imgs_length(int length);
};
