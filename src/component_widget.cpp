#include <component_widget.h>
#include <mtdebug_print.h>
#include <toolkit.h>
#include <prefab_editor_dialog.h>

#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/HashMap.h>

#include <QPushButton>
#include <QTreeWidgetItem>
#include <QLineEdit>
#include <QTreeWidget>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QSpacerItem>

template<class T>
void Slot_Callback(Urho3D::Serializable * serz, Urho3D::VariantVector * nested_attrib_names, const Urho3D::String & attrib_name, const T & val)
{
    Urho3D::Variant attrib = serz->GetAttribute(attrib_name);
    Urho3D::Variant * cur_nested_val = &attrib;

    while (!nested_attrib_names->Empty())
    {
        Urho3D::Variant & cur_val = (*nested_attrib_names)[0];
        if (cur_nested_val->GetType() == Urho3D::VAR_VARIANTVECTOR)
        {
            int index = cur_val.GetInt();
            Urho3D::VariantVector * vv = cur_nested_val->GetVariantVectorPtr();
            cur_nested_val = &((*vv)[index]);
        }
        else if (cur_nested_val->GetType() == Urho3D::VAR_VARIANTMAP)
        {
            Urho3D::StringHash key = (*nested_attrib_names)[0].GetStringHash();
            Urho3D::VariantMap * vm = cur_nested_val->GetVariantMapPtr();
            cur_nested_val = &((*vm)[key]);
        }
        nested_attrib_names->Erase(0);
    }
    *cur_nested_val = val;
    serz->SetAttribute(attrib_name, attrib);
    bbtk.prefab_editor()->get_ui()->prefab_view->setFocus();
}

void create_tw_item(Urho3D::Serializable * ser,
                    const Urho3D::String & attrib_name,
                    const Urho3D::String & nested_name,
                    Urho3D::VariantVector & nested_attrib_names,
                    const Urho3D::Variant & value,
                    QTreeWidget * tw,
                    QTreeWidgetItem * parent)
{
    Urho3D::VariantType vtype = value.GetType();

    QTreeWidgetItem * tw_item = new QTreeWidgetItem;
    tw_item->setText(0, nested_name.CString());
    QWidget * item_widget = nullptr;

    switch (vtype)
    {
    case (Urho3D::VAR_VARIANTVECTOR):
    {
        const Urho3D::VariantVector & vv = value.GetVariantVector();
        tw_item->setText(
            0, tw_item->text(0) + QString(" (") + QString::number(vv.Size()) + QString(" Items)"));
        for (int i = 0; i < vv.Size(); ++i)
        {
            nested_attrib_names.Push(i);
            std::stringstream ss;
            ss << attrib_name.CString() << "[" << i << "]";
            create_tw_item(ser, attrib_name, ss.str().c_str(), nested_attrib_names, vv[i], tw, tw_item);
        }
        break;
    }
    case (Urho3D::VAR_VARIANTMAP):
    {
	    const Urho3D::VariantMap & vm = value.GetVariantMap();
	    tw_item->setText(0, tw_item->text(0) + QString(" (") + QString::number(vm.Size()) + QString(" Items)"));
	    auto vm_iter = vm.Begin();
	    while (vm_iter != vm.End())
	    {
		    nested_attrib_names.Push(vm_iter->first_);
		    create_tw_item(ser, attrib_name, vm_iter->first_.Reverse(), nested_attrib_names, vm_iter->second_, tw, tw_item);
		    ++vm_iter;
	    }
        break;
    }
    case (Urho3D::VAR_INT):
        item_widget = create_widget_item(ser, attrib_name, nested_attrib_names, value.GetInt());
        break;
    case (Urho3D::VAR_PTR):
        eout << "Cannot generate widget for pointer!";
        break;
    case (Urho3D::VAR_BOOL):
        item_widget = create_widget_item(ser, attrib_name, nested_attrib_names, value.GetBool());
        break;
    case (Urho3D::VAR_NONE):
        eout << "No variable type found for dialog";
        break;
    case (Urho3D::VAR_RECT):
        break;
    case (Urho3D::VAR_STRING):
        item_widget = create_widget_item(ser, attrib_name, nested_attrib_names, value.GetString());
        break;
    case (Urho3D::VAR_COLOR):
        break;
    case (Urho3D::VAR_FLOAT):
        item_widget = create_widget_item(ser, attrib_name, nested_attrib_names, value.GetFloat());
        break;
    case (Urho3D::VAR_INT64):
        item_widget = create_widget_item(ser, attrib_name, nested_attrib_names, value.GetInt());
        break;
    case (Urho3D::VAR_INTRECT):
        break;
    case (Urho3D::VAR_MATRIX3):
        break;
    case (Urho3D::VAR_MATRIX4):
        break;
    case (Urho3D::VAR_MATRIX3X4):
        break;
    case (Urho3D::VAR_VECTOR2):
        break;
    case (Urho3D::VAR_VECTOR3):
        break;
    case (Urho3D::VAR_VECTOR4):
        break;
    case (Urho3D::VAR_DOUBLE):
        item_widget = create_widget_item(ser, attrib_name, nested_attrib_names, value.GetDouble());
        break;
    case (Urho3D::VAR_INTVECTOR2):
        break;
    case (Urho3D::VAR_INTVECTOR3):
        break;
    case (Urho3D::VAR_STRINGVECTOR):
        break;
    case (Urho3D::VAR_QUATERNION):
        break;
    case (Urho3D::VAR_VOIDPTR):
        break;
    case (Urho3D::VAR_BUFFER):
        break;
    case (Urho3D::VAR_CUSTOM_HEAP):
        break;
    case (Urho3D::VAR_CUSTOM_STACK):
        break;
    case (Urho3D::VAR_RESOURCEREF):
        break;
    case (Urho3D::VAR_RESOURCEREFLIST):
        break;
    default:
        eout << "Warning - Cannot create dialog element for unknown type";
    }

    if (parent != nullptr)
        parent->addChild(tw_item);
    else
        tw->addTopLevelItem(tw_item);

    tw->setItemWidget(tw_item, 1, item_widget);
}

void build_tree_widget(Urho3D::Node * node, QTreeWidget * tw)
{
    if (node == nullptr)
        return;

    tw->clear();
    QStringList sl;
    sl.append("Attribute");
    sl.append("Value");
    tw->setHeaderLabels(sl);

    QTreeWidgetItem * node_dummy_root = new QTreeWidgetItem;
    node_dummy_root->setText(0, "Node Information");

    QFont fnt = node_dummy_root->font(0);
    fnt.setBold(true);
    fnt.setPointSize(fnt.pointSize() + 1);
    node_dummy_root->setFont(0, fnt);
    tw->addTopLevelItem(node_dummy_root);

    QTreeWidgetItem * node_info_root = new QTreeWidgetItem;

    QWidget * name_widget =
        create_widget_item(node, "Name", Urho3D::VariantVector(), node->GetName());
    QWidget * enabled_widget =
        create_widget_item(node, "Is Enabled", Urho3D::VariantVector(), node->IsEnabled());

    auto node_attribs = node->GetAttributes();
    if (node_attribs != nullptr)
    {
        for (int att_ind = 0; att_ind < node_attribs->Size(); ++att_ind)
        {
            Urho3D::String var_name = (*node_attribs)[att_ind].name_;
            if (var_name == "Name" || var_name == "Is Enabled")
                continue;
            Urho3D::Variant val = node->GetAttribute(var_name);
            Urho3D::VariantVector nested_names;
            create_tw_item(node, var_name, var_name, nested_names, val, tw, node_info_root);
        }
    }
    tw->addTopLevelItem(node_info_root);
    tw->setItemWidget(node_info_root, 0, name_widget);
    tw->setItemWidget(node_info_root, 1, enabled_widget);
    node_info_root->setExpanded(true);

    // Add the components to the tree widget
    const Urho3D::Vector<Urho3D::SharedPtr<Urho3D::Component>> & all_comps = node->GetComponents();
    for (int i = 0; i < all_comps.Size(); ++i)
    {
        QTreeWidgetItem * tw_item = new QTreeWidgetItem;
        Urho3D::Component * comp = all_comps[i];
        QCheckBox * cb_widget = new QCheckBox;
        tw_item->setText(0, comp->GetTypeName().CString());
        Urho3D::Variant enabled = comp->GetAttribute("Is Enabled");
        cb_widget->setText("");
        cb_widget->setChecked(enabled.GetBool());
        auto func = [=](int state) {
            comp->SetAttribute("Is Enabled", state == Qt::Checked);
            bbtk.prefab_editor()->get_ui()->prefab_view->setFocus();
        };
        QObject::connect(cb_widget, &QCheckBox::stateChanged, func);

        auto attribs = all_comps[i]->GetAttributes();
        if (attribs != nullptr)
        {
            for (int j = 0; j < attribs->Size(); ++j)
            {
                Urho3D::String var_name = (*attribs)[j].name_;
                if (((*attribs)[j].mode_ & Urho3D::AM_NOEDIT) == Urho3D::AM_NOEDIT ||
                    var_name == "Is Enabled")
                    continue;

                Urho3D::Variant val = all_comps[i]->GetAttribute(var_name);
                Urho3D::VariantVector nested_names;
                create_tw_item(all_comps[i], var_name, var_name, nested_names, val, tw, tw_item);
            }
            tw->addTopLevelItem(tw_item);
            tw->setItemWidget(tw_item, 1, cb_widget);
        }
    }
}

QWidget * create_widget_item(Urho3D::Serializable * serz,
                             Urho3D::String attrib_name,
                             Urho3D::VariantVector nested_attrib_names,
                             const Urho3D::String & value)
{
    QLineEdit * item = new QLineEdit;
    item->setText(value.CString());
    auto func = [=]() mutable {
        Slot_Callback(serz, &nested_attrib_names, attrib_name, qPrintable(item->text()));
    };
    QObject::connect(item, &QLineEdit::editingFinished, func);
    return item;
}

QWidget * create_widget_item(Urho3D::Serializable * serz,
                             Urho3D::String attrib_name,
                             Urho3D::VariantVector nested_attrib_names,
                             int value)
{
    QLineEdit * item = new QLineEdit;
    item->setText(QString::number(value));
    item->setValidator(new QIntValidator);

    auto func = [=]() mutable {
        Slot_Callback(serz, &nested_attrib_names, attrib_name, item->text().toInt());
    };

    QObject::connect(item, &QLineEdit::editingFinished, func);
    return item;
}

QWidget * create_widget_item(Urho3D::Serializable * serz,
                             Urho3D::String attrib_name,
                             Urho3D::VariantVector nested_attrib_names,
                             float value)
{
    QLineEdit * item = new QLineEdit;
    item->setText(QString::number(value, 'f', 2));
    item->setValidator(new QDoubleValidator);

    auto func = [=]() mutable {
        Slot_Callback(serz, &nested_attrib_names, attrib_name, item->text().toFloat());
    };

    QObject::connect(item, &QLineEdit::editingFinished, func);
    return item;
}

QWidget * create_widget_item(Urho3D::Serializable * serz,
                             Urho3D::String attrib_name,
                             Urho3D::VariantVector nested_attrib_names,
                             double value)
{
    QLineEdit * item = new QLineEdit;
    item->setText(QString::number(value, 'f', 4));
    item->setValidator(new QDoubleValidator);

    auto func = [=]() mutable {
        Slot_Callback(serz, &nested_attrib_names, attrib_name, item->text().toDouble());
    };

    QObject::connect(item, &QLineEdit::editingFinished, func);
    return item;
}

QWidget * create_widget_item(Urho3D::Serializable * serz,
                             Urho3D::String attrib_name,
                             Urho3D::VariantVector nested_attrib_names,
                             bool value)
{
    QCheckBox * item = new QCheckBox;
    item->setText("");
    item->setChecked(value);

    auto func = [=](int state) mutable {
        Slot_Callback(serz, &nested_attrib_names, attrib_name, state == Qt::Checked);
    };

    QObject::connect(item, &QCheckBox::stateChanged, func);
    return item;
}

Component_Widget::Component_Widget(QWidget * parent) : QWidget(nullptr)
{}

Component_Widget::~Component_Widget()
{}

void Component_Widget::setup_ui(Urho3D::Node * node)
{
    QVBoxLayout * layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    QTreeWidget * tw = new QTreeWidget;
    tw->setHeaderHidden(true);
    tw->setStyleSheet(TW_STYLE);
    tw->setSelectionMode(QAbstractItemView::NoSelection);
    build_tree_widget(node, tw);
    tw->header()->setStretchLastSection(false);
    tw->header()->setSectionResizeMode(0,QHeaderView::Stretch);
    tw->header()->setSectionResizeMode(1,QHeaderView::Stretch);
    layout->addWidget(tw);
    setLayout(layout);
}
