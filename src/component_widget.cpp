#include <component_widget.h>
#include <mtdebug_print.h>

#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/Variant.h>

#include <QTreeWidgetItem>
#include <QLineEdit>
#include <QTreeWidget>
#include <QCheckBox>
#include <QVBoxLayout>

void build_tree_widget(Urho3D::Node * node, QTreeWidget * tw)
{
    if (node == nullptr)
        return;

    tw->clear();
    QStringList sl;
    sl.append("Attribute");
    sl.append("Value");
    tw->setHeaderLabels(sl);

    const Urho3D::Vector<Urho3D::SharedPtr<Urho3D::Component>> & all_comps = node->GetComponents();
    for (int i = 0; i < all_comps.Size(); ++i)
    {
        QTreeWidgetItem * tw_item = new QTreeWidgetItem;
        tw_item->setText(0, all_comps[i]->GetTypeName().CString());
        auto attribs = all_comps[i]->GetAttributes();
        if (attribs != nullptr)
        {
            for (int j = 0; j < attribs->Size(); ++j)
            {
                if ( ((*attribs)[j].mode_ & Urho3D::AM_NOEDIT) == Urho3D::AM_NOEDIT)
                    continue;
                Urho3D::String var_name = (*attribs)[j].name_;
                Urho3D::Variant value = all_comps[i]->GetAttribute(var_name);
                Urho3D::VariantType vtype = value.GetType();
                switch (vtype)
                {
                case (Urho3D::VAR_VARIANTVECTOR):
                    break;
                case (Urho3D::VAR_VARIANTMAP):
                    break;
                case (Urho3D::VAR_INT):
                    create_widget_item(all_comps[i], tw, tw_item, var_name, value.GetInt());
                    break;
                case (Urho3D::VAR_PTR):
                    break;
                case (Urho3D::VAR_BOOL):
                    create_widget_item(all_comps[i], tw, tw_item, var_name, value.GetBool());
                    break;
                case (Urho3D::VAR_NONE):
                    break;
                case (Urho3D::VAR_RECT):
                    break;
                case (Urho3D::VAR_STRING):
                    create_widget_item(all_comps[i], tw, tw_item, var_name, value.GetString());
                    break;
                case (Urho3D::VAR_COLOR):
                    break;
                case (Urho3D::VAR_FLOAT):
                    create_widget_item(all_comps[i], tw, tw_item, var_name, value.GetFloat());
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
                    eout << "Scoobers";
                }

                tw->addTopLevelItem(tw_item);
            }
        }
    }
}

void create_widget_item(Urho3D::Component * comp,
                        QTreeWidget * tw,
                        QTreeWidgetItem * parent,
                        const Urho3D::String & name,
                        const Urho3D::String & value)
{
    QTreeWidgetItem * tw_item = new QTreeWidgetItem;
    tw_item->setText(0, name.CString());

    QLineEdit * item = new QLineEdit;
    item->setText(value.CString());
    auto func = [=]() { comp->SetAttribute(name, qPrintable(item->text())); dout << "Should be setting attribute for String"; comp->ApplyAttributes(); };
    QObject::connect(item, &QLineEdit::editingFinished, func);

    parent->addChild(tw_item);
    tw->setItemWidget(tw_item, 1, item);
}

void create_widget_item(Urho3D::Component * comp,
                        QTreeWidget * tw,
                        QTreeWidgetItem * parent,
                        const Urho3D::String & name,
                        int value)
{
    QTreeWidgetItem * tw_item = new QTreeWidgetItem;
    tw_item->setText(0, name.CString());

    QLineEdit * item = new QLineEdit;
    item->setText(QString::number(value));
    item->setValidator(new QIntValidator);
    auto func = [=]() { comp->SetAttribute(name, item->text().toInt()); dout << "Should be setting attribute for int"; comp->ApplyAttributes(); };
    QObject::connect(item, &QLineEdit::editingFinished, func);

    parent->addChild(tw_item);
    tw->setItemWidget(tw_item, 1, item);
}

void create_widget_item(Urho3D::Component * comp,
                        QTreeWidget * tw,
                        QTreeWidgetItem * parent,
                        const Urho3D::String & name,
                        float value)
{
    QTreeWidgetItem * tw_item = new QTreeWidgetItem;
    tw_item->setText(0, name.CString());

    QLineEdit * item = new QLineEdit;
    item->setText(QString::number(value,'f',2));
    item->setValidator(new QDoubleValidator);
    auto func = [=]() { comp->SetAttribute(name, item->text().toFloat()); comp->ApplyAttributes(); };
    QObject::connect(item, &QLineEdit::editingFinished, func);

    parent->addChild(tw_item);
    tw->setItemWidget(tw_item, 1, item);
}

void create_widget_item(Urho3D::Component * comp,
                        QTreeWidget * tw,
                        QTreeWidgetItem * parent,
                        const Urho3D::String & name,
                        double value)
{
    create_widget_item(comp, tw, parent, name, static_cast<float>(value));
}


void create_widget_item(Urho3D::Component * comp,
                        QTreeWidget * tw,
                        QTreeWidgetItem * parent,
                        const Urho3D::String & name,
                        bool value)
{
    QTreeWidgetItem * tw_item = new QTreeWidgetItem;
    tw_item->setText(0, name.CString());

    QCheckBox * item = new QCheckBox;
    item->setText("");
    item->setChecked(value);
    auto func = [=](int state) { comp->SetAttribute(name, state == Qt::Checked); dout << "Should be setting attribute for bool"; comp->ApplyAttributes();};
    QObject::connect(item, &QCheckBox::stateChanged, func);

    parent->addChild(tw_item);
    tw->setItemWidget(tw_item, 1, item);
}

Component_Widget::Component_Widget(QWidget * parent) : QWidget(nullptr)
{}

Component_Widget::~Component_Widget()
{}

void Component_Widget::setup_ui(Urho3D::Node * node)
{
    QVBoxLayout * layout = new QVBoxLayout;
    QTreeWidget * tw = new QTreeWidget;
    build_tree_widget(node, tw);
    layout->addWidget(tw);
    setLayout(layout);
}
