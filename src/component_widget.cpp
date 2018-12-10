#include <component_widget.h>
#include <mtdebug_print.h>
#include <toolkit.h>
#include <prefab_editor_dialog.h>

#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/Variant.h>

#include <QPushButton>
#include <QTreeWidgetItem>
#include <QLineEdit>
#include <QTreeWidget>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QSpacerItem>

void create_tw_item(Urho3D::Serializable * ser,
                    const Urho3D::String & var_name,
                    QTreeWidget * tw,
                    QTreeWidgetItem * parent)
{
    Urho3D::Variant value = ser->GetAttribute(var_name);
    Urho3D::VariantType vtype = value.GetType();

    QTreeWidgetItem * tw_item = new QTreeWidgetItem;
    tw_item->setText(0, var_name.CString());
    QWidget * item_widget = nullptr;

    switch (vtype)
    {
    case (Urho3D::VAR_VARIANTVECTOR):
        break;
    case (Urho3D::VAR_VARIANTMAP):
        break;
    case (Urho3D::VAR_INT):
        item_widget = create_widget_item(ser, var_name, value.GetInt());
        break;
    case (Urho3D::VAR_PTR):
        break;
    case (Urho3D::VAR_BOOL):
        item_widget = create_widget_item(ser, var_name, value.GetBool());
        break;
    case (Urho3D::VAR_NONE):
        break;
    case (Urho3D::VAR_RECT):
        break;
    case (Urho3D::VAR_STRING):
        item_widget = create_widget_item(ser, var_name, value.GetString());
        break;
    case (Urho3D::VAR_COLOR):
        break;
    case (Urho3D::VAR_FLOAT):
        item_widget = create_widget_item(ser, var_name, value.GetFloat());
        break;
    case (Urho3D::VAR_INT64):
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
    node_dummy_root->setText(0,"Node Information");

    QFont fnt = node_dummy_root->font(0);
    fnt.setBold(true);
    fnt.setPointSize(fnt.pointSize()+1);
    node_dummy_root->setFont(0, fnt);
    tw->addTopLevelItem(node_dummy_root);


    QTreeWidgetItem * node_info_root = new QTreeWidgetItem;


    QWidget * name_widget = create_widget_item(node, "Name", node->GetName());
    QWidget * enabled_widget = create_widget_item(node, "Is Enabled", node->IsEnabled());
    
    auto node_attribs = node->GetAttributes();
    if (node_attribs != nullptr)
    {
        for (int att_ind = 0; att_ind < node_attribs->Size(); ++att_ind)
        {
            Urho3D::String var_name = (*node_attribs)[att_ind].name_;
            if (var_name == "Name" || var_name == "Is Enabled")
                continue;
            create_tw_item(node, var_name, tw, node_info_root);
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
                create_tw_item(all_comps[i], var_name, tw, tw_item);
            }
            tw->addTopLevelItem(tw_item);
            tw->setItemWidget(tw_item, 1, cb_widget);
        }
    }
}

QWidget * create_widget_item(Urho3D::Serializable * serz,
                             const Urho3D::String & name,
                             const Urho3D::String & value)
{
    QLineEdit * item = new QLineEdit;
    item->setText(value.CString());
    auto func = [=]() {
        serz->SetAttribute(name, qPrintable(item->text()));
        bbtk.prefab_editor()->get_ui()->prefab_view->setFocus();
    };
    QObject::connect(item, &QLineEdit::editingFinished, func);
    return item;
}

QWidget * create_widget_item(Urho3D::Serializable * serz, const Urho3D::String & name, int value)
{
    QLineEdit * item = new QLineEdit;
    item->setText(QString::number(value));
    item->setValidator(new QIntValidator);
    auto func = [=]() {
        serz->SetAttribute(name, item->text().toInt());
        bbtk.prefab_editor()->get_ui()->prefab_view->setFocus();
    };
    QObject::connect(item, &QLineEdit::editingFinished, func);
    return item;
}

QWidget * create_widget_item(Urho3D::Serializable * serz, const Urho3D::String & name, float value)
{
    QLineEdit * item = new QLineEdit;
    item->setText(QString::number(value, 'f', 2));
    item->setValidator(new QDoubleValidator);
    auto func = [=]() {
        serz->SetAttribute(name, item->text().toFloat());
        bbtk.prefab_editor()->get_ui()->prefab_view->setFocus();
    };
    QObject::connect(item, &QLineEdit::editingFinished, func);
    return item;
}

QWidget * create_widget_item(Urho3D::Serializable * serz, const Urho3D::String & name, double value)
{
    QLineEdit * item = new QLineEdit;
    item->setText(QString::number(value, 'f', 4));
    item->setValidator(new QDoubleValidator);
    auto func = [=]() {
        serz->SetAttribute(name, item->text().toDouble());
        bbtk.prefab_editor()->get_ui()->prefab_view->setFocus();
    };
    QObject::connect(item, &QLineEdit::editingFinished, func);
    return item;
}

QWidget * create_widget_item(Urho3D::Serializable * serz, const Urho3D::String & name, bool value)
{
    QCheckBox * item = new QCheckBox;
    item->setText("");
    item->setChecked(value);
    auto func = [=](int state) {
        serz->SetAttribute(name, state == Qt::Checked);
        bbtk.prefab_editor()->get_ui()->prefab_view->setFocus();
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
    tw->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    tw->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    layout->addWidget(tw);
    setLayout(layout);
}
