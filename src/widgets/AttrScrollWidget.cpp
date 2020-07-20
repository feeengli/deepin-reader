#include "AttrScrollWidget.h"

#include <QDebug>
#include <DFontSizeManager>
#include <DTextBrowser>
#include <DLabel>
#include <QGridLayout>

#include "Utils.h"
#include "DocSheet.h"
#include "widgets/WordWrapLabel.h"

DWIDGET_USE_NAMESPACE
#define LAEBL_TEXT_WIDTH   165
AttrScrollWidget::AttrScrollWidget(DocSheet *sheet, DWidget *parent)
    : DFrame(parent)
    , m_leftminwidth(60)
{
    setFrameShape(QFrame::NoFrame);

    installEventFilter(this);
    auto gridLayout = new QGridLayout;
    gridLayout->setContentsMargins(0, 6, 0, 6);
    gridLayout->setSpacing(3);

    deepin_reader::FileInfo fileInfo;
    sheet->docBasicInfo(fileInfo);
    QLocale locale;
    QFontMetrics fm(font());
    //用最长字符来计算左侧最小宽度
    if (locale.language() == QLocale::English) {
        m_leftminwidth = fm.horizontalAdvance(("Time modified"));
    } else if (locale.language() == QLocale::Chinese) {
        m_leftminwidth = fm.horizontalAdvance("页面大小");
    }

    createLabel(gridLayout, 0, tr("Location"), fileInfo.filePath);
    createLabel(gridLayout, 1, tr("Subject"), fileInfo.theme);
    createLabel(gridLayout, 2, tr("Author"), fileInfo.auther);
    createLabel(gridLayout, 3, tr("Keywords"), fileInfo.keyword);
    createLabel(gridLayout, 4, tr("Producer"), fileInfo.producter);
    createLabel(gridLayout, 5, tr("Creator"), fileInfo.creater);
    createLabel(gridLayout, 6, tr("Time created"), fileInfo.createTime);
    createLabel(gridLayout, 7, tr("Time modified"), fileInfo.changeTime);
    createLabel(gridLayout, 8, tr("Format"), fileInfo.format);
    createLabel(gridLayout, 9, tr("Pages"), QString("%1").arg(fileInfo.numpages));
    createLabel(gridLayout, 10, tr("Optimized"), fileInfo.optimization);
    createLabel(gridLayout, 11, tr("Security"), fileInfo.safe);
    QString sPaperSize = QString("%1*%2").arg(fileInfo.width).arg(fileInfo.height);
    createLabel(gridLayout, 12, tr("Page size"), sPaperSize);
    createLabel(gridLayout, 13, tr("File size"), Utils::getInputDataSize(static_cast<qint64>(fileInfo.size)));

    auto vLayout = new QVBoxLayout;
    vLayout->setContentsMargins(10, 10, 10, 10);

    auto basicLabel = new DLabel(tr("Basic info"));
    DFontSizeManager::instance()->bind(basicLabel, DFontSizeManager::T6);
    vLayout->addWidget(basicLabel);

    vLayout->addItem(gridLayout);

    vLayout->addStretch(1);

    this->setLayout(vLayout);
}

void AttrScrollWidget::createLabel(QGridLayout *layout, const int &index, const QString &objName, const QString &sData)
{
    DLabel *label = new DLabel(objName, this);
    DFontSizeManager::instance()->bind(label, DFontSizeManager::T8);
    label->setAlignment(Qt::AlignTop);
    label->setMaximumWidth(140);
    label->setMinimumWidth(70 + 20);
    layout->addWidget(label, index, 0);

    QString text = sData.isEmpty() ? tr("Unknown") : sData;
    WordWrapLabel *labelText = new WordWrapLabel(this);
    DFontSizeManager::instance()->bind(labelText, DFontSizeManager::T8);
    labelText->setFixedWidth(LAEBL_TEXT_WIDTH);
    labelText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    labelText->setText(text);
    layout->addWidget(labelText, index, 1);
}

void AttrScrollWidget::createLabel(QGridLayout *layout, const int &index, const QString &objName, const QDateTime &sData)
{
    DLabel *label = new DLabel(objName, this);
    DFontSizeManager::instance()->bind(label, DFontSizeManager::T8);
    label->setAlignment(Qt::AlignTop);
    layout->addWidget(label, index, 0);

    DLabel *labelText = new DLabel(this);
    DFontSizeManager::instance()->bind(labelText, DFontSizeManager::T8);
    labelText->setMaximumWidth(260);
    QString strText = sData.toString("yyyy/MM/dd HH:mm:ss");
    labelText->setText(strText);
    labelText->setAlignment(Qt::AlignTop);
    labelText->setWordWrap(true);
    layout->addWidget(labelText, index, 1);
}

void AttrScrollWidget::createLabel(QGridLayout *layout, const int &index, const QString &objName, const bool &bData)
{
    DLabel *label = new DLabel(objName, this);
    DFontSizeManager::instance()->bind(label, DFontSizeManager::T8);
    label->setAlignment(Qt::AlignTop);
    layout->addWidget(label, index, 0);

    DLabel *labelText = new DLabel(this);
    DFontSizeManager::instance()->bind(labelText, DFontSizeManager::T8);
    labelText->setText(bData ? tr("Yes") : tr("No"));
    labelText->setAlignment(Qt::AlignTop);
    layout->addWidget(labelText, index, 1);
}
