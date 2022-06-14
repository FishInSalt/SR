#include "SR.h"
#include <QtWidgets/QApplication>


#include "load_model/SRModel.h"
#include <QDebug>
#include "utils.h"

void model_test() {
	std::string img_path = "E:/Program Files (x86)/Waifu2x-Extension-GUI-v3.84.01-Win64/data/LR images/128.png";
	std::string model_path = "./models/normal/default/x2/model_carn_libtorch_cpu_x2.pt";
	int scale = 2;
	test_CARN(img_path, model_path, scale);


}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SR w;
	//utils::test1();
	//utils::test2();
	//utils::test3();
	//utils::test4();
	//utils::test5();
	//model_test();

    w.show();
    return a.exec();
}
