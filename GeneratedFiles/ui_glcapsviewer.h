/********************************************************************************
** Form generated from reading UI file 'glcapsviewer.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GLCAPSVIEWER_H
#define UI_GLCAPSVIEWER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_glcapsviewerClass
{
public:
    QAction *actionRefresh;
    QAction *actionUpload;
    QAction *actionDatabase;
    QAction *actionSettings;
    QAction *actionAbout;
    QAction *actionExit;
    QAction *actionSave_xml;
    QAction *actionDevice;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_2;
    QTabWidget *tabWidget;
    QWidget *tabDevice;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QLabel *labelDescription;
    QLabel *labelReportPresent;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *treeWidget;
    QTabWidget *tabWidgetDevice;
    QWidget *tab;
    QVBoxLayout *verticalLayout_4;
    QTreeWidget *treeWidgetExtensions;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_5;
    QListWidget *listWidgetCompressedFormats;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_6;
    QTreeWidget *treeWidgetInternalFormats;
    QWidget *tabDatabase;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButtonRefreshDataBase;
    QHBoxLayout *horizontalLayout_4;
    QListWidget *listWidgetDatabaseDevices;
    QVBoxLayout *verticalLayout_3;
    QLabel *label;
    QComboBox *comboBoxDeviceVersions;
    QLabel *labelDatabaseDeviceImplementation;
    QTableWidget *tableWidgetDatabaseDeviceReport;
    QLabel *labelDatabaseDeviceExtensions;
    QListWidget *listWidgetDatabaseDeviceExtensions;
    QWidget *tabCompare;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *glcapsviewerClass)
    {
        if (glcapsviewerClass->objectName().isEmpty())
            glcapsviewerClass->setObjectName(QStringLiteral("glcapsviewerClass"));
        glcapsviewerClass->resize(997, 817);
        QIcon icon;
        icon.addFile(QStringLiteral(":/glcapsviewer/Resources/glCapsViewer.ico"), QSize(), QIcon::Normal, QIcon::Off);
        glcapsviewerClass->setWindowIcon(icon);
        glcapsviewerClass->setLayoutDirection(Qt::LeftToRight);
        glcapsviewerClass->setStyleSheet(QLatin1String("#glcapsviewerClass {\n"
"background: white;\n"
"}\n"
"\n"
"#centralWidget {\n"
"background: white;\n"
"}\n"
"\n"
"#mainToolBar {\n"
"background:white;\n"
"color:black;\n"
"}"));
        actionRefresh = new QAction(glcapsviewerClass);
        actionRefresh->setObjectName(QStringLiteral("actionRefresh"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/glcapsviewer/Resources/refresh48.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRefresh->setIcon(icon1);
        actionUpload = new QAction(glcapsviewerClass);
        actionUpload->setObjectName(QStringLiteral("actionUpload"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/glcapsviewer/Resources/upload48.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUpload->setIcon(icon2);
        actionDatabase = new QAction(glcapsviewerClass);
        actionDatabase->setObjectName(QStringLiteral("actionDatabase"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/glcapsviewer/Resources/browse48.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDatabase->setIcon(icon3);
        actionSettings = new QAction(glcapsviewerClass);
        actionSettings->setObjectName(QStringLiteral("actionSettings"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/glcapsviewer/Resources/settings48.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSettings->setIcon(icon4);
        actionAbout = new QAction(glcapsviewerClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/glcapsviewer/Resources/about48.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAbout->setIcon(icon5);
        actionExit = new QAction(glcapsviewerClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/glcapsviewer/Resources/close48.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon6);
        actionSave_xml = new QAction(glcapsviewerClass);
        actionSave_xml->setObjectName(QStringLiteral("actionSave_xml"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/glcapsviewer/Resources/save48.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_xml->setIcon(icon7);
        actionDevice = new QAction(glcapsviewerClass);
        actionDevice->setObjectName(QStringLiteral("actionDevice"));
        actionDevice->setEnabled(false);
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/glcapsviewer/Resources/device48.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDevice->setIcon(icon8);
        centralWidget = new QWidget(glcapsviewerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout_2 = new QHBoxLayout(centralWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setDocumentMode(false);
        tabWidget->setTabsClosable(false);
        tabWidget->setTabBarAutoHide(false);
        tabDevice = new QWidget();
        tabDevice->setObjectName(QStringLiteral("tabDevice"));
        horizontalLayout_3 = new QHBoxLayout(tabDevice);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        labelDescription = new QLabel(tabDevice);
        labelDescription->setObjectName(QStringLiteral("labelDescription"));
        labelDescription->setStyleSheet(QStringLiteral("font: 75 11pt;"));
        labelDescription->setFrameShape(QFrame::NoFrame);
        labelDescription->setWordWrap(true);

        verticalLayout->addWidget(labelDescription);

        labelReportPresent = new QLabel(tabDevice);
        labelReportPresent->setObjectName(QStringLiteral("labelReportPresent"));
        labelReportPresent->setStyleSheet(QStringLiteral("font: 10pt;"));
        labelReportPresent->setFrameShape(QFrame::NoFrame);

        verticalLayout->addWidget(labelReportPresent);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        treeWidget = new QTreeWidget(tabDevice);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setStyleSheet(QStringLiteral("QTreeView::item { height: 24px;}"));
        treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeWidget->setAlternatingRowColors(true);
        treeWidget->setIndentation(10);
        treeWidget->header()->setDefaultSectionSize(200);
        treeWidget->header()->setStretchLastSection(true);

        horizontalLayout->addWidget(treeWidget);

        tabWidgetDevice = new QTabWidget(tabDevice);
        tabWidgetDevice->setObjectName(QStringLiteral("tabWidgetDevice"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayout_4 = new QVBoxLayout(tab);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        treeWidgetExtensions = new QTreeWidget(tab);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeWidgetExtensions->setHeaderItem(__qtreewidgetitem);
        treeWidgetExtensions->setObjectName(QStringLiteral("treeWidgetExtensions"));
        treeWidgetExtensions->setStyleSheet(QStringLiteral("QTreeView::item { height: 24px;}"));
        treeWidgetExtensions->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeWidgetExtensions->setAlternatingRowColors(true);
        treeWidgetExtensions->header()->setVisible(false);

        verticalLayout_4->addWidget(treeWidgetExtensions);

        tabWidgetDevice->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout_5 = new QVBoxLayout(tab_2);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        listWidgetCompressedFormats = new QListWidget(tab_2);
        listWidgetCompressedFormats->setObjectName(QStringLiteral("listWidgetCompressedFormats"));
        listWidgetCompressedFormats->setAlternatingRowColors(true);

        verticalLayout_5->addWidget(listWidgetCompressedFormats);

        tabWidgetDevice->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        verticalLayout_6 = new QVBoxLayout(tab_3);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        treeWidgetInternalFormats = new QTreeWidget(tab_3);
        treeWidgetInternalFormats->setObjectName(QStringLiteral("treeWidgetInternalFormats"));
        treeWidgetInternalFormats->setStyleSheet(QStringLiteral("QTreeView::item { height: 24px;}"));
        treeWidgetInternalFormats->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeWidgetInternalFormats->setAlternatingRowColors(true);
        treeWidgetInternalFormats->setIndentation(10);
        treeWidgetInternalFormats->header()->setDefaultSectionSize(250);
        treeWidgetInternalFormats->header()->setStretchLastSection(true);

        verticalLayout_6->addWidget(treeWidgetInternalFormats);

        tabWidgetDevice->addTab(tab_3, QString());

        horizontalLayout->addWidget(tabWidgetDevice);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_3->addLayout(verticalLayout);

        QIcon icon9;
        icon9.addFile(QStringLiteral(":/glcapsviewer/Resources/gpu32.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tabDevice, icon9, QString());
        tabDatabase = new QWidget();
        tabDatabase->setObjectName(QStringLiteral("tabDatabase"));
        horizontalLayout_5 = new QHBoxLayout(tabDatabase);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        pushButtonRefreshDataBase = new QPushButton(tabDatabase);
        pushButtonRefreshDataBase->setObjectName(QStringLiteral("pushButtonRefreshDataBase"));
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/glcapsviewer/Resources/refresh16.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonRefreshDataBase->setIcon(icon10);
        pushButtonRefreshDataBase->setFlat(false);

        verticalLayout_2->addWidget(pushButtonRefreshDataBase, 0, Qt::AlignLeft);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        listWidgetDatabaseDevices = new QListWidget(tabDatabase);
        new QListWidgetItem(listWidgetDatabaseDevices);
        listWidgetDatabaseDevices->setObjectName(QStringLiteral("listWidgetDatabaseDevices"));
        listWidgetDatabaseDevices->setAlternatingRowColors(true);

        horizontalLayout_4->addWidget(listWidgetDatabaseDevices);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label = new QLabel(tabDatabase);
        label->setObjectName(QStringLiteral("label"));
        label->setStyleSheet(QLatin1String("font: 75;\n"
"color: rgb(0, 0, 255);"));
        label->setFrameShape(QFrame::NoFrame);
        label->setFrameShadow(QFrame::Plain);

        verticalLayout_3->addWidget(label);

        comboBoxDeviceVersions = new QComboBox(tabDatabase);
        comboBoxDeviceVersions->setObjectName(QStringLiteral("comboBoxDeviceVersions"));

        verticalLayout_3->addWidget(comboBoxDeviceVersions);

        labelDatabaseDeviceImplementation = new QLabel(tabDatabase);
        labelDatabaseDeviceImplementation->setObjectName(QStringLiteral("labelDatabaseDeviceImplementation"));
        labelDatabaseDeviceImplementation->setStyleSheet(QLatin1String("font: 75;\n"
"color: rgb(0, 0, 255);"));
        labelDatabaseDeviceImplementation->setFrameShape(QFrame::NoFrame);

        verticalLayout_3->addWidget(labelDatabaseDeviceImplementation);

        tableWidgetDatabaseDeviceReport = new QTableWidget(tabDatabase);
        if (tableWidgetDatabaseDeviceReport->columnCount() < 2)
            tableWidgetDatabaseDeviceReport->setColumnCount(2);
        QFont font;
        font.setBold(false);
        font.setUnderline(false);
        font.setWeight(50);
        font.setKerning(true);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setFont(font);
        tableWidgetDatabaseDeviceReport->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QFont font1;
        font1.setBold(false);
        font1.setWeight(50);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        __qtablewidgetitem1->setFont(font1);
        tableWidgetDatabaseDeviceReport->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableWidgetDatabaseDeviceReport->setObjectName(QStringLiteral("tableWidgetDatabaseDeviceReport"));
        tableWidgetDatabaseDeviceReport->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidgetDatabaseDeviceReport->setAlternatingRowColors(true);
        tableWidgetDatabaseDeviceReport->setShowGrid(false);
        tableWidgetDatabaseDeviceReport->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_3->addWidget(tableWidgetDatabaseDeviceReport);

        labelDatabaseDeviceExtensions = new QLabel(tabDatabase);
        labelDatabaseDeviceExtensions->setObjectName(QStringLiteral("labelDatabaseDeviceExtensions"));
        labelDatabaseDeviceExtensions->setStyleSheet(QLatin1String("font: 75;\n"
"color: rgb(0, 0, 255);"));
        labelDatabaseDeviceExtensions->setFrameShape(QFrame::NoFrame);

        verticalLayout_3->addWidget(labelDatabaseDeviceExtensions);

        listWidgetDatabaseDeviceExtensions = new QListWidget(tabDatabase);
        listWidgetDatabaseDeviceExtensions->setObjectName(QStringLiteral("listWidgetDatabaseDeviceExtensions"));
        listWidgetDatabaseDeviceExtensions->setAlternatingRowColors(true);

        verticalLayout_3->addWidget(listWidgetDatabaseDeviceExtensions);


        horizontalLayout_4->addLayout(verticalLayout_3);


        verticalLayout_2->addLayout(horizontalLayout_4);


        horizontalLayout_5->addLayout(verticalLayout_2);

        QIcon icon11;
        icon11.addFile(QStringLiteral(":/glcapsviewer/Resources/db24.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tabDatabase, icon11, QString());
        tabCompare = new QWidget();
        tabCompare->setObjectName(QStringLiteral("tabCompare"));
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/glcapsviewer/Resources/compare24.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tabCompare, icon12, QString());

        horizontalLayout_2->addWidget(tabWidget);

        glcapsviewerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(glcapsviewerClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 997, 21));
        glcapsviewerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(glcapsviewerClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        mainToolBar->setMovable(true);
        mainToolBar->setIconSize(QSize(64, 24));
        mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        mainToolBar->setFloatable(false);
        glcapsviewerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);

        mainToolBar->addAction(actionRefresh);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionUpload);
        mainToolBar->addAction(actionSave_xml);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionDevice);
        mainToolBar->addAction(actionDatabase);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionSettings);
        mainToolBar->addAction(actionAbout);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionExit);

        retranslateUi(glcapsviewerClass);

        tabWidget->setCurrentIndex(0);
        tabWidgetDevice->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(glcapsviewerClass);
    } // setupUi

    void retranslateUi(QMainWindow *glcapsviewerClass)
    {
        glcapsviewerClass->setWindowTitle(QApplication::translate("glcapsviewerClass", "glcapsviewer 1.0 - \302\251 2011-2015 by Sascha Willems (www.saschawillems.de)", 0));
        actionRefresh->setText(QApplication::translate("glcapsviewerClass", "Refresh", 0));
#ifndef QT_NO_TOOLTIP
        actionRefresh->setToolTip(QApplication::translate("glcapsviewerClass", "Refrehs OpenGL report", 0));
#endif // QT_NO_TOOLTIP
        actionUpload->setText(QApplication::translate("glcapsviewerClass", "Upload", 0));
#ifndef QT_NO_TOOLTIP
        actionUpload->setToolTip(QApplication::translate("glcapsviewerClass", "Upload OpenGL report to online database", 0));
#endif // QT_NO_TOOLTIP
        actionDatabase->setText(QApplication::translate("glcapsviewerClass", "Database", 0));
#ifndef QT_NO_TOOLTIP
        actionDatabase->setToolTip(QApplication::translate("glcapsviewerClass", "Display OpenGL online database in default browser", 0));
#endif // QT_NO_TOOLTIP
        actionSettings->setText(QApplication::translate("glcapsviewerClass", "Settings", 0));
#ifndef QT_NO_TOOLTIP
        actionSettings->setToolTip(QApplication::translate("glcapsviewerClass", "Application settings (Proxy)", 0));
#endif // QT_NO_TOOLTIP
        actionAbout->setText(QApplication::translate("glcapsviewerClass", "About", 0));
#ifndef QT_NO_TOOLTIP
        actionAbout->setToolTip(QApplication::translate("glcapsviewerClass", "Application information", 0));
#endif // QT_NO_TOOLTIP
        actionExit->setText(QApplication::translate("glcapsviewerClass", "Exit", 0));
#ifndef QT_NO_TOOLTIP
        actionExit->setToolTip(QApplication::translate("glcapsviewerClass", "Exit OpenGL caps viewer", 0));
#endif // QT_NO_TOOLTIP
        actionSave_xml->setText(QApplication::translate("glcapsviewerClass", "Save xml", 0));
#ifndef QT_NO_TOOLTIP
        actionSave_xml->setToolTip(QApplication::translate("glcapsviewerClass", "Export current OpenGL report to xml", 0));
#endif // QT_NO_TOOLTIP
        actionDevice->setText(QApplication::translate("glcapsviewerClass", "Device", 0));
#ifndef QT_NO_TOOLTIP
        actionDevice->setToolTip(QApplication::translate("glcapsviewerClass", "Show device in database", 0));
#endif // QT_NO_TOOLTIP
        labelDescription->setText(QApplication::translate("glcapsviewerClass", "Getting OpenGL implementation details...", 0));
        labelReportPresent->setText(QApplication::translate("glcapsviewerClass", "TextLabel", 0));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("glcapsviewerClass", "value", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("glcapsviewerClass", "key", 0));
        tabWidgetDevice->setTabText(tabWidgetDevice->indexOf(tab), QApplication::translate("glcapsviewerClass", "Extensions", 0));
        tabWidgetDevice->setTabText(tabWidgetDevice->indexOf(tab_2), QApplication::translate("glcapsviewerClass", "Compressed texture formats", 0));
        QTreeWidgetItem *___qtreewidgetitem1 = treeWidgetInternalFormats->headerItem();
        ___qtreewidgetitem1->setText(1, QApplication::translate("glcapsviewerClass", "value", 0));
        ___qtreewidgetitem1->setText(0, QApplication::translate("glcapsviewerClass", "key", 0));
        tabWidgetDevice->setTabText(tabWidgetDevice->indexOf(tab_3), QApplication::translate("glcapsviewerClass", "Internal formats", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabDevice), QApplication::translate("glcapsviewerClass", "Your device", 0));
        pushButtonRefreshDataBase->setText(QApplication::translate("glcapsviewerClass", "Refresh", 0));

        const bool __sortingEnabled = listWidgetDatabaseDevices->isSortingEnabled();
        listWidgetDatabaseDevices->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = listWidgetDatabaseDevices->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("glcapsviewerClass", "Please refresh...", 0));
        listWidgetDatabaseDevices->setSortingEnabled(__sortingEnabled);

        label->setText(QApplication::translate("glcapsviewerClass", "Report version", 0));
        labelDatabaseDeviceImplementation->setText(QApplication::translate("glcapsviewerClass", "Implementation", 0));
        QTableWidgetItem *___qtablewidgetitem = tableWidgetDatabaseDeviceReport->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("glcapsviewerClass", "Capability", 0));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidgetDatabaseDeviceReport->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("glcapsviewerClass", "Value", 0));
        labelDatabaseDeviceExtensions->setText(QApplication::translate("glcapsviewerClass", "Extensions", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabDatabase), QApplication::translate("glcapsviewerClass", "Database", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabCompare), QApplication::translate("glcapsviewerClass", "Compare", 0));
    } // retranslateUi

};

namespace Ui {
    class glcapsviewerClass: public Ui_glcapsviewerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GLCAPSVIEWER_H
