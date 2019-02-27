#include <component_widget.h>
#include <mtdebug_print.h>

#include <toolkit.h>
#include <ui_toolkit.h>

#include <limits>

#include <Urho3D/Container/HashMap.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QSpinBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

Urho3D::Variant * Get_Attrib_Variant(Urho3D::Variant & attrib_value,
                                     Urho3D::VariantVector & nested_attrib_names)
{
    Urho3D::Variant * cur_nested_val = &attrib_value;
    while (!nested_attrib_names.Empty())
    {
        Urho3D::Variant & cur_val = nested_attrib_names[0];
        if (cur_nested_val->GetType() == Urho3D::VAR_VARIANTVECTOR)
        {
            int index = cur_val.GetInt();
            Urho3D::VariantVector * vv = cur_nested_val->GetVariantVectorPtr();
            cur_nested_val = &((*vv)[index]);
        }
        else if (cur_nested_val->GetType() == Urho3D::VAR_VARIANTMAP)
        {
            Urho3D::StringHash key = nested_attrib_names[0].GetStringHash();
            Urho3D::VariantMap * vm = cur_nested_val->GetVariantMapPtr();
            cur_nested_val = &((*vm)[key]);
        }
        nested_attrib_names.Erase(0);
    }
    return cur_nested_val;
}

template<class T>
void Slot_Callback(Urho3D::Serializable * serz,
                   Urho3D::VariantVector nested_attrib_names,
                   const Urho3D::String & attrib_name,
                   const T & val)
{
    Urho3D::Variant attrib = serz->GetAttribute(attrib_name);
    Urho3D::Variant * final_attrib = Get_Attrib_Variant(attrib, nested_attrib_names);
    *final_attrib = val;
    serz->SetAttribute(attrib_name, attrib);
    //bbtk.prefab_editor()->get_ui()->prefab_view->setFocus();
}

void Component_Widget::update_tw_from_node()
{
    auto iter = updaters.begin();
    while (iter != updaters.end())
    {
        Urho3D::Variant attrib = iter.key()->GetAttribute(iter->attrib_name);
        Urho3D::Variant * nested_val = Get_Attrib_Variant(attrib, iter->nested_names);
        iter->set_widget_value(*nested_val);
        ++iter;
    }
}

void Component_Widget::create_tw_item(Urho3D::Serializable * ser,
                                      const Urho3D::String & attrib_name,
                                      const Urho3D::String & nested_name,
                                      Urho3D::VariantVector & nested_attrib_names,
                                      const Urho3D::Variant & value,
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
            create_tw_item(ser, attrib_name, ss.str().c_str(), nested_attrib_names, vv[i], tw_item);
        }
        break;
    }
    case (Urho3D::VAR_VARIANTMAP):
    {
        const Urho3D::VariantMap & vm = value.GetVariantMap();
        tw_item->setText(
            0, tw_item->text(0) + QString(" (") + QString::number(vm.Size()) + QString(" Items)"));
        auto vm_iter = vm.Begin();
        while (vm_iter != vm.End())
        {
            nested_attrib_names.Push(vm_iter->first_);
            create_tw_item(ser,
                           attrib_name,
                           vm_iter->first_.Reverse(),
                           nested_attrib_names,
                           vm_iter->second_,
                           tw_item);
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
        item_widget = create_widget_item(ser, attrib_name, nested_attrib_names, value.GetIntRect());
        break;
    case (Urho3D::VAR_MATRIX3):
        break;
    case (Urho3D::VAR_MATRIX4):
        break;
    case (Urho3D::VAR_MATRIX3X4):
        break;
    case (Urho3D::VAR_VECTOR2):
        item_widget = create_widget_item(ser, attrib_name, nested_attrib_names, value.GetVector2());
        break;
    case (Urho3D::VAR_VECTOR3):
        item_widget = create_widget_item(ser, attrib_name, nested_attrib_names, value.GetVector3());
        break;
    case (Urho3D::VAR_VECTOR4):
        break;
    case (Urho3D::VAR_DOUBLE):
        item_widget = create_widget_item(ser, attrib_name, nested_attrib_names, value.GetDouble());
        break;
    case (Urho3D::VAR_INTVECTOR2):
        item_widget = create_widget_item(ser, attrib_name, nested_attrib_names, value.GetIntVector2());
        break;
    case (Urho3D::VAR_INTVECTOR3):
        item_widget =
            create_widget_item(ser, attrib_name, nested_attrib_names, value.GetIntVector3());
        break;
    case (Urho3D::VAR_STRINGVECTOR):
    {
        const Urho3D::StringVector & vv = value.GetStringVector();
        tw_item->setText(
            0, tw_item->text(0) + QString(" (") + QString::number(vv.Size()) + QString(" Items)"));
        for (int i = 0; i < vv.Size(); ++i)
        {
            nested_attrib_names.Push(i);
            std::stringstream ss;
            ss << attrib_name.CString() << "[" << i << "]";
            create_tw_item(ser, attrib_name, ss.str().c_str(), nested_attrib_names, vv[i], tw_item);
        }
        break;
    }
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
    {
        const Urho3D::ResourceRefList & vv = value.GetResourceRefList();
        tw_item->setText(0, tw_item->text(0) + QString(" (") + QString::number(vv.names_.Size()) + QString(" Items)"));
        for (int i = 0; i < vv.names_.Size(); ++i)
        {
            Urho3D::ResourceRef ref;
            ref.name_ = vv.names_[i];
            ref.type_ = vv.type_;
            nested_attrib_names.Push(i);
            std::stringstream ss;
            ss << attrib_name.CString() << "[" << i << "]";
            create_tw_item(ser, attrib_name, ss.str().c_str(), nested_attrib_names, ref, tw_item);
        }
        break;
    }
    default:
        eout << "Warning - Cannot create dialog element for unknown type";
    }

    if (parent != nullptr)
        parent->addChild(tw_item);
    else
        tw_->addTopLevelItem(tw_item);

    tw_->setItemWidget(tw_item, 1, item_widget);
}

void Component_Widget::build_tree_widget()
{
    if (node_ == nullptr)
        return;

    tw_->clear();
    tw_->setColumnCount(2);
    updaters.clear();

    QTreeWidgetItem * node_dummy_root = new QTreeWidgetItem;
    node_dummy_root->setText(0, "Node Information");

    QFont fnt = node_dummy_root->font(0);
    fnt.setBold(true);
    fnt.setPointSize(fnt.pointSize() + 1);
    node_dummy_root->setFont(0, fnt);
    tw_->addTopLevelItem(node_dummy_root);

    QTreeWidgetItem * node_info_root = new QTreeWidgetItem;

    QWidget * name_widget =
        create_widget_item(node_, "Name", Urho3D::VariantVector(), node_->GetName());
    QWidget * enabled_widget =
        create_widget_item(node_, "Is Enabled", Urho3D::VariantVector(), node_->IsEnabled());

    auto node_attribs = node_->GetAttributes();
    if (node_attribs != nullptr)
    {
        for (int att_ind = 0; att_ind < node_attribs->Size(); ++att_ind)
        {
            Urho3D::String var_name = (*node_attribs)[att_ind].name_;
            if (var_name == "Name" || var_name == "Is Enabled")
                continue;
            Urho3D::Variant val = node_->GetAttribute(var_name);
            Urho3D::VariantVector nested_names;
            create_tw_item(node_, var_name, var_name, nested_names, val, node_info_root);
        }
    }
    tw_->addTopLevelItem(node_info_root);
    tw_->setItemWidget(node_info_root, 0, name_widget);
    tw_->setItemWidget(node_info_root, 1, enabled_widget);
    node_info_root->setExpanded(true);

    // Add the components to the tree widget
    const Urho3D::Vector<Urho3D::SharedPtr<Urho3D::Component>> & all_comps = node_->GetComponents();
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
            //bbtk.ui->map_editor->setFocus();
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
                create_tw_item(all_comps[i], var_name, var_name, nested_names, val, tw_item);
            }
            tw_->addTopLevelItem(tw_item);
            tw_->setItemWidget(tw_item, 1, cb_widget);
        }
    }
}

QWidget * Component_Widget::create_widget_item(Urho3D::Serializable * serz,
                                               Urho3D::String attrib_name,
                                               Urho3D::VariantVector nested_names,
                                               const Urho3D::String & value)
{
    QLineEdit * item = new QLineEdit;

    cb_desc fd(attrib_name, nested_names);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        item->setText(var.GetString().CString());
    };
    updaters[serz] = fd;
    fd.set_widget_value(value);

    auto func = [=]() { Slot_Callback(serz, nested_names, attrib_name, qPrintable(item->text())); };
    QObject::connect(item, &QLineEdit::editingFinished, func);
    return item;
}

QWidget * Component_Widget::create_widget_item(Urho3D::Serializable * serz,
                                               Urho3D::String attrib_name,
                                               Urho3D::VariantVector nested_names,
                                               int value)
{
    QSpinBox * item = new QSpinBox;
    item->setMaximum(std::numeric_limits<int>::max());
    item->setMinimum(std::numeric_limits<int>::min());

    cb_desc fd(attrib_name, nested_names);
    fd.set_widget_value = [=](const Urho3D::Variant & var) { item->setValue(var.GetInt()); };
    updaters[serz] = fd;
    fd.set_widget_value(value);

    auto func = [=](int new_val) { Slot_Callback(serz, nested_names, attrib_name, new_val); };

    QObject::connect(item, qOverload<int>(&QSpinBox::valueChanged), func);
    return item;
}

QWidget * Component_Widget::create_widget_item(Urho3D::Serializable * serz,
                                               Urho3D::String attrib_name,
                                               Urho3D::VariantVector nested_names,
                                               float value)
{
    QDoubleSpinBox * item = new QDoubleSpinBox;
    item->setSingleStep(0.1);
    item->setMaximum(std::numeric_limits<float>::max());
    item->setMinimum(-1.0f * std::numeric_limits<float>::max());

    cb_desc fd(attrib_name, nested_names);
    fd.set_widget_value = [=](const Urho3D::Variant & var) { item->setValue(var.GetFloat()); };
    updaters[serz] = fd;
    fd.set_widget_value(value);

    auto func = [=](float new_val) { Slot_Callback(serz, nested_names, attrib_name, new_val); };

    QObject::connect(item, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    return item;
}

QWidget * Component_Widget::create_widget_item(Urho3D::Serializable * serz,
                                               Urho3D::String attrib_name,
                                               Urho3D::VariantVector nested_names,
                                               double value)
{
    QDoubleSpinBox * item = new QDoubleSpinBox;
    item->setSingleStep(0.1);
    item->setMaximum(std::numeric_limits<double>::max());
    item->setMinimum(-1.0 * std::numeric_limits<double>::max());

    cb_desc fd(attrib_name, nested_names);
    fd.set_widget_value = [=](const Urho3D::Variant & var) { item->setValue(var.GetDouble()); };
    updaters[serz] = fd;
    fd.set_widget_value(value);

    auto func = [=](double new_val) { Slot_Callback(serz, nested_names, attrib_name, new_val); };

    QObject::connect(item, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    return item;
}

QWidget * Component_Widget::create_widget_item(Urho3D::Serializable * serz,
                                               Urho3D::String attrib_name,
                                               Urho3D::VariantVector nested_names,
                                               bool value)
{
    QCheckBox * item = new QCheckBox;
    item->setText("");

    cb_desc fd(attrib_name, nested_names);
    fd.set_widget_value = [=](const Urho3D::Variant & var) { item->setChecked(var.GetBool()); };
    updaters[serz] = fd;
    fd.set_widget_value(value);

    auto func = [=](int state) {
        Slot_Callback(serz, nested_names, attrib_name, state == Qt::Checked);
    };

    QObject::connect(item, &QCheckBox::stateChanged, func);
    return item;
}

QWidget * Component_Widget::create_widget_item(Urho3D::Serializable * serz,
                                               Urho3D::String attrib_name,
                                               Urho3D::VariantVector nested_names,
                                               const Urho3D::Vector2 & value)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
    // layout->setSpacing(0);
    layout->setMargin(0);

    QLabel * lbl_x = new QLabel("x");
    QLabel * lbl_y = new QLabel("y");

    QDoubleSpinBox * sb_x = new QDoubleSpinBox();
    QSizePolicy pc = sb_x->sizePolicy();
    pc.setHorizontalPolicy(QSizePolicy::Maximum);
    sb_x->setMaximum(std::numeric_limits<float>::max());
    sb_x->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_x->setSingleStep(0.1);
    sb_x->setSizePolicy(pc);

    QDoubleSpinBox * sb_y = new QDoubleSpinBox();
    sb_y->setMaximum(std::numeric_limits<float>::max());
    sb_y->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_y->setSingleStep(0.1);
    sb_y->setSizePolicy(pc);

    layout->addWidget(lbl_x);
    layout->addWidget(sb_x);
    layout->addStretch();
    layout->addWidget(lbl_y);
    layout->addWidget(sb_y);
    layout->addStretch();
    item->setLayout(layout);

    cb_desc fd(attrib_name, nested_names);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        Urho3D::Vector2 vec = var.GetVector2();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_x->setValue(vec.x_);
        sb_y->setValue(vec.y_);
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
    };
    updaters[serz] = fd;
    fd.set_widget_value(value);

    auto func = [=](double new_val) {
        Urho3D::Vector2 val;
        val.x_ = sb_x->value();
        val.y_ = sb_y->value();
        Slot_Callback(serz, nested_names, attrib_name, val);
    };

    QObject::connect(sb_x, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    QObject::connect(sb_y, qOverload<double>(&QDoubleSpinBox::valueChanged), func);

    return item;
}

QWidget * Component_Widget::create_widget_item(Urho3D::Serializable * serz,
                                               Urho3D::String attrib_name,
                                               Urho3D::VariantVector nested_names,
                                               const Urho3D::Vector3 & value)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
    // layout->setSpacing(0);
    layout->setMargin(0);

    QLabel * lbl_x = new QLabel("x");
    QLabel * lbl_y = new QLabel("y");
    QLabel * lbl_z = new QLabel("z");

    QDoubleSpinBox * sb_x = new QDoubleSpinBox();
    QSizePolicy pc = sb_x->sizePolicy();
    pc.setHorizontalPolicy(QSizePolicy::Maximum);
    sb_x->setMaximum(std::numeric_limits<float>::max());
    sb_x->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_x->setSingleStep(0.1);
    sb_x->setSizePolicy(pc);

    QDoubleSpinBox * sb_y = new QDoubleSpinBox();
    sb_y->setMaximum(std::numeric_limits<float>::max());
    sb_y->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_y->setSingleStep(0.1);
    sb_y->setSizePolicy(pc);

    QDoubleSpinBox * sb_z = new QDoubleSpinBox();
    sb_z->setMaximum(std::numeric_limits<float>::max());
    sb_z->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_z->setSingleStep(0.1);
    sb_z->setSizePolicy(pc);

    layout->addWidget(lbl_x);
    layout->addWidget(sb_x);
    layout->addStretch();
    layout->addWidget(lbl_y);
    layout->addWidget(sb_y);
    layout->addStretch();
    layout->addWidget(lbl_z);
    layout->addWidget(sb_z);
    layout->addStretch();
    item->setLayout(layout);

    cb_desc fd(attrib_name, nested_names);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        Urho3D::Vector3 vec = var.GetVector3();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_z->blockSignals(true);
        sb_x->setValue(vec.x_);
        sb_y->setValue(vec.y_);
        sb_z->setValue(vec.z_);
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
        sb_z->blockSignals(false);
    };
    updaters[serz] = fd;
    fd.set_widget_value(value);

    auto func = [=](double new_val) {
        Urho3D::Vector3 val;
        val.x_ = sb_x->value();
        val.y_ = sb_y->value();
        val.z_ = sb_z->value();
        Slot_Callback(serz, nested_names, attrib_name, val);
    };

    QObject::connect(sb_x, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    QObject::connect(sb_y, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    QObject::connect(sb_z, qOverload<double>(&QDoubleSpinBox::valueChanged), func);

    return item;
}

QWidget * Component_Widget::create_widget_item(Urho3D::Serializable * serz,
                                               Urho3D::String attrib_name,
                                               Urho3D::VariantVector nested_attrib_names,
                                               const Urho3D::IntVector2 & value)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
    // layout->setSpacing(0);
    layout->setMargin(0);

    QLabel * lbl_x = new QLabel("x");
    QLabel * lbl_y = new QLabel("y");

    QSpinBox * sb_x = new QSpinBox();
    QSizePolicy pc = sb_x->sizePolicy();
    pc.setHorizontalPolicy(QSizePolicy::Minimum);
    sb_x->setMaximum(std::numeric_limits<int>::max());
    sb_x->setMinimum(std::numeric_limits<int>::min());
    sb_x->setSizePolicy(pc);

    QSpinBox * sb_y = new QSpinBox();
    sb_y->setMaximum(std::numeric_limits<int>::max());
    sb_y->setMinimum(std::numeric_limits<int>::min());
    sb_y->setSizePolicy(pc);

    layout->addWidget(lbl_x);
    layout->addWidget(sb_x);
    layout->addStretch();
    layout->addWidget(lbl_y);
    layout->addWidget(sb_y);
    layout->addStretch();
    item->setLayout(layout);

    cb_desc fd(attrib_name, nested_attrib_names);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        Urho3D::IntVector2 vec = var.GetIntVector2();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_x->setValue(vec.x_);
        sb_y->setValue(vec.y_);
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
    };
    updaters[serz] = fd;
    fd.set_widget_value(value);

    auto func = [=](int new_val) {
        Urho3D::IntVector2 val;
        val.x_ = sb_x->value();
        val.y_ = sb_y->value();
        Slot_Callback(serz, nested_attrib_names, attrib_name, val);
    };

    QObject::connect(sb_x, qOverload<int>(&QSpinBox::valueChanged), func);
    QObject::connect(sb_y, qOverload<int>(&QSpinBox::valueChanged), func);

    return item;
}

QWidget * Component_Widget::create_widget_item(Urho3D::Serializable * serz,
                                               Urho3D::String attrib_name,
                                               Urho3D::VariantVector nested_attrib_names,
                                               const Urho3D::IntVector3 & value)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
    // layout->setSpacing(0);
    layout->setMargin(0);

    QLabel * lbl_x = new QLabel("x");
    QLabel * lbl_y = new QLabel("y");
    QLabel * lbl_z = new QLabel("z");

    QSpinBox * sb_x = new QSpinBox();
    QSizePolicy pc = sb_x->sizePolicy();
    pc.setHorizontalPolicy(QSizePolicy::Maximum);
    sb_x->setMaximum(std::numeric_limits<int>::max());
    sb_x->setMinimum(std::numeric_limits<int>::min());
    sb_x->setSizePolicy(pc);

    QSpinBox * sb_y = new QSpinBox();
    sb_y->setMaximum(std::numeric_limits<int>::max());
    sb_y->setMinimum(std::numeric_limits<int>::min());
    sb_y->setSizePolicy(pc);

    QSpinBox * sb_z = new QSpinBox();
    sb_z->setMaximum(std::numeric_limits<int>::max());
    sb_z->setMinimum(std::numeric_limits<int>::min());
    sb_z->setSizePolicy(pc);

    layout->addWidget(lbl_x);
    layout->addWidget(sb_x);
    layout->addStretch();
    layout->addWidget(lbl_y);
    layout->addWidget(sb_y);
    layout->addStretch();
    layout->addWidget(lbl_z);
    layout->addWidget(sb_z);
    layout->addStretch();
    item->setLayout(layout);

    cb_desc fd(attrib_name, nested_attrib_names);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        Urho3D::IntVector3 vec = var.GetIntVector3();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_z->blockSignals(true);
        sb_x->setValue(vec.x_);
        sb_y->setValue(vec.y_);
        sb_z->setValue(vec.z_);
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
        sb_z->blockSignals(false);
    };
    updaters[serz] = fd;
    fd.set_widget_value(value);

    auto func = [=](int new_val) {
        Urho3D::IntVector3 val;
        val.x_ = sb_x->value();
        val.y_ = sb_y->value();
        val.z_ = sb_z->value();
        Slot_Callback(serz, nested_attrib_names, attrib_name, val);
    };

    QObject::connect(sb_x, qOverload<int>(&QSpinBox::valueChanged), func);
    QObject::connect(sb_y, qOverload<int>(&QSpinBox::valueChanged), func);
    QObject::connect(sb_z, qOverload<int>(&QSpinBox::valueChanged), func);

    return item;
}

QWidget * Component_Widget::create_widget_item(Urho3D::Serializable * serz,
                                               Urho3D::String attrib_name,
                                               Urho3D::VariantVector nested_names,
                                               const Urho3D::Vector4 & value)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
    // layout->setSpacing(0);
    layout->setMargin(0);

    QLabel * lbl_x = new QLabel("x");
    QLabel * lbl_y = new QLabel("y");
    QLabel * lbl_z = new QLabel("z");
    QLabel * lbl_w = new QLabel("w");

    QDoubleSpinBox * sb_x = new QDoubleSpinBox();
    QSizePolicy pc = sb_x->sizePolicy();
    pc.setHorizontalPolicy(QSizePolicy::Maximum);
    sb_x->setMaximum(std::numeric_limits<float>::max());
    sb_x->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_x->setSingleStep(0.1);
    sb_x->setSizePolicy(pc);

    QDoubleSpinBox * sb_y = new QDoubleSpinBox();
    sb_y->setMaximum(std::numeric_limits<float>::max());
    sb_y->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_y->setSingleStep(0.1);
    sb_y->setSizePolicy(pc);

    QDoubleSpinBox * sb_z = new QDoubleSpinBox();
    sb_z->setMaximum(std::numeric_limits<float>::max());
    sb_z->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_z->setSingleStep(0.1);
    sb_z->setSizePolicy(pc);

    QDoubleSpinBox * sb_w = new QDoubleSpinBox();
    sb_z->setMaximum(std::numeric_limits<float>::max());
    sb_z->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_z->setSingleStep(0.1);
    sb_z->setSizePolicy(pc);

    layout->addWidget(lbl_x);
    layout->addWidget(sb_x);
    layout->addStretch();
    layout->addWidget(lbl_y);
    layout->addWidget(sb_y);
    layout->addStretch();
    layout->addWidget(lbl_z);
    layout->addWidget(sb_z);
    layout->addStretch();
    layout->addWidget(lbl_w);
    layout->addWidget(sb_w);
    layout->addStretch();
    item->setLayout(layout);

    cb_desc fd(attrib_name, nested_names);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        Urho3D::Vector4 vec = var.GetVector4();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_z->blockSignals(true);
        sb_w->blockSignals(true);
        sb_x->setValue(vec.x_);
        sb_y->setValue(vec.y_);
        sb_z->setValue(vec.z_);
        sb_z->setValue(vec.w_);
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
        sb_z->blockSignals(false);
        sb_w->blockSignals(false);
    };
    updaters[serz] = fd;
    fd.set_widget_value(value);

    auto func = [=](double new_val) {
        Urho3D::Vector4 val;
        val.x_ = sb_x->value();
        val.y_ = sb_y->value();
        val.z_ = sb_z->value();
        val.w_ = sb_w->value();
        Slot_Callback(serz, nested_names, attrib_name, val);
    };

    QObject::connect(sb_x, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    QObject::connect(sb_y, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    QObject::connect(sb_z, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    QObject::connect(sb_w, qOverload<double>(&QDoubleSpinBox::valueChanged), func);

    return item;
}

QWidget * Component_Widget::create_widget_item(Urho3D::Serializable * serz,
                                               Urho3D::String attrib_name,
                                               Urho3D::VariantVector nested_attrib_names,
                                               const Urho3D::IntRect & value)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
    layout->setMargin(0);

    QLabel * lbl_x = new QLabel("x");
    QLabel * lbl_y = new QLabel("y");
    QLabel * lbl_w = new QLabel("w");
    QLabel * lbl_h = new QLabel("h");

    QSpinBox * sb_x = new QSpinBox();
    QSizePolicy pc = sb_x->sizePolicy();
    pc.setHorizontalPolicy(QSizePolicy::Preferred);
    sb_x->setMaximum(std::numeric_limits<int>::max());
    sb_x->setMinimum(std::numeric_limits<int>::min());
    sb_x->setSizePolicy(pc);

    QSpinBox * sb_y = new QSpinBox();
    sb_y->setMaximum(std::numeric_limits<int>::max());
    sb_y->setMinimum(std::numeric_limits<int>::min());
    sb_y->setSizePolicy(pc);

    QSpinBox * sb_w = new QSpinBox();
    sb_w->setMaximum(std::numeric_limits<int>::max());
    sb_w->setMinimum(std::numeric_limits<int>::min());
    sb_w->setSizePolicy(pc);

    QSpinBox * sb_h = new QSpinBox();
    sb_h->setMaximum(std::numeric_limits<int>::max());
    sb_h->setMinimum(std::numeric_limits<int>::min());
    sb_h->setSizePolicy(pc);

    layout->addWidget(lbl_x);
    layout->addWidget(sb_x);
    layout->addStretch();
    layout->addWidget(lbl_y);
    layout->addWidget(sb_y);
    layout->addStretch();
    layout->addWidget(lbl_w);
    layout->addWidget(sb_w);
    layout->addStretch();
    layout->addWidget(lbl_h);
    layout->addWidget(sb_h);
    layout->addStretch();
    item->setLayout(layout);

    cb_desc fd(attrib_name, nested_attrib_names);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        Urho3D::IntRect vec = var.GetIntRect();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_w->blockSignals(true);
        sb_h->blockSignals(true);
        sb_x->setValue(vec.left_);
        sb_y->setValue(vec.bottom_);
        sb_w->setValue(vec.Width());
        sb_h->setValue(vec.Height());
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
        sb_w->blockSignals(false);
        sb_h->blockSignals(false);
    };
    updaters[serz] = fd;
    fd.set_widget_value(value);

    auto func = [=](int new_val) {
        Urho3D::IntRect val;
        val.left_ = sb_x->value();
        val.bottom_ = sb_y->value();
        val.top_ = val.bottom_ + sb_h->value();
        val.right_ = val.left_ + sb_w->value();
        Slot_Callback(serz, nested_attrib_names, attrib_name, val);
    };

    QObject::connect(sb_x, qOverload<int>(&QSpinBox::valueChanged), func);
    QObject::connect(sb_y, qOverload<int>(&QSpinBox::valueChanged), func);
    QObject::connect(sb_w, qOverload<int>(&QSpinBox::valueChanged), func);
    QObject::connect(sb_h, qOverload<int>(&QSpinBox::valueChanged), func);

    return item;
}

Component_Widget::Component_Widget(QWidget * parent)
    : QWidget(nullptr), tw_(nullptr), node_(nullptr)
{
    QVBoxLayout * layout = new QVBoxLayout;
    layout->setMargin(0);
    // layout->setSpacing(0);
    tw_ = new QTreeWidget;
    tw_->setHeaderHidden(true);
    tw_->setStyleSheet(TW_STYLE);
    tw_->setSelectionMode(QAbstractItemView::NoSelection);
    tw_->header()->setStretchLastSection(false);
    tw_->header()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(tw_);
    setLayout(layout);
}

Component_Widget::~Component_Widget()
{}

void Component_Widget::setup_ui(Urho3D::Node * node)
{
    if (node_ == node)
        return;
    
    node_ = node;
    build_tree_widget();
}
