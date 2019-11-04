#include <QComboBox>
#include <component_widget.h>
#include <mtdebug_print.h>

#include <toolkit.h>
#include <ui_toolkit.h>

#include <limits>

#include <urho_common.h>

#include <QToolButton>
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

#define DEBOUT                                                                                     \
    iout << "Attrib name:" << params.top_level_attrib_name_.CString();                             \
    iout << "Nested names:";                                                                       \
    for (int i = 0; i < params.nested_attrib_names_.Size(); ++i)                                   \
        iout << params.nested_attrib_names_[i].GetStringHash().Reverse().CString();

Variant * Get_Attrib_Variant(Variant & attrib_value, VariantVector & nested_attrib_names)
{
    Variant * cur_nested_val = &attrib_value;
    while (!nested_attrib_names.Empty())
    {
        Variant & cur_val = nested_attrib_names[0];
        if (cur_nested_val->GetType() == VAR_VARIANTVECTOR)
        {
            int index = cur_val.GetInt();
            VariantVector * vv = cur_nested_val->GetVariantVectorPtr();
            cur_nested_val = &((*vv)[index]);
        }
        else if (cur_nested_val->GetType() == VAR_VARIANTMAP)
        {
            StringHash key = nested_attrib_names[0].GetStringHash();
            VariantMap * vm = cur_nested_val->GetVariantMapPtr();
            cur_nested_val = &((*vm)[key]);
        }
        nested_attrib_names.Erase(0);
    }
    return cur_nested_val;
}

void Slot_Callback(const Vector<Serializable *> & serz,
                   VariantVector nested_attrib_names,
                   const String & attrib_name,
                   const Variant & val)
{
    for (int i = 0; i < serz.Size(); ++i)
    {
        Variant attrib = serz[i]->GetAttribute(attrib_name);
        Variant * final_attrib = Get_Attrib_Variant(attrib, nested_attrib_names);
        *final_attrib = val;
        serz[i]->SetAttribute(attrib_name, attrib);
    }
}

void Component_Widget::update_tw_from_node()
{
    auto iter = updaters.begin();
    while (iter != updaters.end())
    {
        Vector<Variant> values;
        for (int i = 0; i < iter->serz.Size(); ++i)
        {
            Variant attrib = iter->serz[i]->GetAttribute(iter->attrib_name);
            VariantVector nested_names(iter->nested_names);
            Variant * nested_val = Get_Attrib_Variant(attrib, nested_names);
            values.Push(*nested_val);
        }
        do_set_widget(&iter.value(), values);
        ++iter;
    }
}

void Component_Widget::create_tw_item(const Vector<Serializable *> & serz,
                                      const String & attrib_name,
                                      const String & nested_name,
                                      VariantVector & nested_attrib_names,
                                      const Vector<Variant> & values,
                                      QTreeWidgetItem * parent,
                                      const AttributeInfo & att_inf)
{
    VariantType vtype = values[0].GetType();

    QTreeWidgetItem * tw_item = new QTreeWidgetItem;
    tw_item->setText(0, nested_name.CString());
    QWidget * item_widget = nullptr;
    Selected_Attrib_Desc desc;
    bool set_widget = true;

    switch (vtype)
    {
    case (VAR_VARIANTVECTOR):
    {
        const VariantVector & fvv = values[0].GetVariantVector();
        int size = fvv.Size();
        for (int i = 1; i < values.Size(); ++i)
        {
            const VariantVector & vv = values[i].GetVariantVector();
            if (vv.Size() < size)
                size = vv.Size();
        }

        tw_item->setText(
            0, tw_item->text(0) + QString(" (") + QString::number(size) + QString(" Items)"));
        for (int i = 0; i < size; ++i)
        {
            VariantVector vals;
            for (int j = 0; j < values.Size(); ++j)
            {
                const VariantVector & vv = values[j].GetVariantVector();
                vals.Push(vv[i]);
            }
            nested_attrib_names.Push(i);
            std::stringstream ss;
            ss << attrib_name.CString() << "[" << i << "]";
            create_tw_item(serz,
                           attrib_name,
                           ss.str().c_str(),
                           nested_attrib_names,
                           vals,
                           tw_item,
                           AttributeInfo());
            nested_attrib_names.Pop();
        }
        break;
    }
    case (VAR_VARIANTMAP):
    {
        VariantMap fvm = values[0].GetVariantMap();
        for (int i = 1; i < values.Size(); ++i)
        {
            const VariantMap & vm = values[i].GetVariantMap();
            auto fvm_iter = fvm.Begin();
            while (fvm_iter != fvm.End())
            {
                auto find_iter = vm.Find(fvm_iter->first_);
                if (find_iter == fvm.End())
                    fvm_iter = fvm.Erase(fvm_iter);
                else
                    ++fvm_iter;
            }
        }
        tw_item->setText(
            0, tw_item->text(0) + QString(" (") + QString::number(fvm.Size()) + QString(" Items)"));
        auto vm_iter = fvm.Begin();
        while (vm_iter != fvm.End())
        {
            VariantVector vals;
            for (int i = 0; i < values.Size(); ++i)
            {
                const VariantMap & vm = values[i].GetVariantMap();
                auto fiter = vm.Find(vm_iter->first_);
                vals.Push(fiter->second_);
            }
            nested_attrib_names.Push(vm_iter->first_);
            create_tw_item(serz,
                           attrib_name,
                           vm_iter->first_.Reverse(),
                           nested_attrib_names,
                           vals,
                           tw_item,
                           AttributeInfo());
            nested_attrib_names.Pop();
            ++vm_iter;
        }
        break;
    }
    case (VAR_INT):
        item_widget = create_int_widget_item(
            Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (VAR_PTR):
        eout << "Cannot generate widget for pointer!";
        break;
    case (VAR_BOOL):
        item_widget = create_bool_widget_item(
            Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (VAR_NONE):
        eout << "No variable type found for dialog";
        break;
    case (VAR_RECT):
        break;
    case (VAR_STRING):
        item_widget = create_string_widget_item(
            Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (VAR_COLOR):
        break;
    case (VAR_FLOAT):
        item_widget = create_float_widget_item(
            Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (VAR_INT64):
        item_widget = create_int_widget_item(
            Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (VAR_INTRECT):
        item_widget = create_irect_widget_item(
            Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (VAR_MATRIX3):
        break;
    case (VAR_MATRIX4):
        break;
    case (VAR_MATRIX3X4):
        break;
    case (VAR_VECTOR2):
        item_widget = create_vec2_widget_item(
            Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (VAR_VECTOR3):
        item_widget = create_vec3_widget_item(
            Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (VAR_VECTOR4):
        item_widget = create_vec4_widget_item(
            Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (VAR_DOUBLE):
        item_widget = create_double_widget_item(
            Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (VAR_INTVECTOR2):
        desc.attrib_info = att_inf;
        desc.attrib_name = attrib_name;
        desc.attrib_values = values;
        desc.nested_names = nested_attrib_names;
        desc.serz = serz;
        set_widget = false;
        tw_item->setData(1, Qt::EditRole, QVariant::fromValue(desc));
        break;
    case (VAR_INTVECTOR3):
        item_widget = create_ivec3_widget_item(
            Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (VAR_STRINGVECTOR):
    {
        // const StringVector & vv = value.GetStringVector();
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
    case (VAR_QUATERNION):
        item_widget = create_quat_widget_item(
            Create_Widget_Params(serz, attrib_name, nested_attrib_names, values, att_inf));
        break;
    case (VAR_VOIDPTR):
        break;
    case (VAR_BUFFER):
        break;
    case (VAR_CUSTOM_HEAP):
        break;
    case (VAR_CUSTOM_STACK):
        break;
    case (VAR_RESOURCEREF):
        break;
    case (VAR_RESOURCEREFLIST):
    {
        // const ResourceRefList & vv = value.GetResourceRefList();
        // tw_item->setText(0, tw_item->text(0) + QString(" (") + QString::number(vv.names_.Size()) + QString(" Items)"));
        // for (int i = 0; i < vv.names_.Size(); ++i)
        // {
        //     ResourceRef ref;
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

    if (item_widget != nullptr)
        tw_->setItemWidget(tw_item, 1, item_widget);
}

void Component_Widget::add_node_to_treewidget(const Vector<Node *> nodes)
{
    if (nodes.Empty())
        return;

    QTreeWidgetItem * node_info_root = new QTreeWidgetItem;

    Vector<Variant> names;
    Vector<Variant> enables;
    Vector<Serializable *> serz;
    for (int i = 0; i < nodes.Size(); ++i)
    {
        names.Push(nodes[i]->GetName());
        enables.Push(nodes[i]->IsEnabled());
        serz.Push(nodes[i]);
    }

    QWidget * name_widget = create_string_widget_item(
        Create_Widget_Params(serz, "Name", VariantVector(), names, AttributeInfo()));
    QWidget * enabled_widget = create_bool_widget_item(
        Create_Widget_Params(serz, "Is Enabled", VariantVector(), enables, AttributeInfo()));

    tw_->addTopLevelItem(node_info_root);
    tw_->setItemWidget(node_info_root, 0, name_widget);
    tw_->setItemWidget(node_info_root, 1, enabled_widget);
    auto node_attribs = nodes[0]->GetAttributes();
    if (node_attribs != nullptr)
    {
        for (int att_ind = 0; att_ind < node_attribs->Size(); ++att_ind)
        {
            String var_name = (*node_attribs)[att_ind].name_;
            if (var_name == "Name" || var_name == "Is Enabled" || var_name.Contains("Network"))
                continue;
            VariantVector nested_names;
            Vector<Variant> values;
            bool create_item = true;

            for (int i = 0; i < serz.Size(); ++i)
            {
                Variant val = nodes[i]->GetAttribute(var_name);
                if (val.IsEmpty())
                {
                    create_item = false;
                    break;
                }
                values.Push(val);
            }
            if (create_item)
                create_tw_item(serz,
                               var_name,
                               var_name,
                               nested_names,
                               values,
                               nullptr,
                               (*node_attribs)[att_ind]);
        }
    }

    // Add the components to the tree widget
    const Vector<SharedPtr<Component>> & all_comps = nodes[0]->GetComponents();
    for (int i = 0; i < all_comps.Size(); ++i)
    {
        bool skip_component = false;
        StringHash comp_type = all_comps[i]->GetType();
        String comp_type_string = all_comps[i]->GetTypeName();

        Vector<Serializable *> comp_serz;
        Vector<Variant> enables;
        for (int ndi = 0; ndi < nodes.Size(); ++ndi)
        {
            Component * comp = nodes[ndi]->GetComponent(comp_type);
            if (comp == nullptr)
            {
                skip_component = true;
                iout << "Skipping component:" << comp_type_string;
                break;
            }
            enables.Push(comp->IsEnabled());
            comp_serz.Push(comp);
        }

        if (skip_component)
            continue;

        QTreeWidgetItem * tw_item = new QTreeWidgetItem;

        QToolButton * remove_comp_widget = new QToolButton(this);
        remove_comp_widget->setIcon(QIcon(":/VecTB/trash_icon"));
        QWidget * comp_cb = create_bool_widget_item(Create_Widget_Params(
            comp_serz, "Is Enabled", VariantVector(), enables, AttributeInfo()));
        QWidget * comp_widget = new QWidget;
        QHBoxLayout * comp_layout = new QHBoxLayout;
        comp_layout->setMargin(0);
        comp_layout->addWidget(comp_cb);
        comp_layout->addWidget(remove_comp_widget);
        comp_widget->setLayout(comp_layout);

        auto remove_func = [=]() {
            int result = QMessageBox::question(
                this, "Remove Component", "Remove " + QString(comp_type_string.CString()));
            if (result == QMessageBox::Yes)
            {
                for (int ndi = 0; ndi < nodes.Size(); ++ndi)
                {
                    Component * comp = nodes[ndi]->GetComponent(all_comps[i]->GetType());
                    nodes[ndi]->RemoveComponent(comp);
                }
            }
            reset_table();
        };
        connect(remove_comp_widget, &QToolButton::clicked, remove_func);

        Component * comp = all_comps[i];
        tw_item->setText(0, comp->GetTypeName().CString());

        auto attribs = all_comps[i]->GetAttributes();
        if (attribs != nullptr)
        {
            for (int j = 0; j < attribs->Size(); ++j)
            {
                const AttributeInfo & cur_att_inf = (*attribs)[j];
                String var_name = cur_att_inf.name_;
                if ((cur_att_inf.mode_ & AM_NOEDIT) == AM_NOEDIT || var_name == "Is Enabled")
                    continue;

                bool add_widget = true;
                VariantVector nested_names;
                Vector<Variant> vals;
                for (int ci = 0; ci < comp_serz.Size(); ++ci)
                {
                    Variant val = comp_serz[ci]->GetAttribute(var_name);
                    if (val.IsEmpty())
                    {
                        add_widget = false;
                        break;
                    }
                    vals.Push(val);
                }
                if (add_widget)
                    create_tw_item(
                        comp_serz, var_name, var_name, nested_names, vals, tw_item, cur_att_inf);
            }
            tw_->addTopLevelItem(tw_item);
            tw_->setItemWidget(tw_item, 1, comp_widget);
        }
    }
    node_info_root->setExpanded(true);
}

void Component_Widget::do_set_widget(Selected_Attrib_Desc * fd, const Vector<Variant> & values)
{
    if (values == fd->attrib_values)
        return;

    fd->attrib_values = values;

    if (values.Empty())
        return;

    bool pass_value = true;
    Variant val(values[0]);
    if (val.GetType() == VAR_VECTOR2)
    {
        fvec2 val_vec = val.GetVector2();
        float max_f = std::numeric_limits<float>::max();
        for (int i = 1; i < values.Size(); ++i)
        {
            fvec2 cur_vec = values[i].GetVector2();
            fvec2 prev_vec = values[i - 1].GetVector2();
            if (!Equals(cur_vec.x_, max_f) && !Equals(cur_vec.x_, prev_vec.x_))
                val_vec.x_ = max_f;
            if (!Equals(cur_vec.y_, max_f) && !Equals(cur_vec.y_, prev_vec.y_))
                val_vec.y_ = max_f;
        }
        val = val_vec;
    }
    else if (val.GetType() == VAR_INTVECTOR2)
    {
        ivec2 val_vec = val.GetIntVector2();
        int max_f = std::numeric_limits<int>::max();
        for (int i = 1; i < values.Size(); ++i)
        {
            ivec2 cur_vec = values[i].GetIntVector2();
            ivec2 prev_vec = values[i - 1].GetIntVector2();
            if (cur_vec.x_ != max_f && cur_vec.x_ != prev_vec.x_)
                val_vec.x_ = max_f;
            if (cur_vec.y_ != max_f && cur_vec.y_ != prev_vec.y_)
                val_vec.y_ = max_f;
        }
        val = val_vec;
    }
    else if (val.GetType() == VAR_VECTOR3)
    {
        fvec3 val_vec = val.GetVector3();
        float max_f = std::numeric_limits<float>::max();
        for (int i = 1; i < values.Size(); ++i)
        {
            fvec3 cur_vec = values[i].GetVector3();
            fvec3 prev_vec = values[i - 1].GetVector3();
            if (!Equals(cur_vec.x_, max_f) && !Equals(cur_vec.x_, prev_vec.x_))
                val_vec.x_ = max_f;
            if (!Equals(cur_vec.y_, max_f) && !Equals(cur_vec.y_, prev_vec.y_))
                val_vec.y_ = max_f;
            if (!Equals(cur_vec.z_, max_f) && !Equals(cur_vec.z_, prev_vec.z_))
                val_vec.z_ = max_f;
        }
        val = val_vec;
    }
    else if (val.GetType() == VAR_INTVECTOR3)
    {
        ivec3 val_vec = val.GetIntVector3();
        int max_f = std::numeric_limits<int>::max();
        for (int i = 1; i < values.Size(); ++i)
        {
            ivec3 cur_vec = values[i].GetIntVector3();
            ivec3 prev_vec = values[i - 1].GetIntVector3();
            if (cur_vec.x_ != max_f && cur_vec.x_ != prev_vec.x_)
                val_vec.x_ = max_f;
            if (cur_vec.y_ != max_f && cur_vec.y_ != prev_vec.y_)
                val_vec.y_ = max_f;
            if (cur_vec.z_ != max_f && cur_vec.z_ != prev_vec.z_)
                val_vec.z_ = max_f;
        }
        val = val_vec;
    }
    else if (val.GetType() == VAR_VECTOR4)
    {
        fvec4 val_vec = val.GetVector4();
        float max_f = std::numeric_limits<float>::max();
        for (int i = 1; i < values.Size(); ++i)
        {
            fvec4 cur_vec = values[i].GetVector4();
            fvec4 prev_vec = values[i - 1].GetVector4();
            if (!Equals(cur_vec.x_, max_f) && !Equals(cur_vec.x_, prev_vec.x_))
                val_vec.x_ = max_f;
            if (!Equals(cur_vec.y_, max_f) && !Equals(cur_vec.y_, prev_vec.y_))
                val_vec.y_ = max_f;
            if (!Equals(cur_vec.z_, max_f) && !Equals(cur_vec.z_, prev_vec.z_))
                val_vec.z_ = max_f;
            if (!Equals(cur_vec.w_, max_f) && !Equals(cur_vec.w_, prev_vec.w_))
                val_vec.w_ = max_f;
        }
        val = val_vec;
    }
    else if (val.GetType() == VAR_INTRECT)
    {
        IntRect val_vec = val.GetIntRect();
        int max_f = std::numeric_limits<int>::max();
        for (int i = 1; i < values.Size(); ++i)
        {
            IntRect cur_vec = values[i].GetIntRect();
            IntRect prev_vec = values[i - 1].GetIntRect();
            if (cur_vec.left_ != max_f && cur_vec.left_ != prev_vec.left_)
                val_vec.left_ = max_f;
            if (cur_vec.bottom_ != max_f && cur_vec.bottom_ != prev_vec.bottom_)
                val_vec.bottom_ = max_f;
            if (cur_vec.right_ != max_f && cur_vec.right_ != prev_vec.right_)
                val_vec.right_ = max_f;
            if (cur_vec.top_ != max_f && cur_vec.top_ != prev_vec.top_)
                val_vec.top_ = max_f;
        }
        val = val_vec;
    }
    else if (val.GetType() == VAR_QUATERNION)
    {
        Quaternion quat = val.GetQuaternion();
        fvec3 val_vec = quat.EulerAngles();

        float max_f = std::numeric_limits<float>::max();
        for (int i = 1; i < values.Size(); ++i)
        {
            Quaternion cur_quat = values[i].GetQuaternion();
            Quaternion prev_quat = values[i - 1].GetQuaternion();
            fvec3 cur_vec = cur_quat.EulerAngles();
            fvec3 prev_vec = prev_quat.EulerAngles();
            if (!Equals(cur_vec.x_, max_f) && !Equals(cur_vec.x_, prev_vec.x_))
                val_vec.x_ = max_f;
            if (!Equals(cur_vec.y_, max_f) && !Equals(cur_vec.y_, prev_vec.y_))
                val_vec.y_ = max_f;
            if (!Equals(cur_vec.z_, max_f) && !Equals(cur_vec.z_, prev_vec.z_))
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
                val = Variant();
                break;
            }
        }
    }
    fd->set_widget_value(val);
}

void Component_Widget::create_component_on_selected_node(const StringHash & comp_type)
{
    if (selection_.Empty())
        return;

    dout << "Creating" << comp_type.Reverse();
    for (int i = 0; i < selection_.Size(); ++i)
        selection_[i]->CreateComponent(comp_type);

    reset_table();
}

QWidget * Component_Widget::create_string_widget_item(Create_Widget_Params params)
{
    QLineEdit * item = new QLineEdit;

    Selected_Attrib_Desc fd(
        params.top_level_attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Variant & var) {
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

    auto func = [=]() {
        Slot_Callback(params.serz_,
                      params.nested_attrib_names_,
                      params.top_level_attrib_name_,
                      item->text().toStdString().c_str());
    };
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

        Selected_Attrib_Desc fd(
            params.top_level_attrib_name_, params.nested_attrib_names_, params.serz_);
        fd.set_widget_value = [=](const Variant & var) {
            item->blockSignals(true);
            if (var.IsEmpty())
            {
                if (item->itemData(0).toInt() != 22)
                    item->insertItem(0, "-- --", 22);
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
            Slot_Callback(
                params.serz_, params.nested_attrib_names_, params.top_level_attrib_name_, new_val);
        };

        QObject::connect(item, qOverload<int>(&QComboBox::currentIndexChanged), func);

        return item;
    }
    else
    {
        QSpinBox * item = new QSpinBox;
        item->setMaximum(std::numeric_limits<int>::max());
        item->setMinimum(std::numeric_limits<int>::min());
        item->setSpecialValueText("-- --");

        Selected_Attrib_Desc fd(
            params.top_level_attrib_name_, params.nested_attrib_names_, params.serz_);
        fd.set_widget_value = [=](const Variant & var) {
            item->blockSignals(true);
            if (var.IsEmpty())
                item->setValue(item->minimum());
            else
                item->setValue(var.GetInt());
            item->blockSignals(false);
        };
        updaters[item] = fd;
        do_set_widget(&fd, params.values_);

        auto func = [=](int new_val) {
            Slot_Callback(
                params.serz_, params.nested_attrib_names_, params.top_level_attrib_name_, new_val);
        };

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

    Selected_Attrib_Desc fd(
        params.top_level_attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Variant & var) {
        item->blockSignals(true);
        if (var.IsEmpty())
            item->setValue(item->minimum());
        else
            item->setValue(var.GetFloat());
        item->blockSignals(false);
    };
    updaters[item] = fd;
    do_set_widget(&fd, params.values_);

    auto func = [=](float new_val) {
        Slot_Callback(
            params.serz_, params.nested_attrib_names_, params.top_level_attrib_name_, new_val);
    };

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
    Selected_Attrib_Desc fd(
        params.top_level_attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Variant & var) {
        item->blockSignals(true);
        if (var.IsEmpty())
            item->setValue(item->minimum());
        else
            item->setValue(var.GetDouble());
    };
    item->blockSignals(false);
    updaters[item] = fd;
    do_set_widget(&fd, params.values_);
    fd.set_widget_value(params.values_);

    auto func = [=](double new_val) {
        Slot_Callback(
            params.serz_, params.nested_attrib_names_, params.top_level_attrib_name_, new_val);
    };

    QObject::connect(item, qOverload<double>(&QDoubleSpinBox::valueChanged), func);
    return item;
}

QWidget * Component_Widget::create_bool_widget_item(Create_Widget_Params params)
{
    QCheckBox * item = new QCheckBox;
    item->setTristate(true);

    Selected_Attrib_Desc fd(
        params.top_level_attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Variant & var) {
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
        Slot_Callback(params.serz_,
                      params.nested_attrib_names_,
                      params.top_level_attrib_name_,
                      state != Qt::Unchecked);
    };

    QObject::connect(item, &QCheckBox::stateChanged, func);
    return item;
}

QWidget * Component_Widget::create_vec2_widget_item(Create_Widget_Params params)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
    layout->setMargin(0);

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

    sb_x->setMinimumWidth(SB_MIN_WIDTH);
    sb_y->setMinimumWidth(SB_MIN_WIDTH);

    layout->addWidget(sb_x);
    layout->addWidget(sb_y);
    item->setLayout(layout);

    sb_x->setSpecialValueText("--");
    sb_y->setSpecialValueText("--");

    Selected_Attrib_Desc fd(
        params.top_level_attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Variant & var) {
        fvec2 vec = var.GetVector2();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        float max_f = std::numeric_limits<float>::max();
        if (var.IsEmpty())
        {
            sb_x->setValue(sb_x->minimum());
            sb_y->setValue(sb_y->minimum());
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
        }
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
    };
    updaters[item] = fd;
    do_set_widget(&fd, params.values_);

    auto func_x = [=](double new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_x->value() == sb_x->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            fvec2 vec = final_attrib->GetVector2();
            vec.x_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    auto func_y = [=](double new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_y->value() == sb_y->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            fvec2 vec = final_attrib->GetVector2();
            vec.y_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    QObject::connect(sb_x, qOverload<double>(&QDoubleSpinBox::valueChanged), func_x);
    QObject::connect(sb_y, qOverload<double>(&QDoubleSpinBox::valueChanged), func_y);

    return item;
}

QWidget * Component_Widget::create_vec3_widget_item(Create_Widget_Params params)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
    layout->setMargin(0);

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

    sb_x->setMinimumWidth(SB_MIN_WIDTH);
    sb_y->setMinimumWidth(SB_MIN_WIDTH);
    sb_z->setMinimumWidth(SB_MIN_WIDTH);

    layout->addWidget(sb_x);
    layout->addWidget(sb_y);
    layout->addWidget(sb_z);
    layout->addStretch();
    item->setLayout(layout);

    sb_x->setSpecialValueText("--");
    sb_y->setSpecialValueText("--");
    sb_z->setSpecialValueText("--");

    Selected_Attrib_Desc fd(
        params.top_level_attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Variant & var) {
        fvec3 vec = var.GetVector3();
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
    do_set_widget(&fd, params.values_);

    auto func_x = [=](double new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_x->value() == sb_x->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            fvec3 vec = final_attrib->GetVector3();
            vec.x_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    auto func_y = [=](double new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_y->value() == sb_y->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            fvec3 vec = final_attrib->GetVector3();
            vec.y_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    auto func_z = [=](double new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_z->value() == sb_z->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            fvec3 vec = final_attrib->GetVector3();
            vec.z_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
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

    QSpinBox * sb_x = new QSpinBox();
    QSizePolicy pc = sb_x->sizePolicy();
    pc.setHorizontalPolicy(QSizePolicy::Maximum);
    sb_x->setMaximum(std::numeric_limits<int>::max());
    sb_x->setMinimum(-1.0 * std::numeric_limits<int>::max());
    sb_x->setSizePolicy(pc);

    QSpinBox * sb_y = new QSpinBox();
    sb_y->setMaximum(std::numeric_limits<int>::max());
    sb_y->setMinimum(-1.0 * std::numeric_limits<int>::max());
    sb_y->setSizePolicy(pc);

    sb_x->setMinimumWidth(SB_MIN_WIDTH);
    sb_y->setMinimumWidth(SB_MIN_WIDTH);

    layout->addWidget(sb_x);
    layout->addWidget(sb_y);
    item->setLayout(layout);

    sb_x->setSpecialValueText("--");
    sb_y->setSpecialValueText("--");

    Selected_Attrib_Desc fd(
        params.top_level_attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Variant & var) {
        ivec2 vec = var.GetIntVector2();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        int max_f = std::numeric_limits<int>::max();
        if (var.IsEmpty())
        {
            sb_x->setValue(sb_x->minimum());
            sb_y->setValue(sb_y->minimum());
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
        }
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
    };
    updaters[item] = fd;
    do_set_widget(&fd, params.values_);

    auto func_x = [=](int new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_x->value() == sb_x->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            ivec2 vec = final_attrib->GetIntVector2();
            vec.x_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    auto func_y = [=](int new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_y->value() == sb_y->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            ivec2 vec = final_attrib->GetIntVector2();
            vec.y_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    QObject::connect(sb_x, qOverload<int>(&QSpinBox::valueChanged), func_x);
    QObject::connect(sb_y, qOverload<int>(&QSpinBox::valueChanged), func_y);

    return item;
}

QWidget * Component_Widget::create_ivec3_widget_item(Create_Widget_Params params)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
    layout->setMargin(0);

    QSpinBox * sb_x = new QSpinBox();
    QSizePolicy pc = sb_x->sizePolicy();
    pc.setHorizontalPolicy(QSizePolicy::Maximum);
    sb_x->setMaximum(std::numeric_limits<int>::max());
    sb_x->setMinimum(-1.0 * std::numeric_limits<int>::max());
    sb_x->setSizePolicy(pc);

    QSpinBox * sb_y = new QSpinBox();
    sb_y->setMaximum(std::numeric_limits<int>::max());
    sb_y->setMinimum(-1.0 * std::numeric_limits<int>::max());
    sb_y->setSizePolicy(pc);

    QSpinBox * sb_z = new QSpinBox();
    sb_z->setMaximum(std::numeric_limits<int>::max());
    sb_z->setMinimum(-1.0 * std::numeric_limits<int>::max());
    sb_z->setSizePolicy(pc);

    sb_x->setMinimumWidth(SB_MIN_WIDTH);
    sb_y->setMinimumWidth(SB_MIN_WIDTH);
    sb_z->setMinimumWidth(SB_MIN_WIDTH);

    layout->addWidget(sb_x);
    layout->addWidget(sb_y);
    layout->addWidget(sb_z);
    item->setLayout(layout);

    sb_x->setSpecialValueText("--");
    sb_y->setSpecialValueText("--");
    sb_z->setSpecialValueText("--");

    Selected_Attrib_Desc fd(
        params.top_level_attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Variant & var) {
        ivec3 vec = var.GetIntVector3();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_z->blockSignals(true);
        int max_f = std::numeric_limits<int>::max();
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
    do_set_widget(&fd, params.values_);

    auto func_x = [=](int new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_x->value() == sb_x->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            ivec3 vec = final_attrib->GetIntVector3();
            vec.x_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    auto func_y = [=](int new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_y->value() == sb_y->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            ivec3 vec = final_attrib->GetIntVector3();
            vec.y_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    auto func_z = [=](int new_val) {
        fvec3 val;
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_z->value() == sb_z->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            ivec3 vec = final_attrib->GetIntVector3();
            vec.z_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    QObject::connect(sb_x, qOverload<int>(&QSpinBox::valueChanged), func_x);
    QObject::connect(sb_y, qOverload<int>(&QSpinBox::valueChanged), func_y);
    QObject::connect(sb_z, qOverload<int>(&QSpinBox::valueChanged), func_z);

    return item;
}

QWidget * Component_Widget::create_vec4_widget_item(Create_Widget_Params params)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
    layout->setMargin(0);

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
    sb_w->setMaximum(std::numeric_limits<float>::max());
    sb_w->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_w->setSingleStep(0.1);
    sb_w->setSizePolicy(pc);

    sb_x->setMinimumWidth(SB_MIN_WIDTH);
    sb_y->setMinimumWidth(SB_MIN_WIDTH);
    sb_z->setMinimumWidth(SB_MIN_WIDTH);
    sb_w->setMinimumWidth(SB_MIN_WIDTH);

    layout->addWidget(sb_x);
    layout->addWidget(sb_y);
    layout->addWidget(sb_z);
    layout->addWidget(sb_w);
    item->setLayout(layout);

    sb_x->setSpecialValueText("--");
    sb_y->setSpecialValueText("--");
    sb_z->setSpecialValueText("--");
    sb_z->setSpecialValueText("--");

    Selected_Attrib_Desc fd(
        params.top_level_attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Variant & var) {
        fvec4 vec = var.GetVector4();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_z->blockSignals(true);
        sb_w->blockSignals(true);
        float max_f = std::numeric_limits<float>::max();
        if (var.IsEmpty())
        {
            sb_x->setValue(sb_x->minimum());
            sb_y->setValue(sb_y->minimum());
            sb_z->setValue(sb_z->minimum());
            sb_w->setValue(sb_w->minimum());
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
            if (vec.w_ == max_f)
                sb_w->setValue(sb_w->minimum());
            else
                sb_w->setValue(vec.w_);
        }
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
        sb_z->blockSignals(false);
        sb_w->blockSignals(false);
    };
    updaters[item] = fd;
    do_set_widget(&fd, params.values_);

    auto func_x = [=](double new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_x->value() == sb_x->minimum())
                return;

            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            fvec4 vec = final_attrib->GetVector4();
            vec.x_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    auto func_y = [=](double new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_y->value() == sb_y->minimum())
                return;

            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            fvec4 vec = final_attrib->GetVector4();
            vec.y_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    auto func_z = [=](double new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_z->value() == sb_z->minimum())
                return;

            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            fvec4 vec = final_attrib->GetVector4();
            vec.z_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    auto func_w = [=](double new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_w->value() == sb_w->minimum())
                return;

            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            fvec4 vec = final_attrib->GetVector4();
            vec.w_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    QObject::connect(sb_x, qOverload<double>(&QDoubleSpinBox::valueChanged), func_x);
    QObject::connect(sb_y, qOverload<double>(&QDoubleSpinBox::valueChanged), func_y);
    QObject::connect(sb_z, qOverload<double>(&QDoubleSpinBox::valueChanged), func_z);
    QObject::connect(sb_w, qOverload<double>(&QDoubleSpinBox::valueChanged), func_w);

    return item;
}

QWidget * Component_Widget::create_irect_widget_item(Create_Widget_Params params)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
    layout->setMargin(0);

    QSpinBox * sb_x = new QSpinBox();
    QSizePolicy pc = sb_x->sizePolicy();
    pc.setHorizontalPolicy(QSizePolicy::Maximum);
    sb_x->setMaximum(std::numeric_limits<int>::max());
    sb_x->setMinimum(-1.0 * std::numeric_limits<int>::max());
    sb_x->setSizePolicy(pc);

    QSpinBox * sb_y = new QSpinBox();
    sb_y->setMaximum(std::numeric_limits<int>::max());
    sb_y->setMinimum(-1.0 * std::numeric_limits<int>::max());
    sb_y->setSizePolicy(pc);

    QSpinBox * sb_w = new QSpinBox();
    sb_w->setMaximum(std::numeric_limits<int>::max());
    sb_w->setMinimum(-1.0 * std::numeric_limits<int>::max());
    sb_w->setSizePolicy(pc);

    QSpinBox * sb_h = new QSpinBox();
    sb_h->setMaximum(std::numeric_limits<int>::max());
    sb_h->setMinimum(-1.0 * std::numeric_limits<int>::max());
    sb_h->setSizePolicy(pc);

    sb_x->setMinimumWidth(SB_MIN_WIDTH);
    sb_y->setMinimumWidth(SB_MIN_WIDTH);
    sb_w->setMinimumWidth(SB_MIN_WIDTH);
    sb_h->setMinimumWidth(SB_MIN_WIDTH);

    layout->addWidget(sb_x);
    layout->addWidget(sb_y);
    layout->addWidget(sb_w);
    layout->addWidget(sb_h);
    layout->addStretch();
    item->setLayout(layout);

    sb_x->setSpecialValueText("--");
    sb_y->setSpecialValueText("--");
    sb_w->setSpecialValueText("--");
    sb_h->setSpecialValueText("--");

    Selected_Attrib_Desc fd(
        params.top_level_attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Variant & var) {
        IntRect vec = var.GetIntRect();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_w->blockSignals(true);
        sb_h->blockSignals(true);
        int max_f = std::numeric_limits<int>::max();
        if (var.IsEmpty())
        {
            sb_x->setValue(sb_x->minimum());
            sb_y->setValue(sb_y->minimum());
            sb_w->setValue(sb_w->minimum());
            sb_h->setValue(sb_w->minimum());
        }
        else
        {
            if (vec.left_ == max_f)
                sb_x->setValue(sb_x->minimum());
            else
                sb_x->setValue(vec.left_);
            if (vec.bottom_ == max_f)
                sb_y->setValue(sb_y->minimum());
            else
                sb_y->setValue(vec.bottom_);
            if (vec.right_ == max_f)
                sb_w->setValue(sb_w->minimum());
            else
                sb_w->setValue(vec.right_ - vec.left_);
            if (vec.top_ == max_f)
                sb_w->setValue(sb_w->minimum());
            else
                sb_w->setValue(vec.top_ - vec.bottom_);
        }
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
        sb_w->blockSignals(false);
        sb_h->blockSignals(false);
    };
    updaters[item] = fd;
    do_set_widget(&fd, params.values_);

    auto func_x = [=](int new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_x->value() == sb_x->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            IntRect vec = final_attrib->GetIntRect();
            vec.left_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    auto func_y = [=](int new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_y->value() == sb_y->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            IntRect vec = final_attrib->GetIntRect();
            vec.bottom_ = new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    auto func_w = [=](int new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_w->value() == sb_w->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            IntRect vec = final_attrib->GetIntRect();
            vec.right_ = vec.left_ + new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    auto func_h = [=](int new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_h->value() == sb_h->minimum())
                return;

            Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
            VariantVector nested_vals_copy(params.nested_attrib_names_);
            Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
            IntRect vec = final_attrib->GetIntRect();
            vec.top_ = vec.bottom_ + new_val;
            *final_attrib = vec;
            params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
        }
    };

    QObject::connect(sb_x, qOverload<int>(&QSpinBox::valueChanged), func_x);
    QObject::connect(sb_y, qOverload<int>(&QSpinBox::valueChanged), func_y);
    QObject::connect(sb_w, qOverload<int>(&QSpinBox::valueChanged), func_w);
    QObject::connect(sb_h, qOverload<int>(&QSpinBox::valueChanged), func_h);

    return item;
}

QWidget * Component_Widget::create_quat_widget_item(Create_Widget_Params params)
{
    QWidget * item = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;
    layout->setMargin(0);

    QDoubleSpinBox * sb_x = new QDoubleSpinBox();
    QSizePolicy pc = sb_x->sizePolicy();
    pc.setHorizontalPolicy(QSizePolicy::Maximum);
    sb_x->setSizePolicy(pc);
    sb_x->setMaximum(std::numeric_limits<float>::max());
    sb_x->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_x->setSingleStep(1.0);

    QDoubleSpinBox * sb_y = new QDoubleSpinBox();
    sb_y->setSizePolicy(pc);
    sb_y->setMaximum(std::numeric_limits<float>::max());
    sb_y->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_y->setSingleStep(1.0);

    QDoubleSpinBox * sb_z = new QDoubleSpinBox();
    sb_z->setSizePolicy(pc);
    sb_z->setMaximum(std::numeric_limits<float>::max());
    sb_z->setMinimum(-1.0 * std::numeric_limits<float>::max());
    sb_z->setSingleStep(1.0);

    sb_x->setMinimumWidth(SB_MIN_WIDTH);
    sb_y->setMinimumWidth(SB_MIN_WIDTH);
    sb_z->setMinimumWidth(SB_MIN_WIDTH);

    layout->addWidget(sb_x);
    layout->addWidget(sb_y);
    layout->addWidget(sb_z);
    layout->addStretch();
    item->setLayout(layout);

    sb_x->setSpecialValueText("--");
    sb_y->setSpecialValueText("--");
    sb_z->setSpecialValueText("--");

    Selected_Attrib_Desc fd(
        params.top_level_attrib_name_, params.nested_attrib_names_, params.serz_);
    fd.set_widget_value = [=](const Variant & var) {
        fvec3 vec = var.GetVector3();
        sb_x->blockSignals(true);
        sb_y->blockSignals(true);
        sb_z->blockSignals(true);
        float max_f = std::numeric_limits<float>::max();
        if (var.IsEmpty())
        {
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
        sb_x->setProperty("prev_val", sb_x->value());
        sb_y->setProperty("prev_val", sb_y->value());
        sb_z->setProperty("prev_val", sb_z->value());
        sb_x->blockSignals(false);
        sb_y->blockSignals(false);
        sb_z->blockSignals(false);
    };
    updaters[item] = fd;
    do_set_widget(&fd, params.values_);

    auto func_x = [=](double new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_x->text() == "--")
                return;

            if (params.top_level_attrib_name_ == "Rotation")
            {
                double prev_val = sb_x->property("prev_val").toDouble();
                Node * nd = static_cast<Node *>(params.serz_[i]);
                nd->Pitch(new_val - prev_val, TS_WORLD);
                sb_x->setProperty("prev_val", new_val);
            }
            else
            {
                Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
                VariantVector nested_vals_copy(params.nested_attrib_names_);
                Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
                Quaternion quat(sb_x->value(), sb_y->value(), sb_z->value());
                quat.Normalize();
                *final_attrib = quat;
                params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
            }
        }
    };

    auto func_y = [=](double new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_y->text() == "--")
                return;

            if (params.top_level_attrib_name_ == "Rotation")
            {
                double prev_val = sb_y->property("prev_val").toDouble();
                Node * nd = static_cast<Node *>(params.serz_[i]);
                nd->Yaw(new_val - prev_val, TS_WORLD);
                sb_y->setProperty("prev_val", new_val);
            }
            else
            {
                Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
                VariantVector nested_vals_copy(params.nested_attrib_names_);
                Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
                Quaternion quat(sb_x->value(), sb_y->value(), sb_z->value());
                quat.Normalize();
                *final_attrib = quat;
                params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
            }
        }
    };

    auto func_z = [=](double new_val) {
        for (int i = 0; i < params.serz_.Size(); ++i)
        {
            if (sb_z->text() == "--")
                return;

            if (params.top_level_attrib_name_ == "Rotation")
            {
                double prev_val = sb_z->property("prev_val").toDouble();
                Node * nd = static_cast<Node *>(params.serz_[i]);
                nd->Roll(new_val - prev_val, TS_WORLD);
                sb_z->setProperty("prev_val", new_val);
            }
            else
            {
                Variant attrib = params.serz_[i]->GetAttribute(params.top_level_attrib_name_);
                VariantVector nested_vals_copy(params.nested_attrib_names_);
                Variant * final_attrib = Get_Attrib_Variant(attrib, nested_vals_copy);
                Quaternion quat(sb_x->value(), sb_y->value(), sb_z->value());
                quat.Normalize();
                *final_attrib = quat;
                params.serz_[i]->SetAttribute(params.top_level_attrib_name_, attrib);
            }
        }
    };

    QObject::connect(sb_x, qOverload<double>(&QDoubleSpinBox::valueChanged), func_x);
    QObject::connect(sb_y, qOverload<double>(&QDoubleSpinBox::valueChanged), func_y);
    QObject::connect(sb_z, qOverload<double>(&QDoubleSpinBox::valueChanged), func_z);

    return item;
}

Component_Widget::Component_Widget(QWidget * parent)
    : QWidget(nullptr), tw_(nullptr), type_delegate(new Urho_Type_Delagate)
{
    QVBoxLayout * layout = new QVBoxLayout;
    layout->setMargin(0);
    tw_ = new QTreeWidget;
    tw_->setItemDelegate(type_delegate);
    tw_->setHeaderHidden(true);
    tw_->setStyleSheet(TW_STYLE);
    tw_->setSelectionMode(QAbstractItemView::NoSelection);
    tw_->header()->setStretchLastSection(false);
    tw_->header()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(tw_);
    setLayout(layout);
}

Component_Widget::~Component_Widget()
{
    delete type_delegate;
}

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

void Component_Widget::clear_selection()
{
    selection_.Clear();
    reset_table();
}

void Component_Widget::reset_table()
{
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
    add_node_to_treewidget(selection_);

    // Restore expanded items
    for (int i = 0; i < tw_->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem * cur_item = tw_->topLevelItem(i);
        recursive_tree_widget_check(cur_item, true);
    }
}

void Component_Widget::setup_ui(const Vector<Node *> & nodes)
{
    if (selection_ == nodes || nodes.Empty())
        return;

    selection_ = nodes;
    reset_table();
}
