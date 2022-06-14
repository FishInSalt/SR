#include "SR.h"
#include "Image_Window.h"

#include <qstring.h>
#include <QImage>
#include <Qpixmap>
#include <qdebug.h>
#include <qdialog.h>
#include "utils.h"
#include "Waiting_Dialog.h"
#include <qmessagebox.h>


SR::SR(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	
	//设置按钮的样式
	QString buttonstyle = " QPushButton{font-family:'宋体'; font-size:32px; color:rgb(0,0,0,255);} \
				QPushButton{ background-color:rgb(170,200,50)}\
			    QPushButton:hover{ background-color:rgb(50, 170, 200)}";
	ui.pushButton->setStyleSheet(buttonstyle);
	ui.pushButton_2->setStyleSheet(buttonstyle);

	//QString demo_img_path = "./demo_image/ico.png";
	//QImage  img(demo_img_path,"png");

	//ui.label->setPixmap(QPixmap::fromImage(img).scaled(ui.label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
	//ui.label->setScaledContents(true);


	//connect(ui.actionopen, SIGNAL(triggered()), this, SLOT(actionopenclick()));
	//connect(ui.actionopen_2, SIGNAL(triggered()), this, SLOT(actionopen2click()));
	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(open_image_window()));
	//connect(ui.update_action, &QAction::triggered, this, &SR::on_update_action_triggered);

}

//void SR::actionopenclick() {
//	ui.stackedWidget->setCurrentIndex(0);
//}
//void SR::actionopen2click() {
//	ui.stackedWidget->setCurrentIndex(1);
//}

void SR::open_image_window() {
	Image_Window * img_win = new Image_Window(this);
	img_win->setAttribute(Qt::WA_DeleteOnClose);
	img_win->show();
}

void SR::on_update_action_triggered() {

	Waiting_Dialog* waiting_dialog = new Waiting_Dialog(QString::fromLocal8Bit("更新模型中..."),this);
	waiting_dialog->setAttribute(Qt::WA_DeleteOnClose);

	waiting_dialog->show();

	QCoreApplication::processEvents();
	bool res = utils::update_models();
	waiting_dialog->close();

	if (res == true) {
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("更新成功！"));
	}
	else {
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("更新失败！"));
	}

	


}
