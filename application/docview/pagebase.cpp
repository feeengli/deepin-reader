#include "pagebase.h"

PageBase::PageBase(QWidget *parent)
    : QLabel(parent)
{
    setMouseTracking(true);
    setAlignment(Qt::AlignCenter);
}
