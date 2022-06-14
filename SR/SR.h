#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SR.h"

class SR : public QMainWindow
{
    Q_OBJECT

public:
    explicit SR(QWidget *parent = Q_NULLPTR);

private:
    Ui::SRClass ui;

private slots:

//	void actionopenclick();
//	void actionopen2click();

	void on_update_action_triggered();

	void open_image_window();
};
