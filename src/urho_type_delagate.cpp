#include <QItemEditorFactory>
#include <urho_type_delagate.h>
#include <mtdebug_print.h>

Urho_Type_Delagate::Urho_Type_Delagate(QWidget * parent) : QStyledItemDelegate(parent)
{
    int cb_desc_type_id = qRegisterMetaType<Selected_Attrib_Desc>("Selected_Attrib_Desc");
}

Urho_Type_Delagate::~Urho_Type_Delagate()
{}

void Urho_Type_Delagate::setModelData(QWidget * editor,
                                      QAbstractItemModel * model,
                                      const QModelIndex & index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}

void Urho_Type_Delagate::updateEditorGeometry(QWidget * editor,
                                              const QStyleOptionViewItem & option,
                                              const QModelIndex & index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void Urho_Type_Delagate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

QWidget * Urho_Type_Delagate::createEditor(QWidget * parent,
                                           const QStyleOptionViewItem & option,
                                           const QModelIndex & index) const
{
    dout << "Trying to create editor!";
    if (index.data().canConvert<Selected_Attrib_Desc>())
    {
        auto data = index.data().value<Selected_Attrib_Desc>();
        Attrib_Edit_Widget * edit_widget = new Attrib_Edit_Widget(data.attrib_info.type_, parent);
        return edit_widget;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}