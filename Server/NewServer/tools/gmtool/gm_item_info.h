
#ifndef GM_ITEM_INFO_H_
#define GM_ITEM_INFO_H_

#include "../qt_cpp23.h"
#include <QtWidgets/QDialog>
#include "ui_gm_item_info.h"


class CGMItemInfo : public QDialog
{
    Q_OBJECT

public:
    CGMItemInfo(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~CGMItemInfo();


    void ShowInfo(const char *pcszBuffer, bool bBiggerFont);
    void ShowInfo(const QString& text, bool biggerFont);


public slots:
    void Exit();


private:
    Ui::GMItemInfoDialog ui;
};



#endif //GM_ITEM_INFO_H_



