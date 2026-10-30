/********************************************************************************
** Form generated from reading UI file 'ld_widget.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LD_WIDGET_H
#define UI_LD_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "exchange_history_widget.h"

QT_BEGIN_NAMESPACE

class Ui_ld_widget
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_3;
    QLabel *label_5;
    QLabel *label_6;
    QSpacerItem *horizontalSpacer_5;
    QLabel *label_7;
    QPushButton *pushButton_3;
    QSpacerItem *horizontalSpacer_6;
    QComboBox *comboBox;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_4;
    QTreeWidget *treeWidget;
    QTreeWidget *treeWidget_2;
    QLabel *label_2;
    CExchangeHistoryUI *label_8;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_5;
    QTreeWidget *treeWidget_3;
    QWidget *tab_2;
    QGridLayout *gridLayout_6;
    QTreeWidget *treeWidget_4;
    QWidget *tab_3;
    QGridLayout *gridLayout_7;
    QTreeWidget *treeWidget_5;
    QLabel *label;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_8;
    QPushButton *pushButton_4;
    QDateTimeEdit *dateTimeEdit_2;
    QDateTimeEdit *dateTimeEdit;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_5;
    QSpacerItem *horizontalSpacer;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBox;
    QLabel *label_4;
    QLineEdit *lineEdit;
    QPushButton *pushButton_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton;

    void setupUi(QWidget *ld_widget)
    {
        if (ld_widget->objectName().isEmpty())
            ld_widget->setObjectName(QStringLiteral("ld_widget"));
        ld_widget->resize(1456, 664);
        gridLayout = new QGridLayout(ld_widget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        groupBox = new QGroupBox(ld_widget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        groupBox_3 = new QGroupBox(groupBox);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setMinimumSize(QSize(760, 40));
        gridLayout_3 = new QGridLayout(groupBox_3);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setMinimumSize(QSize(80, 30));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        label_5->setFont(font);

        gridLayout_3->addWidget(label_5, 0, 0, 1, 1);

        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setMinimumSize(QSize(100, 30));
        label_6->setFont(font);

        gridLayout_3->addWidget(label_6, 0, 1, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(336, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_5, 0, 2, 1, 1);

        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setMinimumSize(QSize(160, 30));
        label_7->setFont(font);

        gridLayout_3->addWidget(label_7, 0, 3, 1, 1);

        pushButton_3 = new QPushButton(groupBox_3);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setMinimumSize(QSize(100, 30));
        pushButton_3->setFont(font);

        gridLayout_3->addWidget(pushButton_3, 0, 5, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(336, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_6, 0, 6, 1, 1);

        comboBox = new QComboBox(groupBox_3);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setMinimumSize(QSize(260, 30));
        comboBox->setFont(font);
        comboBox->setMaxVisibleItems(16);

        gridLayout_3->addWidget(comboBox, 0, 4, 1, 1);


        gridLayout_2->addWidget(groupBox_3, 0, 0, 1, 3);

        groupBox_5 = new QGroupBox(groupBox);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        gridLayout_4 = new QGridLayout(groupBox_5);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        treeWidget = new QTreeWidget(groupBox_5);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(treeWidget->sizePolicy().hasHeightForWidth());
        treeWidget->setSizePolicy(sizePolicy);
        treeWidget->setMinimumSize(QSize(200, 150));
        treeWidget->setMaximumSize(QSize(200, 16777215));
        treeWidget->setRootIsDecorated(false);
        treeWidget->header()->setDefaultSectionSize(40);

        gridLayout_4->addWidget(treeWidget, 1, 0, 1, 1);

        treeWidget_2 = new QTreeWidget(groupBox_5);
        treeWidget_2->setObjectName(QStringLiteral("treeWidget_2"));
        sizePolicy.setHeightForWidth(treeWidget_2->sizePolicy().hasHeightForWidth());
        treeWidget_2->setSizePolicy(sizePolicy);
        treeWidget_2->setMinimumSize(QSize(200, 150));
        treeWidget_2->setMaximumSize(QSize(200, 16777215));
        treeWidget_2->setRootIsDecorated(false);
        treeWidget_2->header()->setDefaultSectionSize(40);

        gridLayout_4->addWidget(treeWidget_2, 2, 0, 1, 1);

        label_2 = new QLabel(groupBox_5);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(label_2, 0, 0, 1, 1);

        label_8 = new CExchangeHistoryUI(groupBox_5);
        label_8->setObjectName(QStringLiteral("label_8"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy1);
        label_8->setMinimumSize(QSize(900, 400));
        label_8->setMaximumSize(QSize(900, 16777215));

        gridLayout_4->addWidget(label_8, 0, 1, 3, 1);


        gridLayout_2->addWidget(groupBox_5, 1, 0, 2, 2);

        groupBox_4 = new QGroupBox(groupBox);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setMaximumSize(QSize(310, 16777215));
        verticalLayout = new QVBoxLayout(groupBox_4);
        verticalLayout->setSpacing(9);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabWidget = new QTabWidget(groupBox_4);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setMinimumSize(QSize(300, 0));
        tabWidget->setMaximumSize(QSize(450, 16777215));
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Triangular);
        tabWidget->setIconSize(QSize(16, 16));
        tabWidget->setElideMode(Qt::ElideNone);
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout_5 = new QGridLayout(tab);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        treeWidget_3 = new QTreeWidget(tab);
        treeWidget_3->setObjectName(QStringLiteral("treeWidget_3"));
        sizePolicy.setHeightForWidth(treeWidget_3->sizePolicy().hasHeightForWidth());
        treeWidget_3->setSizePolicy(sizePolicy);
        treeWidget_3->setMinimumSize(QSize(280, 120));
        treeWidget_3->setMaximumSize(QSize(400, 16777215));
        treeWidget_3->setRootIsDecorated(false);
        treeWidget_3->header()->setDefaultSectionSize(40);

        gridLayout_5->addWidget(treeWidget_3, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        gridLayout_6 = new QGridLayout(tab_2);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        treeWidget_4 = new QTreeWidget(tab_2);
        treeWidget_4->setObjectName(QStringLiteral("treeWidget_4"));
        sizePolicy.setHeightForWidth(treeWidget_4->sizePolicy().hasHeightForWidth());
        treeWidget_4->setSizePolicy(sizePolicy);
        treeWidget_4->setMinimumSize(QSize(280, 200));
        treeWidget_4->setMaximumSize(QSize(400, 16777215));
        treeWidget_4->header()->setDefaultSectionSize(40);

        gridLayout_6->addWidget(treeWidget_4, 0, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        gridLayout_7 = new QGridLayout(tab_3);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        treeWidget_5 = new QTreeWidget(tab_3);
        treeWidget_5->setObjectName(QStringLiteral("treeWidget_5"));
        sizePolicy.setHeightForWidth(treeWidget_5->sizePolicy().hasHeightForWidth());
        treeWidget_5->setSizePolicy(sizePolicy);
        treeWidget_5->setMinimumSize(QSize(280, 200));
        treeWidget_5->setMaximumSize(QSize(400, 16777215));
        treeWidget_5->header()->setDefaultSectionSize(40);

        gridLayout_7->addWidget(treeWidget_5, 0, 0, 1, 1);

        tabWidget->addTab(tab_3, QString());

        verticalLayout->addWidget(tabWidget);

        label = new QLabel(groupBox_4);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(0, 150));
        label->setMaximumSize(QSize(16777215, 16777215));

        verticalLayout->addWidget(label);


        gridLayout_2->addWidget(groupBox_4, 1, 2, 1, 1);

        groupBox_6 = new QGroupBox(groupBox);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setMinimumSize(QSize(0, 100));
        gridLayout_8 = new QGridLayout(groupBox_6);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        pushButton_4 = new QPushButton(groupBox_6);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        pushButton_4->setMinimumSize(QSize(0, 0));
        pushButton_4->setFont(font);

        gridLayout_8->addWidget(pushButton_4, 4, 1, 1, 1);

        dateTimeEdit_2 = new QDateTimeEdit(groupBox_6);
        dateTimeEdit_2->setObjectName(QStringLiteral("dateTimeEdit_2"));
        QFont font1;
        font1.setPointSize(10);
        dateTimeEdit_2->setFont(font1);
        dateTimeEdit_2->setAlignment(Qt::AlignCenter);
        dateTimeEdit_2->setCalendarPopup(true);

        gridLayout_8->addWidget(dateTimeEdit_2, 3, 0, 1, 2);

        dateTimeEdit = new QDateTimeEdit(groupBox_6);
        dateTimeEdit->setObjectName(QStringLiteral("dateTimeEdit"));
        dateTimeEdit->setFont(font1);
        dateTimeEdit->setAlignment(Qt::AlignCenter);
        dateTimeEdit->setCalendarPopup(true);

        gridLayout_8->addWidget(dateTimeEdit, 2, 0, 1, 2);


        gridLayout_2->addWidget(groupBox_6, 2, 2, 1, 1);


        gridLayout->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_2 = new QGroupBox(ld_widget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(0, 80));
        horizontalLayout = new QHBoxLayout(groupBox_2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton_5 = new QPushButton(groupBox_2);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));
        pushButton_5->setMinimumSize(QSize(150, 55));
        QFont font2;
        font2.setPointSize(18);
        font2.setBold(true);
        font2.setWeight(75);
        pushButton_5->setFont(font2);

        horizontalLayout->addWidget(pushButton_5);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMinimumSize(QSize(60, 0));
        label_3->setFont(font2);

        horizontalLayout->addWidget(label_3);

        doubleSpinBox = new QDoubleSpinBox(groupBox_2);
        doubleSpinBox->setObjectName(QStringLiteral("doubleSpinBox"));
        doubleSpinBox->setMinimumSize(QSize(200, 50));
        doubleSpinBox->setFont(font2);
        doubleSpinBox->setMinimum(0.1);
        doubleSpinBox->setMaximum(100);
        doubleSpinBox->setSingleStep(0.1);

        horizontalLayout->addWidget(doubleSpinBox);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setMinimumSize(QSize(60, 0));
        label_4->setFont(font2);

        horizontalLayout->addWidget(label_4);

        lineEdit = new QLineEdit(groupBox_2);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setMinimumSize(QSize(200, 50));
        lineEdit->setFont(font2);

        horizontalLayout->addWidget(lineEdit);

        pushButton_2 = new QPushButton(groupBox_2);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setMinimumSize(QSize(150, 55));
        pushButton_2->setFont(font2);

        horizontalLayout->addWidget(pushButton_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        pushButton = new QPushButton(groupBox_2);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setMinimumSize(QSize(150, 55));
        pushButton->setFont(font2);

        horizontalLayout->addWidget(pushButton);


        gridLayout->addWidget(groupBox_2, 1, 0, 1, 1);


        retranslateUi(ld_widget);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ld_widget);
    } // setupUi

    void retranslateUi(QWidget *ld_widget)
    {
        ld_widget->setWindowTitle(QApplication::translate("ld_widget", "\344\272\244\346\230\223\347\256\241\347\220\206", 0));
        groupBox->setTitle(QString());
        groupBox_3->setTitle(QString());
        label_5->setText(QApplication::translate("ld_widget", "\345\275\223\345\211\215\347\273\204\345\217\267\357\274\232", 0));
        label_6->setText(QString());
        label_7->setText(QApplication::translate("ld_widget", "\347\273\204\345\217\267\351\200\211\346\213\251\357\274\232\357\274\2101-31\347\273\204\357\274\211", 0));
        pushButton_3->setText(QApplication::translate("ld_widget", "\351\200\211\346\213\251\347\273\204", 0));
        groupBox_5->setTitle(QString());
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(2, QApplication::translate("ld_widget", "\346\225\260\351\207\217", 0));
        ___qtreewidgetitem->setText(1, QApplication::translate("ld_widget", "\344\273\267\346\240\274", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("ld_widget", "\347\261\273\345\236\213", 0));
        QTreeWidgetItem *___qtreewidgetitem1 = treeWidget_2->headerItem();
        ___qtreewidgetitem1->setText(2, QApplication::translate("ld_widget", "\346\225\260\351\207\217", 0));
        ___qtreewidgetitem1->setText(1, QApplication::translate("ld_widget", "\344\273\267\346\240\274", 0));
        ___qtreewidgetitem1->setText(0, QApplication::translate("ld_widget", "\347\261\273\345\236\213", 0));
        label_2->setText(QApplication::translate("ld_widget", "\344\272\244\346\230\223\347\233\230\345\217\243", 0));
        label_8->setText(QString());
        groupBox_4->setTitle(QString());
        QTreeWidgetItem *___qtreewidgetitem2 = treeWidget_3->headerItem();
        ___qtreewidgetitem2->setText(3, QApplication::translate("ld_widget", "\347\261\273\345\236\213", 0));
        ___qtreewidgetitem2->setText(2, QApplication::translate("ld_widget", "\346\225\260\351\207\217", 0));
        ___qtreewidgetitem2->setText(1, QApplication::translate("ld_widget", "\344\273\267\346\240\274", 0));
        ___qtreewidgetitem2->setText(0, QApplication::translate("ld_widget", "\346\227\266\351\227\264", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("ld_widget", "\345\205\250\351\203\250", 0));
        QTreeWidgetItem *___qtreewidgetitem3 = treeWidget_4->headerItem();
        ___qtreewidgetitem3->setText(3, QApplication::translate("ld_widget", "\347\261\273\345\236\213", 0));
        ___qtreewidgetitem3->setText(2, QApplication::translate("ld_widget", "\346\225\260\351\207\217", 0));
        ___qtreewidgetitem3->setText(1, QApplication::translate("ld_widget", "\344\273\267\346\240\274", 0));
        ___qtreewidgetitem3->setText(0, QApplication::translate("ld_widget", "\346\227\266\351\227\264", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("ld_widget", "\347\263\273\347\273\237", 0));
        QTreeWidgetItem *___qtreewidgetitem4 = treeWidget_5->headerItem();
        ___qtreewidgetitem4->setText(3, QApplication::translate("ld_widget", "\347\261\273\345\236\213", 0));
        ___qtreewidgetitem4->setText(2, QApplication::translate("ld_widget", "\346\225\260\351\207\217", 0));
        ___qtreewidgetitem4->setText(1, QApplication::translate("ld_widget", "\344\273\267\346\240\274", 0));
        ___qtreewidgetitem4->setText(0, QApplication::translate("ld_widget", "\346\227\266\351\227\264", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("ld_widget", "\347\216\251\345\256\266", 0));
        label->setText(QString());
        groupBox_6->setTitle(QString());
        pushButton_4->setText(QApplication::translate("ld_widget", "\345\257\274\345\207\272\346\225\260\346\215\256", 0));
        groupBox_2->setTitle(QString());
        pushButton_5->setText(QApplication::translate("ld_widget", "\350\256\242\345\215\225\351\231\220\345\210\266", 0));
        label_3->setText(QApplication::translate("ld_widget", "\345\215\225\344\273\267", 0));
        label_4->setText(QApplication::translate("ld_widget", "\346\225\260\351\207\217", 0));
        pushButton_2->setText(QApplication::translate("ld_widget", "\345\212\240\345\215\225", 0));
        pushButton->setText(QApplication::translate("ld_widget", "\346\233\264\345\244\232", 0));
    } // retranslateUi

};

namespace Ui {
    class ld_widget: public Ui_ld_widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LD_WIDGET_H
