#include <details_view.h>
#include <ui_details_view.h>

#include <QMessageBox>
#include <QMenu>
#include <QToolButton>
#include <QToolBar>

#include <toolkit.h>

#include <mtdebug_print.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Container/HashMap.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>

Details_View::Details_View(QWidget * parent) : QMainWindow(parent), ui(new Ui::Details_View)
{
    ui->setupUi(this);
    connect(&bbtk, &Toolkit::urho_init_complete, this, &Details_View::urho_init_complete);
}

void Details_View::urho_init_complete()
{
    auto cats = bbtk_ctxt->GetObjectCategories();
    auto vals = bbtk_ctxt->GetObjectFactories();

    QMenu *menu = new QMenu(this);
    QToolButton* toolButton = new QToolButton();
    toolButton->setText("Create");
    toolButton->setMenu(menu);
    toolButton->setPopupMode(QToolButton::InstantPopup);    
    ui->toolBar->addWidget(toolButton);
    
    auto iter = cats.Begin();
    while (iter != cats.End())
    {
        if (iter->first_ == "UI")
        {
            ++iter;
            continue;
        }

        QMenu * submenu = new QMenu(this);
        QAction * cat_action = menu->addMenu(submenu);
        cat_action->setText(iter->first_.CString());
        for (int i = 0; i < iter->second_.Size(); ++i)
        {
            auto find_iter = vals.Find(iter->second_[i]);
            if (find_iter != vals.End())
            {
                QAction * comp_action = new QAction(find_iter->second_->GetTypeName().CString(),this);
                Urho3D::StringHash type(find_iter->first_);
                auto func = [=]()
                {
                    ui->comp_widget->create_component_on_selected_node(type);
                };
                connect(comp_action, &QAction::triggered, ui->comp_widget, func);
                submenu->addAction(comp_action);
            }
        }
        ++iter;
    }
}

void Details_View::set_nodes(const Urho3D::Vector<Urho3D::Node *> & nodes)
{
    ui->comp_widget->setup_ui(nodes);
}

Details_View::~Details_View()
{
    delete ui;
}

void Details_View::update_node()
{
    ui->comp_widget->update_tw_from_node();
}