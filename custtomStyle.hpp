#include<QProxyStyle>
#include<QStyleOptionTab>
#include<QPainter>
#include<QTextOption>
#include<QPainterPath>

class CustomTabStyle:public QProxyStyle
{
public:
    CustomTabStyle(){}


    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
        const QSize &size, const QWidget *widget) const                   //设置Bar上面的内容大小
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == QStyle::CT_TabBarTab) {
            s.transpose();
            s.rwidth() = 150;
            s.rheight() = 50;
        }
        return s;
    }

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if (element == CE_TabBarTabLabel) {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
                QRect allRect = tab->rect;     //定义一个矩形区域
                allRect.setHeight(allRect.height()-5);
                allRect.setWidth(allRect.width()-10);
                allRect.setX(tab->rect.x() + 8);
                allRect.setY(tab->rect.y() + 5);

                QPainterPath path;
                path.addRoundedRect(allRect, 5, 5);
                painter->fillPath(path, QBrush(QColor("#ffffff")));//默认白色底
                painter->setPen("#000000");   //黑色字

                if (tab->state & QStyle::State_Selected || tab->state & State_MouseOver) {
                    painter->fillPath(path, QBrush(QColor("#1568FE")));     //灰色底
                    painter->setPen("#ffffff"); 
                }//选中或者悬浮 
                   


                QTextOption option;
                option.setAlignment(Qt::AlignCenter);     //使文字位置居中      
                painter->drawText(allRect, tab->text, option);
                return;
            }
        }

        if (element == CE_TabBarTab) {
            QProxyStyle::drawControl(element, option, painter, widget);
        }
    }


};
