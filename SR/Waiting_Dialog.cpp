#include "Waiting_Dialog.h"

Waiting_Dialog::Waiting_Dialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

Waiting_Dialog::Waiting_Dialog(QString content, QWidget *parent)
	:QDialog(parent)
{
	ui.setupUi(this);
	ui.label->setText(content);
}

Waiting_Dialog::~Waiting_Dialog()
{
}
