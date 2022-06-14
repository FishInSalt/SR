#include "Scaled_Widget.h"

#include <qgraphicsscene.h>
#include <qtoolbar.h>
#include <qgraphicsitem.h>
#include <qdebug.h>

Scaled_Widget::Scaled_Widget(QWidget *parent)
	: QWidget(parent)
{
	Scaled_Widget(QImage(), parent);
}

Scaled_Widget::Scaled_Widget(QImage img, QWidget *parent) 
	:QWidget(parent),scaled_widget_img(img)
{
	ui.setupUi(this);
	init();
	

	
	

}

Scaled_Widget::~Scaled_Widget()
{
}

void Scaled_Widget::init() {

	QToolBar *toolbar = new QToolBar(this);
	QAction *zoomIn_action = new QAction(QString::fromLocal8Bit("·Å´ó"), this);
	QAction *zoomOut_action = new QAction(QString::fromLocal8Bit("ËõÐ¡"), this);

	toolbar->addAction(zoomIn_action);
	toolbar->addAction(zoomOut_action);

	QGraphicsScene *scene = new QGraphicsScene(this);
	imageItem =  scene->addPixmap(QPixmap::fromImage(scaled_widget_img));
	//imageItem->setOffset(ui.graphicsView->width()/2, ui.graphicsView->height()/2);

	ui.graphicsView->setScene(scene);
	//ui.graphicsView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
	//ui.graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);

	connect(zoomIn_action, &QAction::triggered, this, &Scaled_Widget::on_zoomIn);
	connect(zoomOut_action, &QAction::triggered, this, &Scaled_Widget::on_zoomOut);
}

void Scaled_Widget::draw_image(double scale) {
	int width = scaled_widget_img.width() * scale;
	int height = scaled_widget_img.height() * scale;
	qDebug()  << "width: " << width << "h: " << height ;
	auto scaled_img = scaled_widget_img.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	
	imageItem->setPixmap(QPixmap::fromImage(scaled_img));

	ui.graphicsView->scene()->setSceneRect(imageItem->sceneBoundingRect());
	//imageItem->setOffset(ui.graphicsView->width(), ui.graphicsView->height());

}


void Scaled_Widget::on_zoomIn() {
	scale_factor += 0.2;
	draw_image(scale_factor);


}


void Scaled_Widget::on_zoomOut() {
	scale_factor -= 0.2;
	draw_image(scale_factor);
}