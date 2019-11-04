#pragma once

#include <QStyledItemDelegate>
#include <urho_attrib_edit_widget.h>

class Urho_Type_Delagate : public QStyledItemDelegate
{
    Q_OBJECT

  public:
    Urho_Type_Delagate(QWidget * parent = nullptr);

    ~Urho_Type_Delagate();

    QWidget * createEditor(QWidget * parent,
                           const QStyleOptionViewItem & option,
                           const QModelIndex & index) const;

    void setEditorData(QWidget * editor, const QModelIndex & index) const;

    void updateEditorGeometry(QWidget * editor,
                              const QStyleOptionViewItem & option,
                              const QModelIndex & index) const;

    void
    setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
};