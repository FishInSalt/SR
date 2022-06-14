#pragma once

#include <QWidget>
#include "ui_Scaled_Widget.h"
#include <qimage.h>

class Scaled_Widget : public QWidget
{
	Q_OBJECT

public:
	Scaled_Widget(QWidget *parent = Q_NULLPTR);
	Scaled_Widget(QImage img, QWidget *parent = Q_NULLPTR);
	~Scaled_Widget();

private:
	Ui::Scaled_Widget ui;
	QImage scaled_widget_img;
	double scale_factor = 1;

	QGraphicsPixmapItem * imageItem;

private:
	void init();

	void draw_image(double scale);

	

private slots:
	void on_zoomIn();
	void on_zoomOut();
};
