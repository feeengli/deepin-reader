#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <DStackedWidget>
#include <DWidget>
#include <QVBoxLayout>

#include "FileAttrWidget.h"
#include "HomeWidget.h"
#include "MainShowSplitter.h"

#include "header/CustomWidget.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The MainWidget class
 * @brief   主窗体显示
 */

class MainWidget : public CustomWidget
{
    Q_OBJECT
public:
    MainWidget(CustomWidget *parent = nullptr);
    ~MainWidget() override;

protected:
    void initWidget() override;

private slots:
    void showFileSelected(const QStringList) const;

private:
    void initWidgets();
    void openFileFolder();
    void showFileAttr();

private:
    QVBoxLayout *m_centralLayout = nullptr;

    DStackedWidget *m_pStackedWidget = nullptr; //  栈式 显示
    HomeWidget *m_pHomeWidget = nullptr; //  选择文件
    MainShowSplitter *m_pMainShowSplitter = nullptr; //  文件显示 和 操作
    FileAttrWidget *m_pAttrWidget = nullptr;

    // IObserver interface
public:
    int update(const int &, const QString &) override;
};

#endif // MAINWIDGET_H
