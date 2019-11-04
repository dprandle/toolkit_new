#include <details_view.h>
#include <ui_details_view.h>
#include <toolkit.h>
#include <mtdebug_print.h>

#include <urho_common.h>

#include <QMessageBox>
#include <QMenu>
#include <QToolButton>
#include <QToolBar>


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
                StringHash type(find_iter->first_);
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

void Details_View::set_selected_data(const VariantVector & data, const StringHash & type)
{
    if (type == Node::GetTypeStatic())
    {
        Vector<Node*> nodes;
        nodes.Resize(data.Size());
        for (int i = 0; i < data.Size(); ++i)
        {
            Node * nd = static_cast<Node*>(data[i].GetVoidPtr());
            nodes[i] = nd;
        }
        ui->comp_widget->setup_ui(nodes);
    }
}

void Details_View::clear_selection()
{
    ui->comp_widget->clear_selection();
}

Details_View::~Details_View()
{
    delete ui;
}

void Details_View::update_node()
{
    ui->comp_widget->update_tw_from_node();
}