#include <QComboBox>
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

void Slot_Callback(const Urho3D::Vector<Urho3D::Serializable *> & serz,
                   Urho3D::VariantVector nested_attrib_names,
                   const Urho3D::String & attrib_name,
                   const Urho3D::Variant & val)
{
    for (int i = 0; i < serz.Size(); ++i)
    {
        Urho3D::Variant attrib = serz[i]->GetAttribute(attrib_name);
        Urho3D::Variant * final_attrib = Get_Attrib_Variant(attrib, nested_attrib_names);
        *final_attrib = val;
        serz[i]->SetAttribute(attrib_name, attrib);
    }
}

void Component_Widget::update_tw_from_node()
{
    auto iter = updaters.begin();
    while (iter != updaters.end())
    {
        Urho3D::Vector<Urho3D::Variant> values;
        for (int i = 0; i < iter->serz.Size(); ++i)
        {
            Urho3D::Variant attrib = iter->serz[i]->GetAttribute(iter->attrib_name);
            Urho3D::Variant * nested_val = Get_Attrib_Variant(attrib, iter->nested_names);
            values.Push(*nested_val);
        }
        do_set_widget(&iter.value(), values);
        ++iter;
    }
}

void Component_Widget::create_tw_item(const Urho3D::Vector<Urho3D::Serializable *> & serz,
                                      const Urho3D::String & attrib_name,
                                      const Urho3D::String & nested_name,
                                      Urho3D::VariantVector & nested_attrib_names,
                                      const Urho3D::Vector<Urho3D::Variant> & values,
                                      QTreeWidgetItem * parent,
                                      const Urho3D::AttributeInfo & att_inf)
{
    Urho3D::VariantType vtype = values[0].GetType();

    QTreeWidgetItem * tw_item = new QTreeWidgetItem;
    tw_item->setText(0, nested_name.CString());
    QWidget * item_widget = nullptr;

    switch (vtype)
    {
    case (Urho3D::VAR_VARIANTVECTOR):
    {
        // const Urho3D::VariantVector & vv = value.GetVariantVector();
        // tw_item->setText(
        //     0, tw_item->text(0) + QString(" (") + QString::number(vv.Size()) + QString(" Items)"));
        // for (int i = 0; i < vv.Size(); ++i)
        // {
        //     nested_attrib_names.Push(i);
        //     std::stringstream ss;
        //     ss << attrib_name.CString() << "[" << i << "]";
        //     create_tw_item(ser, attrib_name, ss.str().c_str(), nested_attrib_names, vv[i], tw_item);
        // }
        break;
    }
    case (Urho3D::VAR_VARIANTMAP):
    {
        // const Urho3D::VariantMap & vm = value.GetVariantMap();
        // tw_item->setText(
        //     0, tw_item->text(0) + QString(" (") + QString::number(vm.Size()) + QString(" Items)"));
        // auto vm_iter = vm.Begin();
        // while (vm_iter != vm.End())
        // {
        //     nested_attrib_names.Push(vm_iter->first_);
        //     create_tw_item(ser,
        //                    attrib_name,
        //                    vm_iter->first_.Reverse(),
        //                    nested_attrib_names,
        //                    vm_iter->second_,
        //                    tw_item);
        //     ++vm_iter;
        // }
        break;
    }
    case (Urho3D::VAR_INT):
        item_widget = create_int_widget_item(Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (Urho3D::VAR_PTR):
        eout << "Cannot generate widget for pointer!";
        break;
    case (Urho3D::VAR_BOOL):
        item_widget = create_bool_widget_item(Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (Urho3D::VAR_NONE):
        eout << "No variable type found for dialog";
        break;
    case (Urho3D::VAR_RECT):
        break;
    case (Urho3D::VAR_STRING):
        item_widget = create_string_widget_item(Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (Urho3D::VAR_COLOR):
        break;
    case (Urho3D::VAR_FLOAT):
        item_widget = create_float_widget_item(Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (Urho3D::VAR_INT64):
        item_widget = create_int_widget_item(Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (Urho3D::VAR_INTRECT):
        item_widget = create_irect_widget_item(Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (Urho3D::VAR_MATRIX3):
        break;
    case (Urho3D::VAR_MATRIX4):
        break;
    case (Urho3D::VAR_MATRIX3X4):
        break;
    case (Urho3D::VAR_VECTOR2):
        item_widget = create_vec2_widget_item(Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (Urho3D::VAR_VECTOR3):
        item_widget = create_vec3_widget_item(Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (Urho3D::VAR_VECTOR4):
        item_widget = create_vec4_widget_item(Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (Urho3D::VAR_DOUBLE):
        item_widget = create_double_widget_item(Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (Urho3D::VAR_INTVECTOR2):
        item_widget = create_ivec2_widget_item(Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (Urho3D::VAR_INTVECTOR3):
        item_widget = create_ivec3_widget_item(Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (Urho3D::VAR_STRINGVECTOR):
    {
        // const Urho3D::StringVector & vv = value.GetStringVector();
        // tw_item->setText(
        //     0, tw_item->text(0) + QString(" (") + QString::number(vv.Size()) + QString(" Items)"));
        // for (int i = 0; i < vv.Size(); ++i)
        // {
        //     nested_attrib_names.Push(i);
        //     std::stringstream ss;
        //     ss << attrib_name.CString() << "[" << i << "]";
        //     create_tw_item(ser, attrib_name, ss.str().c_str(), nested_attrib_names, vv[i], tw_item);
        // }
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
        // const Urho3D::ResourceRefList & vv = value.GetResourceRefList();
        // tw_item->setText(0, tw_item->text(0) + QString(" (") + QString::number(vv.names_.Size()) + QString(" Items)"));
        // for (int i = 0; i < vv.names_.Size(); ++i)
        // {
        //     Urho3D::ResourceRef ref;
        //     ref.name_ = vv.names_[i];
        //     ref.type_ = vv.type_;
        //     nested_attrib_names.Push(i);
        //     std::stringstream ss;
        //     ss << attrib_name.CString() << "[" << i << "]";
        //     create_tw_item(ser, attrib_name, ss.str().c_str(), nested_attrib_names, ref, tw_item);
        // }
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

void Component_Widget::add_node_to_treewidget(const Urho3D::Vector<Urho3D::Node *> nodes)
{
    if (nodes.Empty())
        return;

    QTreeWidgetItem * node_info_root = new QTreeWidgetItem;

    Urho3D::Vector<Urho3D::Variant> names;
    Urho3D::Vector<Urho3D::Variant> enables;
    Urho3D::Vector<Urho3D::Serializable*> serz;
    for (int i = 0; i < nodes.Size(); ++i)
    {
        names.Push(nodes[i]->GetName());
        enables.Push(nodes[i]->IsEnabled());
        serz.Push(nodes[i]);
    }

    QWidget * name_widget =
        create_string_widget_item(Create_Widget_Params(serz, "Name", Urho3D::VariantVector(), names,Urho3D::AttributeInfo()));
    QWidget * enabled_widget =
        create_bool_widget_item(Create_Widget_Params(serz, "Is Enabled", Urho3D::VariantVector(), enables,Urho3D::AttributeInfo()));

    auto node_attribs = nodes[0]->GetAttributes();
    if (node_attribs != nullptr)
    {
        for (int att_ind = 0; att_ind < node_attribs->Size(); ++att_ind)
        {
            Urho3D::String var_name = (*node_attribs)[att_ind].name_;
            if (var_name == "Name" || var_name == "Is Enabled")
                continue;
            Urho3D::VariantVector nested_names;
            Urho3D::Vector<Urho3D::Variant> values;
            bool create_item = true;

            for (int i = 0; i < serz.Size(); ++i)
            {
                Urho3D::Variant val = nodes[i]->GetAttribute(var_name);
                if (val.IsEmpty())
                {
                    create_item = false;
                    break;
                }
                values.Push(val);
            }
            if (create_item)
                create_tw_item(serz, var_name, var_name, nested_names, values, node_info_root, (*node_attribs)[att_ind]);
        }
    }

    tw_->addTopLevelItem(node_info_root);
    tw_->setItemWidget(node_info_root, 0, name_widget);
    tw_->setItemWidget(node_info_root, 1, enabled_widget);
    node_info_root->setExpanded(true);

    // Add the components to the tree widget
    const Urho3D::Vector<Urho3D::SharedPtr<Urho3D::Component>> & all_comps = nodes[0]->GetComponents();
    for (int i = 0; i < all_comps.Size(); ++i)
    {
        bool skip_component = false;

        Urho3D::Vector<Urho3D::Serializable*> comp_serz;
        Urho3D::Vector<Urho3D::Variant> enables;
        for (int ndi = 0; ndi < nodes.Size(); ++ndi)
        {
            Urho3D::Component * comp = nodes[ndi]->GetComponent(all_comps[i]->GetType());
            if (comp == nullptr)
            {
                skip_component = true;
                iout << "Skipping component:" << comp->GetTypeName().CString();
                break;
            }
            enables.Push(comp->IsEnabled());
            comp_serz.Push(comp);
        }

        QTreeWidgetItem * tw_item = new QTreeWidgetItem;
        QWidget * comp_widget = create_bool_widget_item(Create_Widget_Params(comp_serz, "Is Enabled", Urho3D::VariantVector(), enables, Urho3D::AttributeInfo()));
        
        Urho3D::Component * comp = all_comps[i];
        tw_item->setText(0, comp->GetTypeName().CString());

        auto attribs = all_comps[i]->GetAttributes();
        if (attribs != nullptr)
        {
            for (int j = 0; j < attribs->Size(); ++j)
            {
                const Urho3D::AttributeInfo & cur_att_inf = (*attribs)[j];
                Urho3D::String var_name = cur_att_inf.name_;
                if ((cur_att_inf.mode_ & Urho3D::AM_NOEDIT) == Urho3D::AM_NOEDIT ||
                    var_name == "Is Enabled")
                    continue;
                
                bool add_widget = true;
                Urho3D::VariantVector nested_names;
                Urho3D::Vector<Urho3D::Variant> vals;
                for (int ci = 0; ci < comp_serz.Size(); ++ci)
                {
                    Urho3D::Variant val = comp_serz[ci]->GetAttribute(var_name);
                    if (val.IsEmpty())
                    {
                        add_widget = false;
                        break;
                    }
                    vals.Push(val);
                }
                if (add_widget)
                    create_tw_item(comp_serz, var_name, var_name, nested_names, vals, tw_item, cur_att_inf);
            }
            tw_->addTopLevelItem(tw_item);
            tw_->setItemWidget(tw_item, 1, comp_widget);
        }
    }
}

void Component_Widget::do_set_widget(cb_desc * fd, const Urho3D::Vector<Urho3D::Variant> & values)
{
    bool pass_value = true;
    Urho3D::Variant val(values[0]);
    if (val.GetType() == Urho3D::VAR_VECTOR3)
    {
        Urho3D::Vector3 val_vec = val.GetVector3();
        float max_f = std::numeric_limits<float>::max();
        for (int i = 1; i < values.Size(); ++i)
        {
            Urho3D::Vector3 cur_vec = values[i].GetVector3();
            Urho3D::Vector3 prev_vec = values[i - 1].GetVector3();
            if (cur_vec.x_ != max_f && cur_vec.x_ != prev_vec.x_)
                val_vec.x_ = max_f;
            if (cur_vec.y_ != max_f && cur_vec.y_ != prev_vec.y_)
                val_vec.y_ = max_f;
            if (cur_vec.z_ != max_f && cur_vec.z_ != prev_vec.z_)
                val_vec.z_ = max_f;
        }
        val = val_vec;
    }
    else
    {
        for (int i = 1; i < values.Size(); ++i)
        {
            if (values[i] != values[i - 1])
            {
                val = Urho3D::Variant();
                break;
            }
        }
    }
    fd->set_widget_value(val);
}

QWidget * Component_Widget::create_string_widget_item(Create_Widget_Params params)
{
    QLineEdit * item = new QLineEdit;

    cb_desc fd(params.attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        if (item->hasFocus())
            return;
        
        item->blockSignals(true);
        if (var.IsEmpty())
        {
            item->setText("");
            item->setPlaceholderText("Different values");
        }
        else
            item->setText(var.GetString().CString());
        item->blockSignals(false);
    };
    updaters[item] = fd;
    do_set_widget(&fd, params.values_);
 
    auto func = [=]() { Slot_Callback(params.serz_, params.nested_attrib_names_, params.attrib_name_, item->text().toStdString().c_str()); };
    QObject::connect(item, &QLineEdit::editingFinished, func);
    return item;
}

QWidget * Component_Widget::create_int_widget_item(Create_Widget_Params params)
{
    if (params.att_inf_.enumNames_ != nullptr)
    {
        QComboBox * item = new QComboBox;
        const char ** begin = params.att_inf_.enumNames_;
        while (*begin != nullptr)
        {
            QString str(*begin);
            item->addItem(str);
            ++begin;
        }

        cb_desc fd(params.attrib_name_, params.nested_attrib_names_, params.serz_);
        fd.set_widget_value = [=](const Urho3D::Variant & var) {
            item->blockSignals(true);
            if (var.IsEmpty())
            {
                if (item->itemData(0).toInt() != 22)
                    item->insertItem(0,"-- --", 22);
                item->setCurrentIndex(0);
            }
            else
            {
                item->setCurrentIndex(var.GetInt());
            }
            item->blockSignals(false);
        };
        updaters[item] = fd;
        do_set_widget(&fd, params.values_);
        
        auto func = [=](int new_val) { 
            if (item->itemData(0).toInt() == 22)
            {
                item->removeItem(0);
                new_val -= 1;
            }
            Slot_Callback(params.serz_, params.nested_attrib_names_, params.attrib_name_, new_val); 
            };

        QObject::connect(item, qOverload<int>(&QComboBox::currentIndexChanged), func);

        return item;
    }
    else
    {
        QSpinBox * item = new QSpinBox;
        item->setMaximum(std::numeric_limits<int>::max());
        item->setMinimum(std::numeric_limits<int>::min());

        cb_desc fd(params.attrib_name_, params.nested_attrib_names_, params.serz_);
        fd.set_widget_value = [=](const Urho3D::Variant & var) {
            item->blockSignals(true);
            if (var.IsEmpty())
                item->setSpecialValueText("-- --");
            else
                item->setValue(var.GetInt());
            item->blockSignals(false);
        };
        updaters[item] = fd;
        do_set_widget(&fd, params.values_);
        
        auto func = [=](int new_val) { Slot_Callback(params.serz_, params.nested_attrib_names_, params.attrib_name_, new_val); };

        QObject::connect(item, qOverload<int>(&QSpinBox::valueChanged), func);
        return item;
    }
}

QWidget * Component_Widget::create_float_widget_item(Create_Widget_Params params)
{
    QDoubleSpinBox * item = new QDoubleSpinBox;
    item->setSingleStep(0.1);
    item->setMaximum(std::numeric_limits<float>::max());
    item->setMinimum(-1.0f * std::numeric_limits<float>::max());
    item->setSpecialValueText("-- --");

    cb_desc fd(params.attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        item->blockSignals(true);
        if (var.IsEmpty())
            item->setValue(item->minimum());
        else
            item->setValue(var.GetFloat());
        item->blockSignals(false);
        };
    updaters[item] = fd;
    fd.set_widget_value(params.values_);

    auto func = [=](float new_val) { Slot_Callback(params.serz_, params.nested_attrib_names_, params.attrib_name_, new_val); };

    QObject::connect(item, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    return item;
}

QWidget * Component_Widget::create_double_widget_item(Create_Widget_Params params)
{
    QDoubleSpinBox * item = new QDoubleSpinBox;
    item->setSingleStep(0.1);
    item->setMaximum(std::numeric_limits<double>::max());
    item->setMinimum(-1.0 * std::numeric_limits<double>::max());
    item->setSpecialValueText("-- --");
    cb_desc fd(params.attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        item->blockSignals(true);
        if (var.IsEmpty())
            item->setValue(item->minimum());
        else
            item->setValue(var.GetDouble());
        };
        item->blockSignals(false);
    updaters[item] = fd;
    fd.set_widget_value(params.values_);

    auto func = [=](double new_val) { Slot_Callback(params.serz_, params.nested_attrib_names_, params.attrib_name_, new_val); };

    QObject::connect(item, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    return item;
}

QWidget * Component_Widget::create_bool_widget_item(Create_Widget_Params params)
{
    QCheckBox * item = new QCheckBox;
    item->setTristate(true);

    cb_desc fd(params.attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        item->blockSignals(true);
        if (var.IsEmpty())
            item->setCheckState(Qt::PartiallyChecked);
        else
            item->setCheckState(var.GetBool() ? Qt::Checked : Qt::Unchecked);
        item->blockSignals(false);
    };
    updaters[item] = fd;
    do_set_widget(&fd, params.values_);

    auto func = [=](int state) {
        Slot_Callback(params.serz_, params.nested_attrib_names_, params.attrib_name_, state != Qt::Unchecked);
    };

    QObject::connect(item, &QCheckBox::stateChanged, func);
    return item;
}

QWidget * Component_Widget::create_vec2_widget_item(Create_Widget_Params params)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
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

    sb_x->setSpecialValueText("-- --");
    sb_y->setSpecialValueText("-- --");

    cb_desc fd(params.attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        Urho3D::Vector2 vec = var.GetVector2();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        if (var.IsEmpty())
        {
            sb_x->setValue(sb_x->minimum());
            sb_y->setValue(sb_y->minimum());
        }
        else
        {
            sb_x->setValue(vec.x_);
            sb_y->setValue(vec.y_);
        }
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
    };
    updaters[item] = fd;
    fd.set_widget_value(params.values_);

    auto func = [=](double new_val) {
        Urho3D::Vector2 val;
        val.x_ = sb_x->value();
        val.y_ = sb_y->value();
        Slot_Callback(params.serz_, params.nested_attrib_names_, params.attrib_name_, val);
    };

    QObject::connect(sb_x, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    QObject::connect(sb_y, qOverload<double>(&QDoubleSpinBox::valueChanged), func);

    return item;
}

QWidget * Component_Widget::create_vec3_widget_item(Create_Widget_Params params)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
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
    sb_y->setMinimumWidth(10);
    sb_y->setSingleStep(0.1);
    sb_y->setSizePolicy(pc);

    QDoubleSpinBox * sb_z = new QDoubleSpinBox();
    sb_z->setMaximum(std::numeric_limits<float>::max());
    sb_z->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_z->setSingleStep(0.1);
    sb_z->setSizePolicy(pc);

    sb_x->setMinimumWidth(10);
    sb_y->setMinimumWidth(10);
    sb_z->setMinimumWidth(10);

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

    sb_x->setSpecialValueText("-");
    sb_y->setSpecialValueText("-");
    sb_z->setSpecialValueText("-");

    cb_desc fd(params.attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Urho3D::Variant & var)
    {            
        Urho3D::Vector3 vec = var.GetVector3();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_z->blockSignals(true);
        float max_f = std::numeric_limits<float>::max();
        if (var.IsEmpty())
        {
            sb_x->setValue(sb_x->minimum());
            sb_y->setValue(sb_y->minimum());
            sb_z->setValue(sb_z->minimum());
        }
        else 
        {
            if (vec.x_ == max_f)
                sb_x->setValue(sb_x->minimum());
            else
                sb_x->setValue(vec.x_);
            if (vec.y_ == max_f)
                sb_y->setValue(sb_y->minimum());
            else
                sb_y->setValue(vec.y_);
            if (vec.z_ == max_f)
                sb_z->setValue(sb_z->minimum());
            else
                sb_z->setValue(vec.z_);
        }
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
        sb_z->blockSignals(false);
    };
    updaters[item] = fd;
    fd.set_widget_value(params.values_);

    auto func_x = [=](double new_val) mutable {
        Urho3D::Vector3 val;
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_x->value() == sb_x->minimum())
                return;
            
            Urho3D::Variant attrib = params.serz_[i]->GetAttribute(params.attrib_name_);
            Urho3D::Variant * final_attrib =
                Get_Attrib_Variant(attrib, params.nested_attrib_names_);
            Urho3D::Vector3 vec = final_attrib->GetVector3();
            vec.x_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.attrib_name_, attrib);
        }
    };

    auto func_y = [=](double new_val) mutable
    {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_y->value() == sb_y->minimum())
                return;
            
            Urho3D::Variant attrib = params.serz_[i]->GetAttribute(params.attrib_name_);
            Urho3D::Variant * final_attrib =
                Get_Attrib_Variant(attrib, params.nested_attrib_names_);
            Urho3D::Vector3 vec = final_attrib->GetVector3();
            vec.y_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.attrib_name_, attrib);
        }
    };

    auto func_z = [=](double new_val) mutable {
        Urho3D::Vector3 val;
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_z->value() == sb_z->minimum())
                return;

            Urho3D::Variant attrib = params.serz_[i]->GetAttribute(params.attrib_name_);
            Urho3D::Variant * final_attrib =
                Get_Attrib_Variant(attrib, params.nested_attrib_names_);
            Urho3D::Vector3 vec = final_attrib->GetVector3();
            vec.z_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.attrib_name_, attrib);
        }
    };

    QObject::connect(sb_x, qOverload<double>(&QDoubleSpinBox::valueChanged), func_x);
    QObject::connect(sb_y, qOverload<double>(&QDoubleSpinBox::valueChanged), func_y);
    QObject::connect(sb_z, qOverload<double>(&QDoubleSpinBox::valueChanged), func_z);

    return item;
}

QWidget * Component_Widget::create_ivec2_widget_item(Create_Widget_Params params)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
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

    sb_x->setSpecialValueText("-- --");
    sb_y->setSpecialValueText("-- --");

    cb_desc fd(params.attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        Urho3D::IntVector2 vec = var.GetIntVector2();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        if (var.IsEmpty())
        {
            sb_x->setValue(sb_x->minimum());
            sb_y->setValue(sb_y->minimum());
        }
        else
        {
            sb_x->setValue(vec.x_);
            sb_y->setValue(vec.y_);
        }
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
    };
    updaters[item] = fd;
    fd.set_widget_value(params.values_);

    auto func = [=](int new_val) {
        Urho3D::IntVector2 val;
        val.x_ = sb_x->value();
        val.y_ = sb_y->value();
        Slot_Callback(params.serz_, params.nested_attrib_names_, params.attrib_name_, val);
    };

    QObject::connect(sb_x, qOverload<int>(&QSpinBox::valueChanged), func);
    QObject::connect(sb_y, qOverload<int>(&QSpinBox::valueChanged), func);

    return item;
}

QWidget * Component_Widget::create_ivec3_widget_item(Create_Widget_Params params)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
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

    sb_x->setSpecialValueText("-- --");
    sb_y->setSpecialValueText("-- --");
    sb_z->setSpecialValueText("-- --");

    cb_desc fd(params.attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        Urho3D::IntVector3 vec = var.GetIntVector3();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_z->blockSignals(true);
        if (var.IsEmpty())
        {
            sb_x->setValue(sb_x->minimum());
            sb_y->setValue(sb_y->minimum());
            sb_z->setValue(sb_z->minimum());
        }
        else
        {
            sb_x->setValue(vec.x_);
            sb_y->setValue(vec.y_);
            sb_z->setValue(vec.z_);
        }
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
        sb_z->blockSignals(false);
    };
    updaters[item] = fd;
    fd.set_widget_value(params.values_);

    auto func = [=](int new_val) {
        Urho3D::IntVector3 val;
        val.x_ = sb_x->value();
        val.y_ = sb_y->value();
        val.z_ = sb_z->value();
        Slot_Callback(params.serz_ , params.nested_attrib_names_, params.attrib_name_, val);
    };

    QObject::connect(sb_x, qOverload<int>(&QSpinBox::valueChanged), func);
    QObject::connect(sb_y, qOverload<int>(&QSpinBox::valueChanged), func);
    QObject::connect(sb_z, qOverload<int>(&QSpinBox::valueChanged), func);

    return item;
}

QWidget * Component_Widget::create_vec4_widget_item(Create_Widget_Params params)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
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

    sb_x->setSpecialValueText("-- --");
    sb_y->setSpecialValueText("-- --");
    sb_z->setSpecialValueText("-- --");
    sb_z->setSpecialValueText("-- --");

    cb_desc fd(params.attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        Urho3D::Vector4 vec = var.GetVector4();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_z->blockSignals(true);
        sb_w->blockSignals(true);
        if (var.IsEmpty())
        {
            sb_x->setValue(sb_x->minimum());
            sb_y->setValue(sb_y->minimum());
            sb_z->setValue(sb_z->minimum());
            sb_z->setValue(sb_w->minimum());
        }
        else
        {
            sb_x->blockSignals(false);
            sb_y->blockSignals(false);
            sb_z->blockSignals(false);
            sb_w->blockSignals(false);
        }
    };
    updaters[item] = fd;
    fd.set_widget_value(params.values_);

    auto func = [=](double new_val) {
        Urho3D::Vector4 val;
        val.x_ = sb_x->value();
        val.y_ = sb_y->value();
        val.z_ = sb_z->value();
        val.w_ = sb_w->value();
        Slot_Callback(params.serz_ , params.nested_attrib_names_, params.attrib_name_, val);
    };

    QObject::connect(sb_x, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    QObject::connect(sb_y, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    QObject::connect(sb_z, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    QObject::connect(sb_w, qOverload<double>(&QDoubleSpinBox::valueChanged), func);

    return item;
}

QWidget * Component_Widget::create_irect_widget_item(Create_Widget_Params params)
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


    sb_x->setSpecialValueText("-- --");
    sb_y->setSpecialValueText("-- --");
    sb_w->setSpecialValueText("-- --");
    sb_h->setSpecialValueText("-- --");

    cb_desc fd(params.attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Urho3D::Variant & var) {
        Urho3D::IntRect vec = var.GetIntRect();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_w->blockSignals(true);
        sb_h->blockSignals(true);
        if (var.IsEmpty())
        {
            sb_x->setValue(sb_x->minimum());
            sb_y->setValue(sb_y->minimum());
            sb_w->setValue(sb_w->minimum());
            sb_h->setValue(sb_h->minimum());
        }
        else
        {
            sb_x->setValue(vec.left_);
            sb_y->setValue(vec.bottom_);
            sb_w->setValue(vec.Width());
            sb_h->setValue(vec.Height());
        }
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
        sb_w->blockSignals(false);
        sb_h->blockSignals(false);
    };
    updaters[item] = fd;
    fd.set_widget_value(params.values_);

    auto func = [=](int new_val) {
        Urho3D::IntRect val;
        val.left_ = sb_x->value();
        val.bottom_ = sb_y->value();
        val.top_ = val.bottom_ + sb_h->value();
        val.right_ = val.left_ + sb_w->value();
        Slot_Callback(params.serz_ , params.nested_attrib_names_, params.attrib_name_, val);
    };

    QObject::connect(sb_x, qOverload<int>(&QSpinBox::valueChanged), func);
    QObject::connect(sb_y, qOverload<int>(&QSpinBox::valueChanged), func);
    QObject::connect(sb_w, qOverload<int>(&QSpinBox::valueChanged), func);
    QObject::connect(sb_h, qOverload<int>(&QSpinBox::valueChanged), func);

    return item;
}

Component_Widget::Component_Widget(QWidget * parent)
    : QWidget(nullptr), tw_(nullptr)
{
    QVBoxLayout * layout = new QVBoxLayout;
    layout->setMargin(0);
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

void Component_Widget::recursive_tree_widget_check(QTreeWidgetItem * item, bool restoring)
{
    if (restoring)
    {
        auto fiter = prev_expanded_items.find(item->text(0));
        item->setExpanded(fiter != prev_expanded_items.end());
    }
    else
    {
        if (item->isExpanded())
            prev_expanded_items.insert(item->text(0));
    }

    for (int i = 0; i < item->childCount(); ++i)
        recursive_tree_widget_check(item->child(i), restoring);
}

void Component_Widget::setup_ui(const Urho3D::Vector<Urho3D::Node *> & nodes)
{
    if (selection_ == nodes || nodes.Empty())
        return;
    
    selection_ = nodes;
    prev_expanded_items.clear();

    // Cache the expanded items
    for (int i = 0; i < tw_->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem * cur_item = tw_->topLevelItem(i);
        recursive_tree_widget_check(cur_item, false);
    }

    tw_->clear();
    tw_->setColumnCount(2);
    updaters.clear();

    // If there is only one node, just add it
    // Otherwise we need to go through and add only components that exist on all nodes
    // If the attribute values are the same for that component or node, then show the value
    // otherwise show a dash like unity. If edits are made, it should change all "serializable's" to that value
    add_node_to_treewidget(nodes);

    // Restore expanded items
    for (int i = 0; i < tw_->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem * cur_item = tw_->topLevelItem(i);
        recursive_tree_widget_check(cur_item, true);
    }
}
