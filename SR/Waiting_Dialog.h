#pragma once

#include <QDialog>
#include "ui_Waiting_Dialog.h"

class Waiting_Dialog : public QDialog
{
	Q_OBJECT

public:
	Waiting_Dialog(QWidget *parent = Q_NULLPTR);
	Waiting_Dialog(QString content, QWidget *parent = Q_NULLPTR);
	~Waiting_Dialog();

private:
	Ui::Waiting_Dialog ui;
};
